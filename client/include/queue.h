#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct QueueNode
{
    char* item;
    struct QueueNode* next;
} QueueNode;
typedef QueueNode* QueueNodePtr;

typedef struct Queue
{
    QueueNodePtr head;
    QueueNodePtr tail;
} Queue;
typedef Queue* QueuePtr;

bool QEmpty(QueuePtr);
void QDestroy(QueuePtr);
void QInit(QueuePtr);
void QInsert(QueuePtr, char*);
char* QRemove(QueuePtr);