#include <unistd.h>

#include "sem.h"
#include "shm.h"
#include "utils.h"
#include "structures.h" 

#define SHM_KEY 1234
#define SEM_KEY 1235

int main(int argc , const char* argv[]){
	int commande = 0;
	int duree = 0; // en secondes
	int semId;
	Programmes* progs;
	int shmId;
	if(argc >= 2){
		commande = atoi(argv[1]);
	}
	else{
		printf("Veuillez entrer un argument de commande.\n");
		exit(0);
	}
	if(commande == 3) {
		if (argc >= 3) {
			duree = atoi(argv[2]);
		}
		else{
			printf("Veuillez entrer une duree apres la commande.\n");
			exit(0);
		}
	}
	
	switch(commande){
		case 1:
			getShm(sizeof(Programmes), SHM_KEY); // création mémoire partagée
			initSem(1,SEM_KEY);
			break;
		case 2: 
			// Suppression de la mémoire partagée 
			progs = (Programmes*)(getShm(sizeof(Programmes), SHM_KEY).shmPtr); // mémoire partagée
			shmId = getShm(sizeof(Programmes), SHM_KEY).shmId; // mémoire partagée
			sshmdt(progs);
			delShm(shmId);
			// Suppression de la semaphore
			semId = getSem(SEM_KEY);
			delSem(semId);
			break;
		case 3:
			semId = getSem(SEM_KEY);
			printf("Tentative de reservation de la memoire partagee...\n");
			down(semId);
			printf("Reservation acceptee. Debut de la maintenance.\n");
			sleep(duree);
			up(semId);
			printf("Fin de la maintenance.\n");
			break;
		default:
			printf("commande invalide.\n");
			break;
	}

	exit(0);
}