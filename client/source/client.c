#include "../include/client.h"

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

    return 0;
}