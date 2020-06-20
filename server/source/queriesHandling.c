#include "../include/queriesHandling.h"

void sendQuerie(workerInfoPtr workersList, const char* querie, int writeDesc, int bufferSize)
{
    char* temp = malloc(strlen(querie) + 1);
    strcpy(temp, querie);

    workerInfoPtr descriptorList = NULL;
    workerInfoPtr iterator = workersList;
    int sock;
    struct sockaddr_in serverAddress;

    if (iterator == NULL) {                       // if querie needs to be sent to one worker
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket failed");
            return;
        }

        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(writeDesc);

        if (inet_pton(AF_INET, "0.0.0.0", &serverAddress.sin_addr) == -1) {
            perror("invalid address");
            return;
        }

        if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            perror("connect failed");
            return;
        }
        msgDecomposer(sock, temp, bufferSize);
    }
    else {                                          // if querie needs to be sent to all workers
        while (iterator != NULL) {
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
                perror("socket failed");
                return;
            }

            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(iterator->port);

            if (inet_pton(AF_INET, "0.0.0.0", &serverAddress.sin_addr) == -1) {
                perror("invalid address");
                return;
            }

            if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
                perror("connect failed");
                return;
            }

            msgDecomposer(sock, temp, bufferSize); // iteratror port?>????
            // char*  t;
            // t=msgComposer(sock, 20);
            // printf("%s \n",t);
            descriptorList = addPortInList(descriptorList, sock);

            iterator->readyForWork = false;
            iterator = iterator->next;
        }
        querieAnswer(descriptorList, "/diseaseFrequency", -1, 20);
    }
    free(temp);
}

int querieAnswer(workerInfoPtr workersList, const char* querie, int readDesc, int bufferSize)
{
    sigset_t emptyset, blockset;
    fd_set readfds;
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGUSR1);
    sigaddset(&blockset, SIGINT);
    sigaddset(&blockset, SIGQUIT);
    sigprocmask(SIG_BLOCK, &blockset, NULL);
    char* msg;
    workerInfoPtr iterator = workersList;
    int diseaseFrequency = 0;

    if (workersList != NULL) {
        while(true) {
            FD_ZERO(&readfds);
            iterator = workersList;
            int max = 0;
            while (iterator != NULL) {
                FD_SET(iterator->port, &readfds);
                if (iterator->port > max)
                    max = iterator->port;
                iterator = iterator->next;
            }

            sigemptyset(&emptyset);
            if (pselect(max + 1, &readfds, NULL, NULL, NULL, &emptyset) == -1) {
                if (errno != EINTR) {
                    perror("pselect failed!");
                    return -1;
                }
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

                    if (!strcmp(querie, "/diseaseFrequency"))
                        diseaseFrequency =  diseaseFrequency + atoi(msg);
                    else
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
                if (!strcmp(querie, "/diseaseFrequency"))
                    printf("%d\n", diseaseFrequency);
                return 0;
            }
        }
    }
    else {
        while (true) {
            if ((msg = msgComposer(readDesc, bufferSize)) == NULL) {
                perror("msgComposer failed");
                return -1;
            }

            if (!strcmp(msg, "finished!")) {
                free(msg);
                return 0;
            }

            printf("%s \n", msg);
            free(msg);
        }
    }

    return -1;
}

int queriesHandler(workerInfoPtr workersList,const char* querie, int bufferSize)
{
    wordexp_t p;
    wordexp(querie, &p, 0);

    if (!strcmp(p.we_wordv[0], "/diseaseFrequency")) {
        if (p.we_wordc == 4) {
            sendQuerie(workersList, querie, -1, bufferSize);
            // querieAnswer(workersList, p.we_wordv[0], -1, bufferSize);
        }
        else if (p.we_wordc == 5) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[4]), bufferSize);
            // querieAnswer(NULL, p.we_wordv[0], selectWorker(workersList, ), bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/topk-AgeRanges")) {
        if (p.we_wordc == 6) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[2]), bufferSize);
            // querieAnswer(NULL, querie, descriptor, bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/searchPatientRecord")) {
        if (p.we_wordc == 2) {
            sendQuerie(workersList, querie, -1, bufferSize);
            // querieAnswer(workersList, querie, descriptor, bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientAdmissions")) {
        if (p.we_wordc == 4) {
            sendQuerie(workersList, querie, -1, bufferSize);
            // querieAnswer(workersList, querie, descriptor, bufferSize);
        }
        else if (p.we_wordc == 5) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[4]), bufferSize);
            // querieAnswer(NULL, querie, descriptor, bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/numPatientDischarges")) {
        if (p.we_wordc == 4) {
            sendQuerie(workersList, querie, -1, bufferSize);
            // querieAnswer(workersList, querie, descriptor, bufferSize);
        }
        else if (p.we_wordc == 5) {
            sendQuerie(NULL, querie, selectWorker(workersList, p.we_wordv[4]), bufferSize);
            // querieAnswer(NULL, querie, descriptor, bufferSize);
        }
        else {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
    }
    else if (!strcmp(p.we_wordv[0], "/exit")) {
        if (p.we_wordc != 1) {
            printf("number of arguments is not right! \n");
            wordfree(&p);
            return -1;
        }
        sendQuerie(workersList, querie, -1, bufferSize);
        // querieAnswer(workersList, querie, -1, bufferSize);
        wordfree(&p);
        return -1;
    }
    else {
        printf("wrong querie ! \n");
        wordfree(&p);
    }

    wordfree(&p);
    return 0;
}

int selectWorker(const workerInfoPtr workersList, char* country) 
{
    workerInfoPtr iterator = workersList;
    countryPtr countryIterator;
    while (iterator != NULL) {
        countryIterator = iterator->countriesList;
        while (countryIterator != NULL) {
            if (!strcmp(countryIterator->name, country)) {
                return iterator->port;
            }
            countryIterator = countryIterator->next;
        }
        iterator = iterator->next;
    }
    return -1;  //???
}