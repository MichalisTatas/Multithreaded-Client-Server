#include "../include/client.h"
#include "../include/queue.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutexPrint = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t conditionVariable = PTHREAD_COND_INITIALIZER; 

typedef struct arguments
{
    QueuePtr Q;
    int servPort;
    char* servIP;
} arguments;
typedef arguments* argumentsPtr;

char* msgComposer(int fileDescriptor, int bufferSize)
{
    char msgSizeArray[12] = {0};
    char* buffer[bufferSize];
    char* msg;


    for (int i=0; i<12/bufferSize; i++) {
        if (read(fileDescriptor, buffer, bufferSize) == -1) {
            perror("read failed");
            return NULL;
        }
        memcpy(msgSizeArray + i*bufferSize, buffer, bufferSize); //bufferSize?
    }

    if ((12 % bufferSize)) {
        if (read(fileDescriptor, buffer, 12 - bufferSize*(12 / bufferSize)) == -1) {
            perror("read failed");
            return NULL;
        }
        memcpy(msgSizeArray + bufferSize * (12 / bufferSize), buffer, 12 - bufferSize * (12 / bufferSize));
    }

    int msgSize = strtol(msgSizeArray, NULL, 10);
    if ((msg = malloc(msgSize)) == NULL) {      // +1 ???
        perror("malloc failed!");
        return NULL;
    }

    for (int i=0; i<msgSize/bufferSize; i++) {
        if (read(fileDescriptor, buffer, bufferSize) == -1) {
            perror("read failed");
            return NULL;
        }
        memcpy(msg + i*bufferSize, buffer, bufferSize);
    }

    if (msgSize % bufferSize) {
        if (read(fileDescriptor, buffer, msgSize - bufferSize*(msgSize / bufferSize) ) == -1) {
            perror("read failed");
            return NULL;
        }
        memcpy(msg + bufferSize*(msgSize / bufferSize), buffer, msgSize - bufferSize*(msgSize / bufferSize));
    }

    return msg;
}

int msgDecomposer(int fileDescriptor, char* msg, int bufferSize)
{
    int msgSize = strlen(msg)+1;
    char numBuff[12] = {0};
    sprintf(numBuff, "%d", msgSize);

    for (int i=0; i<12/bufferSize; i++) {
        if (write(fileDescriptor,  numBuff + i*bufferSize, bufferSize) == -1) {
            perror("write client failed");
            return -1;
        }
    }

    if (12 % bufferSize) {
        if (write(fileDescriptor, numBuff + (12 - 12 % bufferSize), 12 - bufferSize*(12 / bufferSize)) == -1) {
            perror("write client failed");
            return -1;
        }
    }


    for (int i=0; i<msgSize/bufferSize; i++) {
        if (write(fileDescriptor, msg + i*bufferSize , bufferSize) == -1) {
            perror("write client failed");
            return -1;
        }
    }

    if (msgSize % bufferSize) {
        if (write(fileDescriptor, msg + ( msgSize - msgSize % bufferSize) , msgSize - bufferSize*(msgSize / bufferSize)) == -1) {
            perror("write client failed");
            return -1;
        }
    }

    return 0;
}


void* threadFunction(void* argument)
{
    argumentsPtr arg = (argumentsPtr)argument;
    char* msg;
    char* answer;
    int sock;

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&conditionVariable, &mutex);
    pthread_mutex_unlock(&mutex);

    struct sockaddr_in serverAddress;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return NULL;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(arg->servPort);

    if (inet_pton(AF_INET, arg->servIP, &serverAddress.sin_addr) == -1) {
        perror("invalid address");
        return NULL;
    }

    if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("connect failed");
        return NULL;
    }

    // send a "c" so the server can recognize its from the client
    msgDecomposer(sock, "c", 20);

    while (true) {
        pthread_mutex_lock(&mutex);
        if ((msg = QRemove(arg->Q)) == NULL){
            msgDecomposer(sock, "finished!", 20);
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }
        pthread_mutex_unlock(&mutex);
        if (msg != NULL) {
            msgDecomposer(sock, msg, 20);
            answer = msgComposer(sock, 20);
            pthread_mutex_lock(&mutexPrint);
            printf("%s \n", msg);
            printf("%s \n", answer);
            pthread_mutex_unlock(&mutexPrint);
            // printf("prin");
            // while (!strcmp((msg = msgComposer(sock, 20)), "finished!"))
            //     printf("%s \n", msg);
            // printf("prin");
            
        }
    }
}

int clientRun(char* queryFile, int numThreads, int servPort, char* servIP)
{
    FILE* filePtr;
    if ((filePtr = fopen(queryFile, "r")) == NULL) {
        perror("fopen failed");
        return -1;
    }
    char* line = NULL;
    size_t len = 0;
    argumentsPtr arg = malloc(sizeof(arguments));
    arg->Q = malloc(sizeof(Queue));
    QInit(arg->Q);
    arg->servPort = servPort;
    arg->servIP = malloc(strlen(servIP) + 1);
    strcpy(arg->servIP, servIP);

    while(getline(&line, &len, filePtr) != -1) {
        line = strtok(line, "\n");
        QInsert(arg->Q, line);
    }

    pthread_t threadsArray[numThreads];
    for (int i=0; i<numThreads; i++)
        pthread_create(&threadsArray[i], NULL, threadFunction, (void*)arg);

    sleep(0.2);           // sleep to make sure all threads lock on the condition variable
    pthread_cond_broadcast(&conditionVariable);

    for (int i=0; i<numThreads; i++){
        if (pthread_join(threadsArray[i], NULL) != 0) {
            perror("pthread_join failed");
            return -1;
        }
    }
    fclose(filePtr);
    free(line);
    // free(Q);
    return 0;
}















