#include "sem.h"
#include "shm.h" 
#include "structures.h" 
#include "utils.h"
#include "socket.h"


#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define TAILLE_COMMANDE 255
#define TAILLE_NOMBRE 50
#define FILS_MINUTERIE 1
#define PERE 0
#define NB_PROG_EXEC_PARALLELE_MAX 50


//PIPE_PRINCIPALE
int pipePrincipal[2];

// COMMANDE + <PATH>
void ajouterUnProgramme(char* adr, int port, char *nomFichier){
	int sockfd;
	//INITIALISATION DU SOCKET
	checkNeg((sockfd = initSocketClient(adr, port)), "error init socket client - client maint");

	//CLIENT ENVOIE
	int commandeAjouter = -1;
	int tailleNomFichier = strlen(nomFichier);
	checkNeg(write(sockfd,&commandeAjouter,sizeof(int)),"error write");
	checkNeg(write(sockfd,&tailleNomFichier,sizeof(int)),"error write");
	checkNeg(write(sockfd,nomFichier,tailleNomFichier),"error write");

	char bufRd[TAILLE_NOM];
	int fd, nbChar;

	//LECTURE DU FICHIER ET ECRITURE SUR SOCKET
	checkNeg((fd = open(nomFichier, O_RDONLY, 0666)),"error open file - client ajouterUnProgramme");
	while((nbChar = read(fd,bufRd,TAILLE_NOM))){
		checkNeg(nbChar,"error read file - client ajouterUnProgramme");
		checkNeg(write(sockfd,bufRd,nbChar),"error write sockfd- client ajouterUnProgramme");		
	}
	shutdown(sockfd, SHUT_WR);

	//LECTURE SUR SOCKET ET AFFICHAGE ECRAN
	int numeroProgramme;
	checkNeg(read(sockfd,&numeroProgramme,sizeof(int)),"error read sockfd - client ajouterUnProgramme");
	printf("\nNumero du programme : %d\n", numeroProgramme);

	//SET MEMORY DU BUFRD A 0
	memset(bufRd,0,TAILLE_NOM);
	//LECTURE COMPILATION 
	printf("Description de l'erreur (aucune description si aucune erreur):\n");
	while((nbChar = read(sockfd, bufRd, TAILLE_NOM))){
		checkNeg(nbChar, "error read file - client ajouterUnProgramme");
		printf("%s", bufRd);
	}
	printf("\n");
	
	close(sockfd);
}



//COMMANDE @ NUMERO_PROGRAMME
void executerUnPrgramme(char * adr,int port, int num){	
	int sockfd;
	//INITIALISATION DU SOCKET
	checkNeg((sockfd = initSocketClient(adr, port)), "error init socket client - client maint");

	//CLIENT ENVOIE
	int commandeExecuter = -2;
	checkNeg(write(sockfd, &commandeExecuter, sizeof(int)), "error write 1");
	checkNeg(write(sockfd, &num, sizeof(int)), "error write 2"); // client envoie numero programme
	shutdown(sockfd, SHUT_WR);
	//

	int entier;

	//CLIENT RECOIT
	//NUMERO PROGRAME
	checkNeg(read(sockfd,&entier,sizeof(int)),"error read - client executerUnProgramme");
	printf("\n[Resultat de l'execution du programme %d]\n", entier);
	memset(&entier,0,sizeof(int));
	
	//ETAT PROGRAMME
	checkNeg(read(sockfd,&entier,sizeof(int)),"error read - client executerUnProgramme");
	printf("Etat : %d\n", entier);
	memset(&entier,0,sizeof(int));

	//TEMPS D EXECUTION
	checkNeg(read(sockfd,&entier,sizeof(int)),"error read - client executerUnProgramme");
	printf("Temps d'execution : %d\n", entier);
	memset(&entier,0,sizeof(int));

	//CODE DE RETOUR
	checkNeg(read(sockfd,&entier,sizeof(int)),"error read - client executerUnProgramme");
	printf("Code de retour : %d\n", entier);
	memset(&entier,0,sizeof(int));

	//AFFICHAGE SORTIE STANDARD DU PROGRAMME
	char bufRd[TAILLE_NOM];
	int nbChar;
	printf("Affichage a la sortie standard du programme :\n");
	while((nbChar = read(sockfd, bufRd, TAILLE_NOM))){
		checkNeg(nbChar, "error read file - client ajouterUnProgramme");
		printf("%s", bufRd);
	}
	printf("\n");
	memset(bufRd,0,TAILLE_NOM);

	close(sockfd);
}

//COMMANDE * NUMERO_PROGRAMME
void executerUnProgrammeRecurrente(char* adr, int port, int num){
	Message msg;
	msg.code = PERE;
	msg.numProgramme = num;
	msg.port = port;
	msg.adr = adr;
	checkNeg(write(pipePrincipal[1],&msg,sizeof(msg)),"error write pipe - client executerUnProgrammeRecurrente");
}


//FILS_MINUTERIE
void battementCoeur(void* delayarg0) {
	close(pipePrincipal[0]);
	int delay = *(int*)(delayarg0);
	Message msg;
	msg.code = FILS_MINUTERIE;
	while(true){
		checkNeg(write(pipePrincipal[1],&msg, sizeof(msg)),"error write - client battementCoeur");
		sleep(delay);
	}
	close(pipePrincipal[1]);
}


//FILS_EXEC_PROG_REC
void execProgRec(){
	close(pipePrincipal[1]);	 
	Message tableProg[NB_PROG_EXEC_PARALLELE_MAX];
	int tailleLog = 0;
	while(true){
		Message msg;
		checkNeg(read(pipePrincipal[0],&msg,sizeof(msg)), "Error read boucle infinie execProgRec");
		if(msg.code == FILS_MINUTERIE && tailleLog > 0){
			printf("\nDebut de l'execution recurrente.\n");
			for(int i=0; i < tailleLog ; i++){								
				executerUnPrgramme(tableProg[i].adr,tableProg[i].port, tableProg[i].numProgramme);
			}
			printf("Fin de l'execution recurrente.\n\n");
			printf("*********************************************************\n");
			printf("+ <chemin d'un fichier C> : Pour ajouter un programme\n"); 
			printf("* num : Pour executer un programme de maniere recurrente\n");
			printf("@ num : Pour executer un programme\n");
			printf("q : Pour quitter l'application\n");
			printf("*********************************************************\n");
			printf("*********************************************************\n");
		}
		//SI LE MESSAGE VIENT DU PERE 
		if(msg.code == PERE){
			printf("Le programme a ete ajoute aux programmes executes de facon recurrente.\n");
			tableProg[tailleLog++] = msg;
		}

		//SET MEMORY DE MSG A 0
		memset(&msg,0,sizeof(msg));
	} 
	close(pipePrincipal[0]);

}

int main(int argc, const char* argv[]){
	
	// EXIT SI ARGUMENTS TROP PETIT 
	if(argc < 4){
		printf("Veuillez entrer 3 arguments valdides : l'adr / le port / le delay\n");
		exit(0);
	}

	//VARIABLES 
	char adr[16];
	strcpy(adr,argv[1]);
	int port = atoi(argv[2]);	
	int delay = atoi(argv[3]);
	char commande[TAILLE_COMMANDE];
	char commandeArgu[TAILLE_COMMANDE]; 
	char numero[TAILLE_NOMBRE];
	int numeroProg ;

	//PIPE
	checkNeg(pipe(pipePrincipal),"error pipe"); 


	//FORK AND RUN FILS BATTEMENT DE COEUR
	void (*battementCoeurPtr)(void*) = &battementCoeur; 
	pid_t cpidBattCoeur = forkAndRun1( battementCoeurPtr, &delay);


	//FORK AND RUN FILS RECURRENT
	pid_t cpidexecProgRec = forkAndRun(&execProgRec);
	close(pipePrincipal[0]);
	

	//SLEEP POUR QUE LES FILS SONT BIEN SET
	sleep(1);

	printf("*********************************************************\n");
	printf("**********************----MENU----***********************\n");
	
	while(true){

		//CLEAN LA MEMOIRE 
		memset(commande,0,TAILLE_COMMANDE);
		memset(commandeArgu,0,TAILLE_COMMANDE);
		printf("*********************************************************\n");
		printf("+ <chemin d'un fichier C> : Pour ajouter un programme\n"); 
		printf("* num : Pour executer un programme de maniere recurrente\n");
		printf("@ num : Pour executer un programme\n");
		printf("q : Pour quitter l'application\n");
		printf("*********************************************************\n");
		printf("*********************************************************\n");

		// READ ENTREE CLAVIER (COMMANDE)
		checkNeg(read(0,commande,TAILLE_COMMANDE),"error read keyboardCommand - client main");
		commande[strlen(commande)-1]='\0';

		
		// COMMANDE ={ '+', '*', '@', 'q'} 
		char commandeGlobal = commande[0];
		
		
		//NE PREND UNIQUEMENT L ARGUMENT DE LA COMMANDE 
		strcpy(commandeArgu,&commande[2]);

		
		//SWITCH
		switch (commandeGlobal){
		case '+':
			ajouterUnProgramme(adr,port,commandeArgu);
			//TEST SI RENTRE DEDANS
			break;
		case '*':
			strcpy(numero,commandeArgu);
			numeroProg = atoi(numero);
			executerUnProgrammeRecurrente(adr,port,numeroProg);
			//TEST SI RENTRE DEDANS
			break;
		case '@':
			// CONVERSION DU CHAR EN INT DIRECTEMENT (COMMANDEARGU)
			strcpy(numero,commandeArgu);
			numeroProg = atoi(numero);
			executerUnPrgramme(adr,port,numeroProg);
			//TEST SI RENTRE DEDANS
			break;
		case 'q':
			//QUITTE L APPLICATION
			printf("Vous quittez l'application.\n");
			close(pipePrincipal[1]);

			//tuer les fils avant fermeture de l'application
			checkNeg(kill(cpidBattCoeur, SIGKILL),"error kill - client case q");
			checkNeg(kill(cpidexecProgRec, SIGKILL),"error kill - client case q");

			//TO VERIFY --> WAITPID POUR EVITER LE MODE ZOMBIE
			checkNeg(waitpid(cpidBattCoeur, NULL, 0),"error  waitpid - client case q");
			checkNeg(waitpid(cpidexecProgRec, NULL, 0),"error waitpid - client case q");
			exit(0);
		default:
			printf("Commande invalide.\n");
			break;
		}

	}
	exit(0);
}