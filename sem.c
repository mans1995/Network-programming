#include "sem.h"
#include "utils.h"

#define PERM 0666

int initSem(int val, int semKey){
  // CREATE A SET OF ONE SEMAPHORE.
  // THE NUMBER ASSOCIATED WITH THIS SEMAPHORE IS 0.
  
  int semId = semget(semKey, 1, IPC_CREAT | PERM);
  checkNeg(semId, "Error semget");

  // INIT THE SEMAPHORE VALUE TO val
  union semun arg; 
  arg.val = val;
  
  int rv = semctl(semId, 0, SETVAL, arg);
  checkNeg(rv, "Error semctl");

  return semId;
}

int getSem(int semKey){
  int semId = semget(semKey, 0, IPC_CREAT | PERM);
  checkNeg(semId, "Error semget");
  return semId;
} 

void addSem(int val, int semId){
  struct sembuf sem; 
  sem.sem_num = 0; 
  sem.sem_op = val; 
  sem.sem_flg = 0;

  int rc = semop(semId, &sem, 1);
  checkNeg(rc, "Error semop");
}

void down(int semId) {
  addSem(-1, semId);
}

void up(int semId) {
  addSem(1, semId);
}


void delSem(int semId){
  int rv = semctl(semId, 0, IPC_RMID);
  checkNeg(rv, "Error semctl");
}
