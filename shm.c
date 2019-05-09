#include "shm.h"
#include "utils.h"
#include "structures.h"

ShmInfos getShm(int shmSize, int shmKey){
  ShmInfos shmInfos;
  shmInfos.shmId = shmget(shmKey, shmSize, IPC_CREAT | PERM);
  checkNeg(shmInfos.shmId, "Error shmget");
  shmInfos.shmPtr = shmat(shmInfos.shmId, NULL, 0);
  checkCond(shmInfos.shmPtr == (void*) -1, "Error shmat");
  return shmInfos;
}

void sshmdt(void* shmPtr){
	int r = shmdt(shmPtr);
 	checkNeg(r, "Error shmdt");
}

void delShm(int shmId){
  int r = shmctl(shmId, IPC_RMID, NULL);
  checkNeg(r, "Error shmctl");
}
