#include "../include/workers.h"

#define QSIZE 100

volatile sig_atomic_t signalHandlerNumber;

static void handler(int sig)
{
    signalHandlerNumber = sig;
}

int workersFunction(int bufferSize, char* inputDirectory, int workersNum)
{
    char* serverIP;
    char* serverPort;
    int queriesAnswered = 0;
    int errorsRaised = 0;
    int pid = getpid();
    int readDesc;
    countryPtr countryList = NULL;
    countryPtr completeDateList = NULL;
    char* msg;
    sigset_t emptyset, blockset;
    fd_set readfds;
    signal(SIGUSR1, handler);
    signal(SIGINT, handler);
    signal(SIGQUIT, handler);
    sigemptyset(&blockset);
    sigaddset(&blockset, SIGUSR1);
    sigaddset(&blockset, SIGINT);
    sigaddset(&blockset, SIGQUIT);
    sigprocmask(SIG_BLOCK, &blockset, NULL);

    if ((readDesc = createPipe("pipes/", pid, O_RDONLY,"P2C")) == -1) {  //P2C parent writes to child
        errorsRaised++;
        perror("createPipe failed");
        return -1;
    }


    while(true) {
        if ((msg = msgComposer(readDesc, bufferSize)) == NULL) {
            errorsRaised++;
            perror("msgComposer failed");
            return -1;
        }

        if (!strcmp(msg, "finished writing countries")) {
            free(msg);
            break;
        }
        else {
            if (addCountryInList(&countryList, msg) == -1) {
                errorsRaised++;
                perror("addCountryInList failed");
                return -1;
            }
            free(msg);
        }
    }

    // get serverIP
    if ((msg = msgComposer(readDesc, bufferSize)) == NULL) {
        errorsRaised++;
        perror("msgComposer failed");
        return -1;
    }
    if ((serverIP = malloc(strlen(msg)+1)) == NULL) {
        errorsRaised++;
        perror("msgComposer failed");
        return -1;
    }
    strcpy(serverIP, msg);
    free(msg);

    // get serverPort
    if ((msg = msgComposer(readDesc, bufferSize)) == NULL) {
        errorsRaised++;
        perror("msgComposer failed");
        return -1;
    }
    if ((serverPort = malloc(strlen(msg)+1)) == NULL) {
        errorsRaised++;
        perror("msgComposer failed");
        return -1;
    }
    strcpy(serverPort, msg);
    free(msg);

    HashTablePtr diseaseHashtable;
    patientPtr patientListHead = NULL;
    if ((diseaseHashtable = HTCreate(200)) == NULL) {
        errorsRaised++;
        perror("HTCreate failed");
        return -1;
    }

    int sock;
    struct sockaddr_in serverAddress;
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(serverPort));

    if (inet_pton(AF_INET, serverIP, &serverAddress.sin_addr) == -1) {
        perror("invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("connect failed");
        return -1;
    }

    // send a "w" so the server can recognize its from the worker
    if (msgDecomposer(sock, "w", 20) == -1) {
        printf("msgDecomposer failed\n");
        return -1;
    }

    int serverQueriesDesc, clientStatisticsDesc;
    struct sockaddr_in serverQueriesAddress,clientStatisticsAddress;

    if ((serverQueriesDesc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    serverQueriesAddress.sin_family = AF_INET;
    serverQueriesAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverQueriesAddress.sin_port = htons(0);
    
    if (bind(serverQueriesDesc, (struct sockaddr*)&serverQueriesAddress, sizeof(struct sockaddr_in)) == -1) {
        perror("bind failed");
        return -1;
    }
    if (listen(serverQueriesDesc, QSIZE) == -1) {
        perror("listen failed");
        return -1;
    }

    struct sockaddr_in sin;
    socklen_t s = sizeof(struct sockaddr_in);
    getsockname(serverQueriesDesc, (struct sockaddr*)&sin, &s);


    char buff[12];
    sprintf(buff, "%d", ntohs(sin.sin_port));
    if (msgDecomposer(sock, buff, 20) == -1) {
        printf("msgDecomposer failed\n");
        return -1;
    }

    // send ip address
    // char* m = inet_ntoa(sin.sin_addr);
    // if (msgDecomposer(sock, m, 20) == -1) {
    //     printf("msgDecomposer failed\n");
    //     return -1;
    // }

    countryPtr cntry = countryList;
    while (cntry != NULL) {
        if (msgDecomposer(sock, cntry->name, 20) == -1) {
            printf("msgDecomposer failed\n");
            return -1;
        }
        cntry = cntry->next;
    }
    if (msgDecomposer(sock, "finished writing countries!", 20) == -1) {
        printf("msgDecomposer failed\n");
        return -1;
    }


    // make it send statistics to server not to parent
    if ((completeDateList = setDataStructures(&diseaseHashtable, countryList, &patientListHead, inputDirectory, sock, bufferSize)) == NULL) {
        errorsRaised++;
        perror("setDataStructures");
        return -1;
    }

    //finished filling data structures ready for queries
    if (msgDecomposer(sock, "finished!", bufferSize) == -1) {
        errorsRaised++;
        perror("msgDecomposer failed");
        return -1;
    }

    int addressSize = sizeof(struct sockaddr_in);
    while(true) {
        
        FD_ZERO(&readfds);
        FD_SET(serverQueriesDesc, &readfds);

        sigemptyset(&emptyset);
        if (pselect(serverQueriesDesc + 1, &readfds, NULL, NULL, NULL, &emptyset) == -1) {
            if (errno == EINTR){
                if (signalHandlerNumber == SIGINT) {
                    if (createLogFile(queriesAnswered, countryList, errorsRaised) == -1) {
                        errorsRaised++;
                        perror("createLogFile failed");
                        return -1;
                    }
                    // free(s);
                    free(serverIP);
                    free(serverPort);
                    freeDataStructures(&diseaseHashtable, &countryList, &completeDateList, &patientListHead);
                    kill(getppid(), SIGCHLD);
                    return 0;
                }
                if (signalHandlerNumber == SIGQUIT) {
                    if (createLogFile(queriesAnswered, countryList, errorsRaised) == -1) {
                        errorsRaised++;
                        perror("createLogFile failed");
                        return -1;
                    }
                    free(serverIP);
                    free(serverPort);
                    freeDataStructures(&diseaseHashtable, &countryList, &completeDateList, &patientListHead);
                    kill(getppid(), SIGCHLD);
                    return 0;
                }
            }
            if (errno != EINTR) {
                errorsRaised++;
                perror("pselect failed!");
                return -1;
            }
        }

        if ((clientStatisticsDesc = accept(serverQueriesDesc, (struct sockaddr*)&clientStatisticsAddress, (socklen_t*)&addressSize)) == -1) {
            perror("accept failed");
            return -1;
        }

        if ((msg = msgComposer(clientStatisticsDesc, bufferSize)) == NULL) {
            errorsRaised++;
            perror("msgComposer failed");
            return -1;
        }

        queriesAnswered++;

        if (queriesAnswerer(msg, bufferSize, diseaseHashtable, countryList, patientListHead, clientStatisticsDesc) == 1)
            break;

        free(msg);
    }


    free(serverIP);
    free(serverPort);
    free(msg);
    freeDataStructures(&diseaseHashtable, &countryList, &completeDateList, &patientListHead);
    return 0;         // return here and exit in forkassignFunctionality to free remaining memory
}

countryPtr setDataStructures(HashTablePtr* diseaseHashtable, countryPtr countryList, patientPtr* patientListHead,  char* inputDirectory, int writeDesc, int bufferSize){
    // open each country file and fill the needed data structures
    DIR* countryDir;
    struct dirent* d;
    countryPtr country = countryList;
    countryPtr completeDateList = NULL;
    statisticDataPtr dataStatistics = NULL;
    FILE* filePtr;
    char* countryPath;
    char* datePath;
    char* line = NULL;
    size_t len = 0;
    patientPtr currentPatient = NULL;

    while (country != NULL) {

        countryPath = malloc(strlen(inputDirectory) + strlen(country->name) + 1);
        strcpy(countryPath, inputDirectory);
        strcat(countryPath, country->name);

        if ((countryDir = opendir(countryPath)) == NULL) {
            perror("opendir failed");
            return NULL;
        }

        countryPtr dateList = NULL;
        while ((d = readdir(countryDir)) != NULL) {
            if ( !strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") )
                continue;
            if ((dateList = sortedAddDateInList(dateList, d->d_name)) == NULL) {
                perror("sortedAddDateInList failed");
                return NULL;
            }
            if ((completeDateList = sortedAddDateInList(completeDateList, d->d_name)) == NULL) {
                perror("sortedAddDateInList failed");
                return NULL;
            }
        }

        countryPtr date = dateList;
        while (date != NULL) {
            dataStatistics = NULL;
            datePath = malloc(strlen(countryPath) + strlen(date->name) + 2);
            strcpy(datePath, countryPath);
            strcat(datePath, "/");       //        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            strcat(datePath, date->name);
            if ((filePtr = fopen(datePath, "r")) == NULL)   {
                perror("fopen failed");
                return NULL;
            }

            while (getline(&line, &len, filePtr) != -1) {
                line = strtok(line, "\n");
                if ((currentPatient = createPatientStruct(line, country->name, date->name)) == NULL) {
                    perror("createPatientStruct failed");
                    return NULL;
                }
                if (canInsertPatient(*patientListHead, currentPatient)) {
                    if(!strcmp(currentPatient->action, "ENTER")) {

                        //insert
                        *patientListHead = patientListInsert(*patientListHead, currentPatient);
                        if (HTInsert(*diseaseHashtable, currentPatient->diseaseID, currentPatient) == -1) {
                            perror("HTInsert failed");
                            return NULL;
                        }

                        // update statistics
                        dataStatistics = addDataStatistics(dataStatistics, currentPatient);
                    }
                    else {
                        *patientListHead = replaceExitDate(*patientListHead, currentPatient->recordID, date->name);
                        destroyPatientList(currentPatient);
                    }
                    
                }
                else
                    destroyPatientList(currentPatient);
            }

            if (fclose(filePtr) == EOF) {
                perror("fclose failed");
                return NULL;
            }

            sendStatistics(dataStatistics, country->name, date->name, writeDesc, bufferSize);
            freeDataStatistics(dataStatistics);
            free(datePath);
            date = date->next;
        }

        if (closedir(countryDir) == -1) {
            perror("closedir failed!");
            return NULL;
        }

        destroyCountryList(dateList);

        free(countryPath);
        country = country->next;
    }

    free(line);
    return completeDateList;
}

bool existInCompleteDateList(countryPtr completeDateList, char* name)
{
    countryPtr temp = completeDateList;
    while (temp != NULL) {
        if (!strcmp(temp->name, name))
            return true;
        temp = temp->next;
    }
    return false;
}

statisticDataPtr addDataStatistics(statisticDataPtr dataStatisticsNode, patientPtr currentPatient)
{
    if (dataStatisticsNode != NULL) {
        statisticDataPtr curr = dataStatisticsNode;
        while (curr != NULL) {
            if (!strcmp(curr->disease, currentPatient->diseaseID)) {
                if (currentPatient->age < 20)
                    curr->counter0_20++;
                else if (currentPatient->age < 40)
                    curr->counter21_40++;
                else if (currentPatient->age < 60)
                    curr->counter41_60++;
                else
                    curr->counter60andOver++;
                return dataStatisticsNode;
            }
            curr = curr->next;
        }
    }

    statisticDataPtr temp = malloc(sizeof(statisticData));
    temp->disease = malloc(strlen(currentPatient->diseaseID) + 1);
    strcpy(temp->disease, currentPatient->diseaseID);
    temp->counter0_20 = 0;
    temp->counter21_40 = 0;
    temp->counter41_60 = 0;
    temp->counter60andOver = 0;
    temp->next = NULL;
    if (currentPatient->age < 20)
        temp->counter0_20++;
    else if (currentPatient->age < 40)
        temp->counter21_40++;
    else if (currentPatient->age < 60)
        temp->counter41_60++;
    else
        temp->counter60andOver++;
    if (dataStatisticsNode == NULL)
        return temp;
    else {
        temp->next = dataStatisticsNode;
        dataStatisticsNode = temp;
        return dataStatisticsNode;
    }
}

int sendStatistics(statisticDataPtr head, char* country, char* date, int writeDesc, int bufferSize)
{
    statisticDataPtr temp = head;
    while (temp!= NULL) {
        char* buffer = malloc(strlen(date) + strlen(country) + strlen(temp->disease) + 200); //135??
        sprintf(buffer, "%s\n%s\n%s\nAge range 0-20 years: %d cases\nAge range 21-40 years: %d cases\nAge range 41-60 years: %d cases\nAge range 60+ years: %d cases\n", date, country, temp->disease, temp->counter0_20, temp->counter21_40, temp->counter41_60, temp->counter60andOver);
        msgDecomposer(writeDesc, buffer, bufferSize);
        free(buffer);
        temp = temp->next;
    }
    return 0;

}

int createLogFile(int queriesAnswered, countryPtr countryList, int errorsRaised)
{
    FILE* filePtr;
    char* path = malloc(strlen("log_files/log_file.%d") + sizeof(getpid()) + 1);
    sprintf(path, "log_files/log_file.%d", getpid());
    if ((filePtr = fopen(path, "w")) == NULL) {
        perror("open file failed");
        return -1;
    }
    countryPtr country = countryList;
    fputs("requests received : ", filePtr);
    char buffer[12] = {0};
    sprintf(buffer, "%d \n", queriesAnswered);
    fputs(buffer, filePtr);

    fputs("errors raised : ", filePtr);
    sprintf(buffer, "%d \n", errorsRaised);
    fputs(buffer, filePtr);
    while (country != NULL) {
        fputs(country->name, filePtr);
        fputs("\n", filePtr);
        country=country->next;
    }
    fclose(filePtr);
    free(path);
    return 0;
}

void freeDataStructures(HashTablePtr* diseaseHashtable,countryPtr* countryList, countryPtr* completeDateList, patientPtr* patientListHead)
{
    destroyPatientList(*patientListHead);
    HTDestroy(*diseaseHashtable);
    destroyCountryList(*countryList);
    destroyCountryList(*completeDateList);
}