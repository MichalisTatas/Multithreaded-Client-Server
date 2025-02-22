#include "../include/list.h"
#include "../include/message.h"

workerInfoPtr addPortInList(workerInfoPtr head, int portNum)
{
    workerInfoPtr temp, curr;
    if ((temp = malloc(sizeof(workerInfo))) == NULL) {
        perror("malloc failed");
        return NULL;
    }

    temp->port = portNum;
    temp->countriesList = NULL;
    temp->readyForWork = false;
    temp->ipAddress = NULL;
    temp->next = NULL;
    
    if (head == NULL)
        return temp;

    curr = head;
    while (curr->next != NULL)
        curr = curr->next;
    
    curr->next = temp;
    return head;
}

int addCountryInList(countryPtr* head, char* countryName)
{
    countryPtr cntry;
    if ((cntry = malloc(sizeof(country))) == NULL) {
        perror("malloc failed");
        return -1;
    }
    if ((cntry->name = malloc(strlen(countryName) + 1)) == NULL) {
        perror("malloc failed!");
        return -1;
    }
    strcpy(cntry->name, countryName);

    cntry->next = *head;
    *head = cntry;
    return 0;
}

void bufferInit(circularBufferPtr Q)
{
    Q->head = NULL;
    Q->tail = NULL;
}

bool bufferEmpty(circularBufferPtr Q)
{
    if (Q->head == NULL)
        return true;
    return false;
}

void bufferInsert(circularBufferPtr Q, int desc)
{
    QueueNodePtr temp = malloc(sizeof(QueueNode));
    if (Q->currentSize >= Q->maxBufferSize) {
        printf("buffer full\n");
        msgDecomposer(desc, "buffer is full query not answered!", 20);
        return;
    }

    Q->currentSize++;
    if (temp == NULL) {
        printf("system has no more space\n");
        return ;
    }

    temp->fileDesc = desc;

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

int bufferRemove(circularBufferPtr Q)
{
    QueueNodePtr temp;

    if(Q->head == NULL) {
        return -1;
    }
    else {
        Q->currentSize--;
        temp = Q->head;
        Q->head = temp->next;
        if (Q->head == NULL)
            Q->tail = NULL;
        int desc = temp->fileDesc;
        free(temp);
        return desc;
    }
}

void bufferDestroy(circularBufferPtr Q)
{
    QueueNodePtr temp;
    while (Q->head != NULL) {
        temp = Q->head;
        Q->head = Q->head->next;
        free(temp);
    }
    free(Q);
}

void destroyCountryList(countryPtr head)
{
    if (head == NULL)
        return;
    destroyCountryList(head->next);
    // if (head->statistics != NULL)
    //     freeDateStatistics(head->statistics);
    free(head->name);
    free(head);
}

void destroyList(workerInfoPtr head)
{
    if (head == NULL)
        return;
    destroyList(head->next);
    destroyCountryList(head->countriesList);
    free(head);
}