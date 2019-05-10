#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "sem.h"
#include "shm.h" 
#include "structures.h" 
#include "utils.h"
#include "socket.h"

#define PERM 0666
#define SHM_KEY 1234
#define SEM_KEY 1235
#define MAX_CLIENTS 50
#define NOM_FICHIER_ERR_COMPIL "resCompile.txt"

void traiterClient(void*);
void ajouterProgramme(int);
void compilerProgramme(void*);
void executerProgramme(int);
void executer(void*, void*);
long now();

int main(int argc, const char* argv[]){

	if (argc < 2) {
		printf("Veuillez entrer un argument indiquant le port de ce serveur.\n");
		exit(0);
	}
	int port = atoi(argv[1]);
	
	int sockfd = initSocketServer(port, MAX_CLIENTS);
	
	while (1) {		

		int newsockfd = accept(sockfd, NULL,NULL);
		if (newsockfd > 0) {
			// TODO : forkAndRun de traiterClient avec un argument
			void (*traiterClientPtr)(void*) = &traiterClient; 
			forkAndRun1( traiterClientPtr, &newsockfd );
		}
		else {
			perror("Error server");
			exit(0);
		}
	}

	exit(0);
}

void traiterClient(void* newsockfdarg0) {
	int newsockfd = *(int*)(newsockfdarg0);
	// Lecture de la commande
	int commande;
	checkNeg(read(newsockfd, &commande , sizeof(int)), "Error server read commande");
	switch (commande) {
		case -1:
			ajouterProgramme(newsockfd);
			break;
		case -2:
			executerProgramme(newsockfd);
			break;
		default:
			printf("Commande invalide.\n");
			break;
	}
	exit(0);
}

void ajouterProgramme(int newsockfd) {
	
	// Lecture du nombre de caractères du fichier source
	int nbCharNomSource;
	checkNeg(read(newsockfd, &nbCharNomSource , sizeof(int)), "Error server read nbCharNomSource");
	
	// Lecture du nom du fichier source
	char nomSource[TAILLE_NOM];
	checkNeg(read(newsockfd, &nomSource , nbCharNomSource*sizeof(char)), "Error server read nbCharNomSource");
	
	// Début de la mise à jour des informations dans la mémoire partagée
	int semId = getSem(SEM_KEY);
	printf("Tentative de reservation de la memoire partagee...\n");
	down(semId);
	printf("Reservation acceptee. Debut d'ajout d'un programme dans la memoire partagee.\n");
	Programmes* progs = (Programmes*)(getShm(sizeof(Programmes), SHM_KEY).shmPtr); // mémoire partagée
	(progs->listeProgrammes[progs->tailleLog]).id = progs->tailleLog;
	strcpy((progs->listeProgrammes[progs->tailleLog]).nomSource, nomSource);
	(progs->listeProgrammes[progs->tailleLog]).nbExec = 0;
	(progs->listeProgrammes[progs->tailleLog]).tpsExec = 0;
	int idProgramme = progs->tailleLog;
	printf("Valeur int tailleLog avant incrementation dans shm : %d\n", idProgramme); // TODO supprimer cette ligne quand test OK

	// Nom du fichier = id.c
	char nomFichierC[TAILLE_NOM];
	sprintf(nomFichierC, "code/%d.c", idProgramme);
	printf("%s\n", nomFichierC); // TODO supprimer cette ligne quand test OK
	int fichierfd = open(nomFichierC, O_CREAT | O_TRUNC | O_WRONLY, PERM);
	
	// Lecture du contenu et écriture dans le fichier id.c
	char buff[TAILLE_NOM];
	while (readAndWrite(newsockfd, fichierfd, buff, TAILLE_NOM*sizeof(char))); // lecture tant que pas EOF
	
	// Compilation du programme id.c
	int errFd = open(NOM_FICHIER_ERR_COMPIL, O_CREAT | O_WRONLY| O_TRUNC, PERM); // création du fichier
	checkNeg(errFd, "Error open");	
	int stderr_copy = dup(2); // redirection de stderr
	checkNeg(stderr_copy, "Error dup");	
	int ret = dup2(errFd, 2);
	checkNeg(ret, "Error dup2");
	void (*compilerProgrammePtr)(void*) = &compilerProgramme; 
	pid_t compPid = forkAndRun1( compilerProgrammePtr, &idProgramme );
	int status;
	checkNeg(waitpid(compPid, &status, 0), "Error waitpid serveur"); // attente de la fin de l'exécution de la compilation	
	ret = dup2(stderr_copy, 2); // rétablissement de stderr
	checkNeg(ret, "Error dup");
	close(stderr_copy);

	// On renvoie le numéro associé au programme et le résultat des erreurs tout en indiquant si une erreur a été générée (errorGen)
	checkNeg(write(newsockfd, &idProgramme, sizeof(int)), "Error write id newsockfd");
	int nbFoisLu = 0;
	while (readAndWrite(errFd, newsockfd, buff, TAILLE_NOM * sizeof(char))) { // lecture tant que pas EOF	
		nbFoisLu++;
	}
	(progs->listeProgrammes[progs->tailleLog]).errorGen = nbFoisLu > 0 ? 0 : 1;// nbFoisLu > 0 => fichier non vide => erreur = 0

	// On ferme le fichier des erreurs
	close(errFd);
	
	// On ferme le fichier id.c
	close(fichierfd);

	// On incrémente tailleLog pour le prochain programme
	(*progs).tailleLog++; 
	printf("Valeur int tailleLog après incrementation dans shm : %d\n", idProgramme); // TODO supprimer cette ligne quand test OK

	// Fin de la mise à jour de la mémoire partagée
	sshmdt(progs);
	up(semId);
	printf("Fin d'ajout d'un programme dans la memoire partagee.\n");
}

void compilerProgramme(void * idProgrammearg0) {
	int idProgramme = *(int*)(idProgrammearg0);
	char nomFichier[TAILLE_NOM];
	sprintf(nomFichier, "code/%d", idProgramme);
	char nomFichierC[TAILLE_NOM];
	sprintf(nomFichierC, "code/%d.c", idProgramme);
	execl("/usr/bin/gcc", "gcc", "-o", nomFichier, nomFichierC, NULL);
  	perror("Error execl compiler");
}

void executerProgramme(int newsockfd) {

	// Lecture de l'identifiant du programme
	int idProgramme;
	checkNeg(read(newsockfd, &idProgramme , sizeof(int)), "Error server read idProgramme");

	// On renvoie cet identifiant au client
	checkNeg(write(newsockfd, &idProgramme, sizeof(int)), "Error server write idProgramme");

	// Accès à la mémoire partagée
	int semId = getSem(SEM_KEY);
	printf("Tentative de reservation de la memoire partagee...\n");
	down(semId);
	printf("Reservation acceptee. Debut d'ajout d'un programme dans la memoire partagee.\n");
	Programmes* progs = (Programmes*)(getShm(sizeof(Programmes), SHM_KEY).shmPtr); // mémoire partagée
	int etatProgramme = -2; // on considère qu'il n'existe pas par défaut
	int idProgrammeMax = progs->tailleLog;
	int tempsExecution = 0;
	int codeRetour = 0;
	if (idProgramme >= idProgrammeMax) {
		etatProgramme = -2; // état d'un programme inexistant
	}
	else {
		if ((progs->listeProgrammes[idProgramme]).errorGen == 0) {
			etatProgramme = -1;
		}
		else {
		    long t1 = now();
			int status;
			void (*executerPtr)(void*, void*) = &executer;
			pid_t execPid = forkAndRun2( executerPtr, &idProgramme, &(progs->noExec));    		
    		checkNeg(waitpid(execPid, &status, 0), "Error waitpid serveur"); // attente de la fin de l'exécution
    		long t2 = now();
    		bool executionOk = (bool)WIFEXITED(status);
			if (executionOk) {
				etatProgramme = 1;
			}
			else {
				etatProgramme = 0;
			}
			codeRetour = WEXITSTATUS(status);
    		tempsExecution = t2 - t1;
		}
	}

	// On renvoie l'état du programme
	checkNeg(write(newsockfd, &etatProgramme, sizeof(int)), "Error server write etatProgramme");

	// On renvoie le temps d'exécution du programme
	checkNeg(write(newsockfd, &tempsExecution, sizeof(int)), "Error server write tempsExecution");

	// On renvoie le code de retour du programme
	checkNeg(write(newsockfd, &codeRetour, sizeof(int)), "Error server write codeRetour");

	// On renvoie l'affichage à la sortie standard du programme
	char resultatFichier[TAILLE_NOM];
	sprintf(resultatFichier, "code/%d.txt", progs->noExec);
	int resultatFd = open(resultatFichier, O_CREAT | O_TRUNC | O_RDONLY, PERM);
	char buff[TAILLE_NOM];
	while (readAndWrite(resultatFd, newsockfd, buff, TAILLE_NOM * sizeof(char))); // lecture tant que pas EOF

	// Mise à jour du temps d'exécution et du nombre d'exécutions
	((*progs).listeProgrammes)[idProgramme].tpsExec += tempsExecution;
	((*progs).listeProgrammes)[idProgramme].nbExec++;

	// On incrémente le numéro d'exécution pour le nom du fichier résultats d'exécution de la prochaine exécution
	(progs->noExec)++;

	// Fin de la mise à jour de la mémoire partagée
	sshmdt(progs);
	up(semId);
	printf("Fin d'execution du programme %d.\n", idProgramme);
}

void executer(void *idProgrammearg0, void *noExecarg1) {
	int idProgramme = *(int*)(idProgrammearg0);
	int noExec = *(int*)(noExecarg1);
	char cheminFichier[TAILLE_NOM];
	sprintf(cheminFichier, "code/%d", idProgramme);
	char nomFichier[TAILLE_NOM];
	sprintf(nomFichier, "%d", idProgramme);
	char resultatFichier[TAILLE_NOM];
	sprintf(resultatFichier, "code/%d.txt", noExec);
	int resultatFd = open(resultatFichier, O_CREAT | O_TRUNC | O_WRONLY, PERM);
  dup2(resultatFd, 1);  // redirige stdout vers le fichier qui contient le résultat de l'exécution
	execl(cheminFichier, nomFichier, NULL);
  perror("Error exec executer");
}

long now() {
  struct timeval tv;  
  int res = gettimeofday(&tv, NULL);
  checkNeg(res, "Error gettimeofday");  
  return tv.tv_sec * 1000000 + tv.tv_usec;
}