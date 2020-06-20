#include <wordexp.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <errno.h>
#include <arpa/inet.h>
#pragma once

#include "list.h"
#include "message.h"

int querieAnswer(workerInfoPtr, const char* querie, int, int);
void sendQuerie(workerInfoPtr, const char*, int, int);
int queriesHandler(workerInfoPtr,const char*, int);
int selectWorker(const workerInfoPtr, char*);
void listCountriesFunction(workerInfoPtr);