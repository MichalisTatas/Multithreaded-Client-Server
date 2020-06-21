#pragma once
#include "patient.h"
#include "avlTree.h"

typedef struct ItemNode
{
    char* key;
    treeNodePtr tree;
} ItemNode;
typedef ItemNode* ItemNodePtr;

typedef struct HTNode
{
    struct HTNode* next;
    char* key;
    treeNodePtr tree;
    // ItemNodePtr* array;
} HTNode;
typedef HTNode* HTNodePtr;

typedef struct HashTable
{
    int size;
    HTNodePtr* table;
} HashTable;
typedef HashTable* HashTablePtr;

int findTree(HTNodePtr, char*, int);
void HTPrint(HashTablePtr);
HashTablePtr HTCreate(int);
int HTInsert(HashTablePtr, char*, patientPtr);
int hashFunction(char*);
void HTDestroy(HashTablePtr);