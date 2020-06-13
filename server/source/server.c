#include "../include/server.h"
#include "../include/message.h"
#include "../include/list.h"

#define QSIZE 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t conditionVariable = PTHREAD_COND_INITIALIZER; 

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
            while(strcmp(msg, "finished!") != 0) {
                msg = msgComposer(descriptor, 20);
                printf("dsdsad %s \n", msg);
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
    int counter = 0;
    fd_set readfds;
    char* msg;
    pthread_t threadsArray[numThreads];
    pthreadArguments* parameters = malloc(sizeof(pthreadArguments));
    parameters->circularBuff = malloc(sizeof(circularBuffer));
    bufferInit(parameters->circularBuff);
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

    workerInfoPtr iterator;

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
        else {
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






    // addressSize = sizeof(struct sockaddr_in);
    // if ((clientQueriesDesc = accept(serverQueriesDesc, (struct sockaddr*)&clientQueriesAddress, (socklen_t*)&addressSize)) == -1) {
    //     perror("accept failed");
    //     return -1;
    // }




    // get all descriptors put them in list and then whiule (true) with pselect and its over
    // while (true) {
    //     printf("Waitinng\n");
    //     addressSize = sizeof(struct sockaddr_in);
    //     if ((clientStatisticsDesc = accept(serverStatisticsDesc, (struct sockaddr*)&clientStatisticsAddress, (socklen_t*)&addressSize)) == -1) {
    //         perror("accept failed");
    //         return -1;
    //     }
    //     counter++;
    //     workersList = addPortInList(workersList, clientStatisticsDesc);
    //     msg = msgComposer(clientStatisticsDesc, 20);
    //     if (counter == atoi(msg))
    //         break;
    // }

    // get the statistics

    // ADD COUNTRIES IN LIST???? SO CAN SEND THE QUERIES WHERE THEY SHOULD
    // na dinw kai to port twra wste na kserw se poio kai na thn bazw sto telos gt mporei na exist
    // apla na tis stelnw mia fora stin arxi poli kaliteri poliplokotita apo proigoumeno ofc 


    // workerInfoPtr iterator;
    // while (true) {
    //     FD_ZERO(&readfds);
    //     iterator = workersList;
    //     int max = 0;
    //     while (iterator != NULL) {
    //         FD_SET(iterator->port, &readfds);
    //         if (iterator->port > max)
    //             max = iterator->port;
    //         iterator = iterator->next;
    //     }

    //     if (pselect(max + 1, &readfds, NULL, NULL, NULL, NULL) == -1) {
    //         perror("pselect failed!");
    //         return -1;
    //     }

    //     iterator = workersList;
    //      while (iterator != NULL) {
    //         if (FD_ISSET(iterator->port, &readfds)) {
    //             if ((msg = msgComposer(iterator->port, bufferSize)) == NULL) {
    //                 perror("msgComposer failed");
    //                 return -1;
    //             }
                
    //             if (!strcmp(msg, "finished!")) {
    //                 iterator->readyForWork = true;
    //                 free(msg);
    //                 break;
    //             }

    //             printf("%s \n", msg);
    //             free(msg);
    //         }
    //         iterator = iterator->next;
    //     }
    //     iterator = workersList;
    //     bool allReady=true;
    //     while (iterator != NULL) {
    //         if (iterator->readyForWork == false)
    //             allReady = false;
    //         iterator = iterator->next;
    //     }
    //     if (allReady) {
    //         break;
    //     }
    // }

    

    // while (true) {
    //     FD_ZERO(&readfds);
    //     FD_SET(clientQueriesDesc, &readfds);

    //     if (pselect(clientQueriesDesc + 1, &readfds, NULL, NULL, NULL, NULL) == -1) {
    //             perror("pselect failed!");
    //             return -1;
    //     }
    //     if ((msg = msgComposer(clientQueriesDesc, 20)) == NULL) {
    //         perror("msgComposer failed");
    //         return -1;
    //     }

    //     iterator = workersList;
    //     while ( iterator!= NULL) {
    //         msgDecomposer(iterator->port, msg, 20);
    //         iterator=iterator->next;
    //     }

    // }
    return 0;
}
