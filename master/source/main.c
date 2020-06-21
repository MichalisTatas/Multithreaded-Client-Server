#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/diseaseAggregator.h"

int main(int argc, char* argv[])
{
    unsigned int numWorkers, bufferSize;
    char* input_dir;
    char* serverIP;
    char* serverPort;

    if (argc != 11 ) {
        printf("Wrong number of arguments exactly 11 is required! \n");
        return -1;
    }

    for (int i=0; i<argc; i++) {
        if (!strcmp(argv[i], "-w")) {
            if ((numWorkers = atoi(argv[i+1])) == 0) {
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
        else if (!strcmp(argv[i], "-i")) {
            if ((input_dir = malloc(strlen(argv[i+1]) + 1)) == NULL) {
                perror("malloc failed");
                return -1;
            }
            strcpy(input_dir, argv[i+1]);
        }
        else if (!strcmp(argv[i], "-s")) {
            if ((serverIP = malloc(strlen(argv[i+1]) + 1)) == NULL) {
                perror("malloc failed");
                return -1;
            }
            strcpy(serverIP, argv[i+1]);
        }
        else if (!strcmp(argv[i], "-p")) {
            if ((serverPort = malloc(strlen(argv[i+1]) + 1)) == NULL) {
                perror("malloc failed");
                return -1;
            }
            strcpy(serverPort, argv[i+1]);
        }
    }

    if (forkAssignFunctionality(bufferSize, numWorkers, input_dir, serverIP, serverPort) == -1) {
        printf("Error occured in diseaseAggregatorFunction!\n");
        return -1;
    }

    free(serverPort);
    free(serverIP);
    return 0;
}