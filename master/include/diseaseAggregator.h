#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/select.h>
#include <errno.h>

#include "pipes.h"
#include "workers.h"

int forkAssignFunctionality(int, int, char*, char*, char*);
int diseaseAggregator(workerInfoPtr, int, int, char*, char*, char*);
workerInfoPtr handleChildDeath(workerInfoPtr, int, char*, int);
void parentSIGINThandler(workerInfoPtr);
void parentSIGQUIThandler(workerInfoPtr);
int distributeCountries(workerInfoPtr*, int, int, char*);
bool areWorkersReady(workerInfoPtr*, int);
int countriesNumber(char*);