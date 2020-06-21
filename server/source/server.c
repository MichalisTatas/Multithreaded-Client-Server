#include "../include/server.h"

#define QSIZE 100

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t conditionVariable = PTHREAD_COND_INITIALIZER; 

int workerFunction(pthreadArguments* parameters, int descriptor)
{
    char* msg = msgComposer(descriptor, 20);
    int port = atoi(msg);

    workerInfoPtr worker = NULL;
    worker = addPortInList(worker, port);

    while (true) {
        if (!strcmp((msg = msgComposer(descriptor, 20)), "finished writing countries!"))
            break;
        addCountryInList(&worker->countriesList, msg);
    }
    pthread_mutex_lock(&mutex);
    worker->next = parameters->workersList;
    parameters->workersList = worker;
    pthread_mutex_unlock(&mutex);

    while (true) {
        if (!strcmp((msg = msgComposer(descriptor, 20)), "finished!"))
            break;
        printf("%s \n",msg);  // print the statistics or not?
    }
    return 0;
}

int clientFunction(pthreadArguments* parameters, int descriptor)
{
    char* msg = " ";

    while (true) {
        if (!strcmp((msg = msgComposer(descriptor, 20)), "finished!"))
            break;
        queriesHandler(parameters->workersList, msg, descriptor);
    }
    return 0;
}

void* threadFunction(void* argument)
{
    int descriptor;
    char* msg = " ";
    pthreadArguments* parameters = (pthreadArguments*)argument;
    while (true) {
        pthread_mutex_lock(&mutex);
        if ((descriptor = bufferRemove(parameters->circularBuff)) == -1) {
            pthread_cond_wait(&conditionVariable, &mutex);
            descriptor = bufferRemove(parameters->circularBuff);
        }
        pthread_mutex_unlock(&mutex);
        if (descriptor != -1) {
            msg = msgComposer(descriptor, 20);
            if (!strcmp(msg, "w"))
                workerFunction(parameters, descriptor);
            else if (!strcmp(msg, "c"))
                clientFunction(parameters, descriptor);
            else {
                printf("error in thread function!");
                return NULL;
            }
        }
    }
}

int serverRun(int statisticsPortNum, int queryPortNum, int bufferSize, int numThreads)
{
    int serverStatisticsDesc, clientStatisticsDesc, serverQueriesDesc, clientQueriesDesc;
    struct sockaddr_in serverStatisticsAddress, clientStatisticsAddress;
    struct sockaddr_in serverQueriesAddress, clientQueriesAddress;
    int addressSize = sizeof(struct sockaddr_in);
    fd_set readfds;
    pthread_t threadsArray[numThreads];
    pthreadArguments* parameters = malloc(sizeof(pthreadArguments));
    parameters->circularBuff = malloc(sizeof(circularBuffer));
    bufferInit(parameters->circularBuff);
    parameters->circularBuff->maxBufferSize = bufferSize;
    parameters->circularBuff->currentSize = 0;
    parameters->workersList = NULL;

    for (int i=0; i<numThreads; i++)
        pthread_create(&threadsArray[i], NULL, threadFunction, (void*)parameters);


    // get the port for statistics ready
    if ((serverStatisticsDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    serverStatisticsAddress.sin_family = AF_INET;
    serverStatisticsAddress.sin_addr.s_addr = INADDR_ANY;
    serverStatisticsAddress.sin_port = htons(statisticsPortNum);
    
    if (bind(serverStatisticsDesc, (struct sockaddr*)&serverStatisticsAddress, sizeof(serverStatisticsAddress)) == -1) {
        perror("bind failed");
        return -1;
    }
    if (listen(serverStatisticsDesc, QSIZE) == -1) {
        perror("listen failed");
        return -1;
    }

    // get the port for queries ready
    if ((serverQueriesDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    serverQueriesAddress.sin_family = AF_INET;
    serverQueriesAddress.sin_addr.s_addr = INADDR_ANY;
    serverQueriesAddress.sin_port = htons(queryPortNum);
    
    if (bind(serverQueriesDesc, (struct sockaddr*)&serverQueriesAddress, sizeof(serverQueriesAddress)) == -1) {
        perror("bind failed");
        return -1;
    }
    if (listen(serverQueriesDesc, QSIZE) == -1) {
        perror("listen failed");
        return -1;
    }

    int max = 0;
    if (serverStatisticsDesc > serverQueriesDesc)
        max = serverStatisticsDesc;
    else
        max = serverQueriesDesc;

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(serverQueriesDesc, &readfds);
        FD_SET(serverStatisticsDesc, &readfds);

        if (pselect(max + 1, &readfds, NULL, NULL, NULL, NULL) == -1) {
            perror("pselect failed!");
            return -1;
        }

        if (FD_ISSET(serverStatisticsDesc, &readfds)) {            
            if ((clientStatisticsDesc = accept(serverStatisticsDesc, (struct sockaddr*)&clientStatisticsAddress, (socklen_t*)&addressSize)) == -1) {
                perror("accept failed");
                return -1;
            }
            pthread_mutex_lock(&mutex);
            bufferInsert(parameters->circularBuff, clientStatisticsDesc);
            pthread_cond_signal(&conditionVariable);
            pthread_mutex_unlock(&mutex);
        }
        else if (FD_ISSET(serverQueriesDesc, &readfds)) {
            if ((clientQueriesDesc = accept(serverQueriesDesc, (struct sockaddr*)&clientQueriesAddress, (socklen_t*)&addressSize)) == -1) {
                perror("accept failed");
                return -1;
            }
            pthread_mutex_lock(&mutex);
            bufferInsert(parameters->circularBuff, clientQueriesDesc);
            pthread_cond_signal(&conditionVariable);
            pthread_mutex_unlock(&mutex);
        }
    }




    for (int i=0; i<numThreads; i++){
        if (pthread_join(threadsArray[i], NULL) != 0) {
            perror("pthread_join failed");
            return -1;
        }
    }

    return 0;
}
