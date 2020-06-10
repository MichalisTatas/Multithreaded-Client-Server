#include "../include/list.h"

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