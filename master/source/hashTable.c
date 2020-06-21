#include "../include/hashTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>





// REHASH








HashTablePtr HTCreate(int size)
{
    HashTablePtr ht;
    if ((ht = malloc(sizeof(HashTable))) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    if ((ht->table = malloc(size*sizeof(HTNode))) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    ht->size = size;
    for (int i=0; i<size; i++)
        ht->table[i] = NULL;
    
    return ht;
}

int hashFunction(char* str)
{
    return (*str)/10;
}

HTNodePtr bucketInit(HTNodePtr node)
{
    if ((node = malloc(sizeof(HTNode))) == NULL) {
        perror("malloc failed");
        return NULL;
    }
    node->next = NULL;
    return node;
}

int HTInsert(HashTablePtr hashTable, char* key, patientPtr patient)
{
    int bucketNum = hashFunction(key);
    HTNodePtr temp = NULL;

    // if bucket is empty
    if (hashTable->table[bucketNum] == NULL) {
        if ((hashTable->table[bucketNum] = bucketInit(hashTable->table[bucketNum])) == NULL) {
            perror("bucketInit failed");
            return -1;
        }
        if ((hashTable->table[bucketNum]->key = malloc(strlen(key) + 1)) == NULL) {
            perror("malloc failed");
            return -1;
        }
        strcpy(hashTable->table[bucketNum]->key, key);
        hashTable->table[bucketNum]->tree = NULL;
        if ((hashTable->table[bucketNum]->tree = AVLInsert(hashTable->table[bucketNum]->tree, patient)) == NULL) {
            perror("AVLInsert failed");
        }
    }
    else {
    // if node with such key exists insert
        temp = hashTable->table[bucketNum];
        while (temp->next != NULL) {
            if (!strcmp(temp->key, key)) {
                if ((temp->tree = AVLInsert(temp->tree, patient)) == NULL) {
                    perror("AVLInsert failed");
                    return -1;
                }
                return 0;
            }
            temp = temp->next;
        }
    // if last node has the same key
        if (!strcmp(temp->key, key)) {
            if ((temp->tree = AVLInsert(temp->tree, patient)) == NULL) {
                perror("AVLInsert failed");
            }
        }
        else {
            if ((temp->next = bucketInit(temp->next)) == NULL) {
                perror("bucketInit failed");
                return -1;
            }
            temp = temp->next;
            if ((temp->key = malloc(strlen(key) + 1)) == NULL) {
                perror("malloc failed");
                return -1;
            }
            strcpy(temp->key, key);
            temp->tree = NULL;
            if ((temp->tree = AVLInsert(temp->tree, patient)) == NULL) {
                perror("AVLInsert failed");
                return -1;
            }
        }
    }
    return 0;
}

void HTPrint(HashTablePtr hashTable)
{
    HTNodePtr temp;
    for (int i=0; i<hashTable->size; i++) {
        if (hashTable->table[i] != NULL) {
            temp = hashTable->table[i];
            while (temp != NULL) {
                printf("%s --> ", temp->key);
                        // preorder(temp->array[j]->tree);
                        // printf("=========================================================\n");
                temp = temp->next;
            }
        }
        printf("\n");
    }
}

void recursiveDestroy(HTNodePtr node)
{
    if (node == NULL)
        return;
    recursiveDestroy(node->next);
    AVLDestroy(node->tree);
    free(node->key);
    free(node);
}

void HTDestroy(HashTablePtr hashTable)
{
    for (int i=0; i<hashTable->size; i++)
        recursiveDestroy(hashTable->table[i]);
    free(hashTable->table);
    free(hashTable);
}