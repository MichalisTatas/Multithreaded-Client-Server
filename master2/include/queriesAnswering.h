#include <wordexp.h>
#include <sys/select.h>

#include "hashTable.h"
#include "list.h"
#include "pipes.h"
#include "patient.h"

int queriesAnswerer(const char*, int, HashTablePtr,countryPtr, patientPtr, int);
char* diseaseFrequency(HashTablePtr, char*, datePtr, datePtr, countryPtr);
char* topkAgeRanges(int, char*, char*, datePtr, datePtr, HashTablePtr);
char* searchPatientRecord(char*, patientPtr);
char* numPatientAdmissions(HashTablePtr, countryPtr, char*, datePtr, datePtr);
char* numPatientDischarges(HashTablePtr, countryPtr, char*, datePtr, datePtr);

// secondary function that are used for the prior ones
int diseaseFrequencyHelper(treeNodePtr, datePtr, datePtr, char*);
int topkAgeRangesHelper(treeNodePtr, datePtr, datePtr, char*, int, int);
char* resynthesizePatient(patientPtr);
int numPatientAdmissionsHelper(char*, datePtr, datePtr, treeNodePtr);
int numPatientDischargesHelper(char*, char*, datePtr, datePtr, treeNodePtr);