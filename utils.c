#include "utils.h"

void checkCond(bool cond, char* msg){
	if (cond) {
    	perror(msg);
    	exit(EXIT_FAILURE);
 	}  
}

void checkNeg(int res, char* msg) {
  checkCond(res < 0, msg);
}

void checkNull(void* res, char* msg) {
  checkCond(res == NULL, msg);
}
pid_t fork_and_run(void (*handler)()) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run]");
  // child process
  if (childId == 0) {  
    (*handler)();
    exit(EXIT_SUCCESS);
  }
  
  return childId;
}

pid_t forkAndRun1(void (*handler)(void *), void* arg0) {
  int childId = fork();
  checkNeg(childId, "Error [forkAndRun1]");
  
  // child process
  if (childId == 0) {  
    (*handler)(arg0);
    exit(EXIT_SUCCESS);
  }
  
  return childId;
}

pid_t forkAndRun2(void (*handler)(void*, void*), void* arg0, void* arg1) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run]");
  
  // child process
  if (childId == 0) {  
    (*handler)(arg0, arg1);
    exit(EXIT_SUCCESS);
  }
  
  return childId;
}

void spipe(int* fildes) {
  int r = pipe(fildes);
  checkNeg(r, "Error pipe");
}

// que pour des châines de caractères
bool readAndWrite(int fdRead, int fdWrite, char buff[], int size){
  int readSize;
  checkNeg(readSize = read(fdRead, buff,  size), "Error read in readAndWrite");
  checkNeg(write(fdWrite, buff, readSize), "Error write in readAndWrite");
  bool endOfFile;
  if ( buff[ (readSize-1) * sizeof(*buff) ] == EOF ) {
    endOfFile = true;
  }
  else {
    endOfFile = false;
  }
  return endOfFile;
}
