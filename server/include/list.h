#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#pragma once

typedef struct QueueNode
{
    int fileDesc;
    struct QueueNode* next;
} QueueNode;
typedef QueueNode* QueueNodePtr;

typedef struct queue
{
    int maxBufferSize;
    int currentSize;
    QueueNodePtr head;
    QueueNodePtr tail;
} circularBuffer;
typedef circularBuffer* circularBufferPtr;


typedef struct country {
    char* name;
    struct country* next;
} country;
typedef country* countryPtr;

typedef struct workerInfo
{
    int port;
    countryPtr countriesList;
    char* ipAddress;
    bool readyForWork;
    struct workerInfo* next;
} workerInfo;
typedef workerInfo* workerInfoPtr;

typedef struct pthreadArguments 
{
    workerInfoPtr workersList;
    circularBufferPtr circularBuff;
} pthreadArguments;

workerInfoPtr addPortInList(workerInfoPtr, int);
int addCountryInList(countryPtr*, char*);
bool bufferEmpty(circularBufferPtr);
void bufferDestroy(circularBufferPtr);
void bufferInit(circularBufferPtr);
void bufferInsert(circularBufferPtr, int);
int bufferRemove(circularBufferPtr);