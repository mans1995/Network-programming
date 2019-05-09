#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


void checkCond(bool cond, char* msg);

void checkNeg(int res, char* msg);

void checkNull(void* res, char* msg);

pid_t forkAndRun(void (*run)());

pid_t forkAndRun1(void (*handler)(void *), void* arg0);

pid_t forkAndRun2(void (*handler)(void*, void*), void* arg0, void* arg1);

void spipe(int* fildes);

// renvoie faux tant que pas fin de lecture
bool readAndWrite(int fdRead, int fdWrite, char buff[], int size);

	
#endif
