#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/client.h"

int main(int argc, char* argv[])
{
    int numThreads, servPort;
    char* queryFile;
    char* servIP;

    if (argc != 9 ) {
        printf("Wrong number of arguments exactly 9 is required! \n");
        return -1;
    }

    for (int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "-q")) {
            if ((queryFile = malloc(strlen(argv[i+1]) + 1)) == NULL) {
                printf("malloc failed !\n");
                return -1;
            }
            strcpy(queryFile, argv[i+1]);
        }
        else if (!strcmp(argv[i], "-w")) {
            if ((numThreads = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or bufferSize argument is 0 !\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-sp")) {
            if ((servPort = atoi(argv[i+1])) == 0) {
                printf("No valid conversion could be performed or bufferSize argument is 0 !\n");
                return -1;
            }
        }
        else if (!strcmp(argv[i], "-sip")) {
            if ((servIP = malloc(strlen(argv[i+1]) + 1)) == NULL) {
                printf("malloc failed !\n");
                return -1;
            }
            strcpy(servIP, argv[i+1]);
        }
    }

    if (clientRun(queryFile, numThreads, servPort, servIP) == -1) {
        perror("clientRun failed");
        return -1;
    }

    // free(queryFile);
    // free(servIP);
    return 0;
}