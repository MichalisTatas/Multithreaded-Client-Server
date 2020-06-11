#include "../include/server.h"
#include "../include/message.h"
#include "../include/list.h"

#define QSIZE 2

int serverRun(int statisticsPortNum, int queryPortNum, int bufferSize)
{
    // connection with master to get the statistics and save every desc to write queries to them brother
    workerInfoPtr workersList = NULL;
    int counter = 0;
    int serverStatisticsDesc, clientStatisticsDesc, serverQueriesDesc, clientQueriesDesc;
    struct sockaddr_in serverAddress, clientAddress;
    if ((serverStatisticsDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(statisticsPortNum);
    
    if (bind(serverStatisticsDesc, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind failed");
        return -1;
    }
    if (listen(serverStatisticsDesc, QSIZE) == -1) {
        perror("listen failed");
        return -1;
    }

    int addressSize;
    char* msg;

    // get all descriptors put them in list and then whiule (true) with pselect and its over
    while (true) {
        printf("Waitinng\n");
        addressSize = sizeof(struct sockaddr_in);
        if ((clientStatisticsDesc = accept(serverStatisticsDesc, (struct sockaddr*)&clientAddress, (socklen_t*)&addressSize)) == -1) {
            perror("accept failed");
            return -1;
        }
        counter++;
        workersList = addPortInList(workersList, clientStatisticsDesc);
        msg = msgComposer(clientStatisticsDesc, 20);
        if (counter == atoi(msg))
            break;
    }

    // get the statistics

    // ADD COUNTRIES IN LIST???? SO CAN SEND THE QUERIES WHERE THEY SHOULD
    // na dinw kai to port twra wste na kserw se poio kai na thn bazw sto telos gt mporei na exist
    // apla na tis stelnw mia fora stin arxi poli kaliteri poliplokotita apo proigoumeno ofc 

    fd_set readfds;
    workerInfoPtr iterator;
    while (true) {
        FD_ZERO(&readfds);
        iterator = workersList;
        int max = 0;
        while (iterator != NULL) {
            FD_SET(iterator->port, &readfds);
            if (iterator->port > max)
                max = iterator->port;
            iterator = iterator->next;
        }

        if (pselect(max + 1, &readfds, NULL, NULL, NULL, NULL) == -1) {
            perror("pselect failed!");
            return -1;
        }

        iterator = workersList;
         while (iterator != NULL) {
            if (FD_ISSET(iterator->port, &readfds)) {
                if ((msg = msgComposer(iterator->port, bufferSize)) == NULL) {
                    perror("msgComposer failed");
                    return -1;
                }
                
                if (!strcmp(msg, "finished!")) {
                    iterator->readyForWork = true;
                    free(msg);
                    break;
                }

                printf("%s \n", msg);
                free(msg);
            }
            iterator = iterator->next;
        }
        iterator = workersList;
        bool allReady=true;
        while (iterator != NULL) {
            if (iterator->readyForWork == false)
                allReady = false;
            iterator = iterator->next;
        }
        if (allReady) {
            break;
        }
    }

    // printf("BRAKE \n\n\n");

    // iterator = workersList;
    // while (iterator != NULL) {
    //     msgDecomposer(iterator->port, "LEL", 20);
    //     iterator = iterator->next;
    // }

    // HANDLE CLIENT CONNECTIONS

    // need to connect to client its a start but  it connects continue when finished with statistics

    if ((serverQueriesDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(queryPortNum);
    
    if (bind(serverQueriesDesc, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("bind failed");
        return -1;
    }
    if (listen(serverQueriesDesc, QSIZE) == -1) {
        perror("listen failed");
        return -1;
    }

    addressSize = sizeof(struct sockaddr_in);
    if ((clientQueriesDesc = accept(serverQueriesDesc, (struct sockaddr*)&clientAddress, (socklen_t*)&addressSize)) == -1) {
        perror("accept failed");
        return -1;
    }
    printf("Accepted the connection LOSER! \n ");

    sigset_t emptyset;
    while (true) {
        // wait for queries LOL
        FD_ZERO(&readfds);
        FD_SET(clientQueriesDesc, &readfds);

        sigemptyset(&emptyset);
        if (pselect(clientQueriesDesc + 1, &readfds, NULL, NULL, NULL, &emptyset) == -1) {
                perror("pselect failed!");
                return -1;
        }
        if ((msg = msgComposer(clientQueriesDesc, bufferSize)) == NULL) {
            perror("msgComposer failed");
            return -1;
        }
        printf("client msg : %s \n", msg);

    }
    return 0;
}