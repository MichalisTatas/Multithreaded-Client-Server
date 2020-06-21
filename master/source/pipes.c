#include "../include/pipes.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int createPipe(char* path, int pid, int flags, char* ending)
{
    char buffer[12];
    if (sprintf(buffer, "%d", pid) <= 0) {
        perror("sprintf failed!");
        return -1;
    }

    char* pipeName;
    if ((pipeName = malloc(strlen(buffer) + strlen(path) + 4)) == NULL) {
        perror("malloc failed!");
        return -1;
    }

    strcpy(pipeName, path);
    strcat(pipeName, ending);
    strcat(pipeName, buffer);

    if (mkfifo(pipeName, 0666) == -1) {
        if (errno != EEXIST) {    
            perror("mkfifo failed!");
            return -1;
        }
    }

    int fileDesc;
    if ((fileDesc = open(pipeName, flags)) == -1) {
        perror("open failed!");
        return -1;
    }

    free(pipeName);
    return fileDesc;
}

int closePipe(char* path, int pid, char* ending)
{
    char buffer[12];
    if (sprintf(buffer, "%d", pid) <= 0) {
        perror("sprintf failed!");
        return -1;
    }

    char* pipeName;
    if ((pipeName = malloc(strlen(buffer) + strlen(path) + 4)) == NULL) {
        perror("malloc failed!");
        return -1;
    }

    strcpy(pipeName, path);
    strcat(pipeName, ending);
    strcat(pipeName, buffer);

    unlink(pipeName);
    free(pipeName);
    return 0;
}

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

