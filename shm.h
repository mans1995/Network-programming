#ifndef _SHM_H_
#define _SHM_H_

#include <sys/ipc.h>
#include <sys/shm.h>

#include "structures.h"

#define PERM 0666

typedef struct {
	int shmId;
	void* shmPtr;
} ShmInfos; 

ShmInfos getShm(int shmSize, int shmKey);

void sshmdt(void* shmPtr);

void delShm(int shmId);

#endif
