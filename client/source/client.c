#include "../include/client.h"

int msgDecomposer(int fileDescriptor, char* msg, int bufferSize)
{
    int msgSize = strlen(msg)+1;
    char numBuff[12] = {0};
    sprintf(numBuff, "%d", msgSize);

    for (int i=0; i<12/bufferSize; i++) {
        if (write(fileDescriptor,  numBuff + i*bufferSize, bufferSize) == -1) {
            perror("write failed");
            return -1;
        }
    }

    if (12 % bufferSize) {
        if (write(fileDescriptor, numBuff + (12 - 12 % bufferSize), 12 - bufferSize*(12 / bufferSize)) == -1) {
            perror("write failed");
            return -1;
        }
    }


    for (int i=0; i<msgSize/bufferSize; i++) {
        if (write(fileDescriptor, msg + i*bufferSize , bufferSize) == -1) {
            perror("write failed");
            return -1;
        }
    }

    if (msgSize % bufferSize) {
        if (write(fileDescriptor, msg + ( msgSize - msgSize % bufferSize) , msgSize - bufferSize*(msgSize / bufferSize)) == -1) {
            perror("write failed");
            return -1;
        }
    }

    return 0;
}

int clientRun(char* queryFile, int numThreads, int servPort, char* servIP)
{
    int sock;
    struct sockaddr_in serverAddress;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(servPort);

    if (inet_pton(AF_INET, servIP, &serverAddress.sin_addr) == -1) {
        perror("invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("connect failed");
        return -1;
    }

    if (msgDecomposer(sock, "geika eima ito query", 20) == -1) {
        printf("msgDecomposer failed\n");
        return -1;
    }

    return 0;
}