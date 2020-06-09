#include "../include/server.h"

#define QSIZE 2

int serverRun(int statisticsPortNum, int queryPortNum)
{
    // connection with master to get the statistics and save every desc to write queries to them brother

    int serverStatisticsDesc, clientStatisticsDesc;
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
    while (true) {
        printf("Waitinng\n");
        addressSize = sizeof(struct sockaddr_in);
        if ((clientStatisticsDesc = accept(serverStatisticsDesc, (struct sockaddr*)&clientAddress, (socklen_t*)&addressSize)) == -1) {
            perror("accept failed");
            return -1;
        }
        printf("Accepted the connection LOSER! \n ");
    }

    // need to connect to client its a start but  it connects continue when finished with statistics

    // if ((clientStatisticsDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    //     perror("socket failed");
    //     return -1;
    // }

    // serverAddress.sin_family = AF_INET;
    // serverAddress.sin_addr.s_addr = INADDR_ANY;
    // serverAddress.sin_port = htons(queryPortNum);
    
    // if (bind(clientStatisticsDesc, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
    //     perror("bind failed");
    //     return -1;
    // }
    // if (listen(clientStatisticsDesc, QSIZE) == -1) {
    //     perror("listen failed");
    //     return -1;
    // }

    // int addressSize;
    // while (true) {
    //     printf("Waitinng\n");
    //     addressSize = sizeof(struct sockaddr_in);
    //     if ((clientStatisticsDesc = accept(clientStatisticsDesc, (struct sockaddr*)&clientAddress, (socklen_t*)&addressSize)) == -1) {
    //         perror("accept failed");
    //         return -1;
    //     }
    //     printf("Accepted the connection LOSER! \n ");
    // }
    return 0;
}