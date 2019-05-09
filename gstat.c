#include "sem.h"
#include "utils.h"
#include "structures.h"
#include "shm.h"

#define SHM_KEY 1234
#define SEM_KEY 1235

int main(int argc, const char* argv[]){
	if (argc < 2) {
		printf("Veuillez entrer un argument d'identification du numero du programme.\n");
		exit(0);
	}
	int progId = atoi(argv[1]);
	int semId = getSem(SEM_KEY);
	printf("Tentative d'acces a la memoire partagee...\n");
	down(semId);
	printf("Acces obtenu.\n");
	Programmes* progs = (Programmes*)(getShm(sizeof(Programmes), SHM_KEY).shmPtr); // mémoire partagée			
	Programme programme = ((*progs).listeProgrammes)[progId];
	printf("Programme numero %d :\n", programme.id);
	printf("Nom du fichier source : %s\n", programme.nomSource);
	printf("Erreur de compilation : %s\n", programme.errorGen == 0 ? "oui" : "non");
	printf("Nombre d'executions : %d\n", programme.nbExec);
	printf("Temps total d'execution : %dms\n", programme.tpsExec);
	sshmdt(progs);
	// On détache juste la mémoire, pas de delShm ici car on veut juste lire la mémoire,
	// et surtout pas la supprimer.
	up(semId);
	exit(0);
}