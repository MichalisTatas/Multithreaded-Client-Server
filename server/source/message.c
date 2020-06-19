#include "../include/message.h"

int msgDecomposer(int fileDescriptor, char* msg, int bufferSize)
{
    int msgSize = strlen(msg)+1;
    char numBuff[12] = {0};
    sprintf(numBuff, "%d", msgSize);

    for (int i=0; i<12/bufferSize; i++) {
        if (write(fileDescriptor,  numBuff + i*bufferSize, bufferSize) == -1) {
            perror("write server failed");
            return -1;
        }
    }

    if (12 % bufferSize) {
        if (write(fileDescriptor, numBuff + (12 - 12 % bufferSize), 12 - bufferSize*(12 / bufferSize)) == -1) {
            perror("write server failed");
            return -1;
        }
    }


    for (int i=0; i<msgSize/bufferSize; i++) {
        if (write(fileDescriptor, msg + i*bufferSize , bufferSize) == -1) {
            perror("write server failed");
            return -1;
        }
    }

    if (msgSize % bufferSize) {
        if (write(fileDescriptor, msg + ( msgSize - msgSize % bufferSize) , msgSize - bufferSize*(msgSize / bufferSize)) == -1) {
            perror("write server failed");
            return -1;
        }
    }

    return 0;
}


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