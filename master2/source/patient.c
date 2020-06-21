#include "../include/patient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wordexp.h>

datePtr createDate(const char* line)
{
    char* temp;
    char* result;
    if ((temp = malloc(strlen(line) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(temp, line);

    datePtr dt;
    if ((dt = malloc(sizeof(date))) == NULL) {
        perror("malloc failed");
        return NULL;
    }

    result = strtok(temp, "-");
    dt->day = atoi(result);
    result = strtok(NULL, "-");
    dt->month = atoi(result);
    result = strtok(NULL, " ");
    dt->year = atoi(result);

    free(temp);
    return dt;
}

bool canInsertPatient(patientPtr list, patientPtr patient)
{
    if (list == NULL) {
        if (!strcmp(patient->action, "ENTER"))
            return true;
        return false;
    }

    patientPtr temp = list;
    while (temp!=NULL) {
        if (!strcmp(temp->recordID, patient->recordID)) {
            if (!strcmp(temp->action, "ENTER") && !strcmp(patient->action, "EXIT")) {
                // if (compareDates(temp->entryDate, patient->exitDate) == -1)
                    return true;
            }
            else
                return false; 
        }
        temp=temp->next;
    }

    if (!strcmp(patient->action, "ENTER"))
        return true;
    else 
        return false;
}

patientPtr replaceExitDate(patientPtr head, char* recordID, char* dt)
{
    datePtr date;
    
    date = createDate(dt);
    patientPtr temp = head;
    while (temp != NULL){
        if (!strcmp(temp->recordID, recordID)) {
            temp->exitDate = date;
            return head;
        }
        temp = temp->next;
    }
    return head;
}

int compareDates(datePtr date1, datePtr date2)
{
    if (date1->year > date2->year)
        return 1;
    else if (date1->year < date2->year)
        return -1;
    else {
        if (date1->month > date2->month)
            return 1;
        else if (date1->month < date2->month)
            return -1;
        else {
            if (date1->day > date2->day)
                return 1;
            else if (date1->day < date2->day)
                return -1;
        }
    }
    return 0;
}

patientPtr patientListInsert(patientPtr head, patientPtr current)
{
    if (head == NULL)
        return current;

    patientPtr temp = head;

    if (compareDates(head->entryDate, current->entryDate) == 1) {
        current->next = head;
        head = current;
    }
    else {
        while (temp->next != NULL) {
            if (compareDates(temp->next->entryDate, current->entryDate) == 1) {
                current->next = temp->next;
                temp->next = current;
                return head;
            }
            temp = temp->next;
        }
        if (temp->next == NULL)
            temp->next = current;

    }
    return head;
}

patientPtr createPatientStruct(const char* line, char* country, char* date)
{

    // FOR PATIENT WITH EXIT CREATE NEW FUNCITON SINCE ITS NOT TOGETHER LIKE PREVIOUS EXERCISE
    char* data = malloc(strlen(line) + 1);
    strcpy(data, line);
    wordexp_t p;
    wordexp(data, &p, 0);
    patientPtr current = malloc(sizeof(patient));

    if ((current->recordID = malloc(strlen(p.we_wordv[0]) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(current->recordID, p.we_wordv[0]);

    if ((current->action = malloc(strlen(p.we_wordv[1]) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(current->action, p.we_wordv[1]);

    if ((current->patientFirstName = malloc(strlen(p.we_wordv[2]) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(current->patientFirstName, p.we_wordv[2]);
    
    if ((current->patientLastName = malloc(strlen(p.we_wordv[3]) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(current->patientLastName, p.we_wordv[3]);
    
    if ((current->diseaseID = malloc(strlen(p.we_wordv[4]) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(current->diseaseID, p.we_wordv[4]);
    
    current->age = atoi(p.we_wordv[5]);

    if ((current->country = malloc(strlen(country) + 1)) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    strcpy(current->country, country);

    if ((current->entryDate = createDate(date)) == NULL) {
        perror("creating date failed");
        return NULL;
    }
    current->exitDate = NULL;

    current->next = NULL;
    wordfree(&p);
    free(data);
    return current;
}

void destroyPatientList(patientPtr patient)
{
    if (patient == NULL)
        return;
    destroyPatientList(patient->next);
    free(patient->recordID);
    free(patient->country);
    free(patient->diseaseID);
    free(patient->action);
    free(patient->entryDate);
    free(patient->exitDate);
    free(patient->patientFirstName);
    free(patient->patientLastName);
    free(patient);
}