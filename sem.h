#ifndef _SEM_H_
#define _SEM_H_

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int initSem(int val, int semKey);

int getSem(int semKey);

void addSem(int val, int semId);

void down(int semId);

void up(int semId);

void delSem(int semId);

#endif