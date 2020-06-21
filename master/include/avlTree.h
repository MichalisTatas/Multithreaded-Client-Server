#pragma once
#include <stdbool.h>
#include "patient.h"

typedef struct treeNode
{
    int height;
    patientPtr patient;
    struct treeNode* left;
    struct treeNode* right;
} treeNode;
typedef treeNode* treeNodePtr;

bool existInTree(treeNodePtr, patientPtr);
void AVLDestroy(treeNodePtr);
void preorder(treeNodePtr);
treeNodePtr AVLInsert(treeNodePtr, patientPtr);