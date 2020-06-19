#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>
#pragma once

#include "../include/message.h"
#include "../include/list.h"
#include "../include/queriesHandling.h"

int serverRun(int, int, int, int);
void* threadFunction(void*);
int workerFunction(pthreadArguments*, int);
int clientFunction(pthreadArguments*, int);