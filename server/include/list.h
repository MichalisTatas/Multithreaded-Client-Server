#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct country {
    char* name;
    struct country* next;
} country;
typedef country* countryPtr;

typedef struct workerInfo
{
    int port;
    countryPtr countriesList;
    bool readyForWork;
    struct workerInfo* next;
} workerInfo;
typedef workerInfo* workerInfoPtr;

workerInfoPtr addPortInList(workerInfoPtr, int);
int addCountryInList(countryPtr*, char*);