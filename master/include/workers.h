#include "pipes.h"
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "list.h"
#include "hashTable.h"
#include "queriesAnswering.h"


int workersFunction(int, char*);
countryPtr setDataStructures(HashTablePtr*, countryPtr, patientPtr*, char*, int, int);
countryPtr setNewDataStructures(HashTablePtr*, countryPtr, countryPtr, patientPtr*,char*);
void updateStatistics(countryPtr, patientPtr, char*, char*);
int createLogFile(int, countryPtr, int);
void freeDataStructures(HashTablePtr*, countryPtr*, countryPtr*, patientPtr*);


statisticDataPtr addDataStatistics(statisticDataPtr dataStatisticsNode, patientPtr currentPatient);
int sendStatistics(statisticDataPtr head, char* country, char* date, int, int);
