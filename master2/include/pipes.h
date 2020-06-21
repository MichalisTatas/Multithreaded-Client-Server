#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#pragma once

int closePipe(char*, int, char*);
int createPipe(char*, int, int, char*);
int msgDecomposer(int, char*, int);
char* msgComposer(int, int);