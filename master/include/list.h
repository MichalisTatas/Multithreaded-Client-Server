#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#pragma once

#include "patient.h"

typedef struct statisticData{
    char* disease;
    int counter0_20;
    int counter21_40;
    int counter41_60;
    int counter60andOver;
    struct statisticData* next;
} statisticData;
typedef statisticData* statisticDataPtr;

typedef struct statisticsNode {
    char* dateString;
    statisticDataPtr data; 
    struct statisticsNode* next;
} statisticsNode;
typedef statisticsNode* statisticsNodePtr;

typedef struct country {
    char* name;
    // statisticsNodePtr statistics;
    struct country* next;
} country;
typedef country* countryPtr;

typedef struct workerInfo
{
    int pid;
    int write;
    bool readyForWork;
    countryPtr countriesList;
    struct workerInfo* next;
} workerInfo;

typedef workerInfo* workerInfoPtr;

workerInfoPtr addPidInList(workerInfoPtr, int);
int addCountryInList(countryPtr*, char*);
countryPtr sortedAddDateInList(countryPtr, const char*);
countryPtr addDateAndSort(countryPtr, char*);
bool existIn(patientPtr, patientPtr);
void destroyCountryList(countryPtr);
void destroyList(workerInfoPtr);
void freeDataStatistics(statisticDataPtr);