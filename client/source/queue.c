#include "../include/queue.h"

void QInit(QueuePtr Q)
{
    Q->head = NULL;
    Q->tail = NULL;
}

bool QEmpty(QueuePtr Q)
{
    if (Q->head == NULL)
        return true;
    return false;
}

void QInsert(QueuePtr Q, char* query)
{
    QueueNodePtr temp = malloc(sizeof(QueueNode));

    if (temp == NULL) {
        printf("system has no more space\n");
        return ;
    }

    temp->item = malloc(strlen(query) + 1);
    strcpy(temp->item, query);
    // temp->item =query;
    temp->next = NULL;

    if (Q->tail == NULL) {
        Q->head = temp;
        Q->tail = temp;
    }
    else {
        Q->tail->next = temp;
        Q->tail = temp;
    }
}

char* QRemove(QueuePtr Q)
{
    QueueNodePtr temp;

    if(Q->head == NULL) {
        return NULL;
    }
    else {
        temp = Q->head;
        Q->head = temp->next;
        if (Q->head == NULL)
            Q->tail = NULL;
        char* query = malloc(strlen(temp->item) + 1);
        strcpy(query, temp->item);
        free(temp->item);
        free(temp);
        return query;
    }
}

void QDestroy(QueuePtr Q)
{
    QueueNodePtr temp;
    while (Q->head != NULL) {
        temp = Q->head;
        Q->head = Q->head->next;
        free(temp);
    }
    free(Q);
}