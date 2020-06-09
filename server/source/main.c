#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/server.h"

int main(int argc, char* argv[])
{
    int queryPortNum, statisticsPortNum, numThreads, bufferSize;

    if (argc != 9 ) {
        printf("Wrong number of arguments exactly 9 is required! \n");
        return -1;
    }

    for (int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "-q")) {
            if ((queryPortNum = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or numWorkers argument is 0 !\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-b")) {
            if ((bufferSize = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or bufferSize argument is 0 !\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-s")) {
            if ((statisticsPortNum = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or bufferSize argument is 0 !\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-w")) {
            if ((numThreads = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or bufferSize argument is 0 !\n");
                return -1;
            }
        }
    }

    if (serverRun(statisticsPortNum, queryPortNum) == -1) {
        perror("serverRun failed");
        return -1;
    }

    return 0;
}