#include "../include/list.h"

workerInfoPtr addPidInList(workerInfoPtr head, int pid)
{
    workerInfoPtr temp, curr;
    if ((temp = malloc(sizeof(workerInfo))) == NULL) {
        perror("malloc failed");
        return NULL;
    }

    temp->pid = pid;
    temp->write = -1;
    temp->readyForWork = false;
    temp->countriesList = NULL;
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

countryPtr sortedAddDateInList(countryPtr date, const char* dateString)
{
    countryPtr toBeInserted;
    if ((toBeInserted = malloc(sizeof(country))) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    if ((toBeInserted->name = malloc(strlen(dateString) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(toBeInserted->name, dateString);
    toBeInserted->next = NULL;
    // toBeInserted->statistics = NULL;

    if (date == NULL) {
        date = toBeInserted;
        return date;
    }
    else {
        countryPtr fastPtr, slowPtr;
        datePtr dateToBeInserted = createDate(dateString);
        fastPtr = date;
        datePtr fastDate;
        if (compareDates((fastDate = createDate(fastPtr->name)), dateToBeInserted) >= 0) {
            toBeInserted->next = date;
            date = toBeInserted;
            free(fastDate);
            free(dateToBeInserted);
            return date;
        }
        free(fastDate);

        while ((fastPtr!= NULL)) {
            if (compareDates((fastDate = createDate(fastPtr->name)), dateToBeInserted) >= 0) {
                free(fastDate);
                break;
            }
            free(fastDate);
            slowPtr = fastPtr;
            fastPtr = fastPtr->next;
        }
        free(dateToBeInserted);
        slowPtr->next = toBeInserted;
        toBeInserted->next = fastPtr;
    }
    return date;
}

bool existIn(patientPtr list, patientPtr patient)
{
    if (list == NULL)
        return false;

    patientPtr temp = list;
    while (temp!=NULL) {
        if (strcmp(temp->recordID, patient->recordID) == 0)
            return true;
        temp=temp->next;
    }
    return false;
}

void freeDataStatistics(statisticDataPtr head)
{
    if (head == NULL)
        return;
    freeDataStatistics(head->next);
    free(head->disease);
    free(head);
}

void freeDateStatistics(statisticsNodePtr head)
{
    if (head == NULL)
        return;
    freeDateStatistics(head->next);
    freeDataStatistics(head->data);
    free(head->dateString);
    free(head);
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