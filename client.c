#include "sem.h"
#include "shm.h" 
#include "structures.h" 
#include "utils.h"
#include "socket.h"


#include <fcntl.h>

#define TAILLE_COMMANDE 255
#define TAILLE_NOMBRE 50


void ajouterUnProgramme(int sockfd){
	printf("1) Veuillez entrer le nom du fichier source \n");
	char nomFichier[TAILLE_NOM] , bufRd[TAILLE_NOM] , tailleNomFichierToChar[TAILLE_NOMBRE];
	int nbChar, tailleNomFichier , fd;
	checkNeg((nbChar = read(0,nomFichier,TAILLE_NOM*sizeof(char))),"error read keyboard - client ajouterUnProgramme");

	if(nbChar == TAILLE_NOM && nomFichier[strlen(nomFichier)-1] != '\n'){
		perror("error : trop de caractères");
		exit(1);
	}

	nomFichier[strlen(nomFichier)-1]='\0';
	tailleNomFichier = strlen(nomFichier);
	sprintf(tailleNomFichierToChar ,"%d",tailleNomFichier);

	printf("nomFichier : %s \ntaille du nomFichier : %d\n" , nomFichier , tailleNomFichier);
	checkNeg(write(sockfd,nomFichier,tailleNomFichier*sizeof(char)),"error write sockfd - client ajouterUnProgramme");
	checkNeg(write(sockfd,tailleNomFichierToChar,strlen(tailleNomFichierToChar)*sizeof(char)),"error write sockfd - client ajouterUnProgramme");
	

	//TODO : faire avec ReadAndWrite !!! 
	checkNeg((fd = open(nomFichier, O_RDONLY, 0666)),"error open file - clien ajouterUnProgramme");
	while((nbChar = read(fd,bufRd,TAILLE_NOM*sizeof(char)))){
	checkNeg(nbChar,"error read file - client ajouterUnProgramme");
	printf("%s" , bufRd);
	checkNeg(write(sockfd,bufRd,nbChar*sizeof(char)),"error write sockfd- client ajouterUnProgramme");
	}
}
void executerUnPrgramme(int sockfd){
	//TODO fork and run fils 
	printf("2) Veuillez entrer le numero du programme \n");
	char numProgrammeToChar[TAILLE_NOMBRE];
	checkNeg(read(0,numProgrammeToChar,TAILLE_NOMBRE*sizeof(char)),"error read keyboard - client executerUnProgramme");
	numProgrammeToChar[strlen(numProgrammeToChar)-1]= '\0';
	checkNeg(write(sockfd,numProgrammeToChar,strlen(numProgrammeToChar)*sizeof(char)),"error write sockfd - client ajouterUnProgramme");

	//TODO : lire les renvoie du serveur 
}
// MAIN //
int main(int argc, char* argv[]){
	//TODO : pipe
	if(argc < 4){
		printf("Veuillez entrer 3 arguments valdides : l'adr / le port / le delay\n");
		exit(0);
	}
	char adr[16];
	strcpy(adr,argv[1]);
	int port = atoi(argv[2]);
	//int delay = atoi(argv[3]);
	int sockfd = 1;
	char commande[TAILLE_COMMANDE];
	printf("*********************************************************\n");
	printf("**********************----MENU----***********************\n");
	while(1){
		printf("*********************************************************\n");
		printf("+ <chemin d'un fichier C> : Pour ajouter un programme\n"); 
		printf("* num : Pour executer un programme de maniere recurrente\n");
		printf("@ num : Pour executer un programme\n");
		printf("*********************************************************\n");
		printf("*********************************************************\n");
		//struct message pour les commande vers le serveur ?
		checkNeg(read(0,commande,TAILLE_COMMANDE),"error read keyboardCommand - client main");
		//commande[strlen(commande)-1] = '\0';
		char *commandeArgu = &commande[2];
		//commandeArgu[strlen(commandeArgu)-1] = '\0';
		checkNeg(write(1,commandeArgu,strlen(commandeArgu)),"error write screen : client main");
		switch (commande[0]){
		case '+':
			checkNeg((sockfd = initSocketClient(adr,port)),"error initialisation socket - client switchCase -1");
			checkNeg(write(sockfd,commande,TAILLE_COMMANDE*sizeof(char)),"error write sockfd - client switchCase -1");
			ajouterUnProgramme(sockfd);
			close(sockfd);
			continue;
			break;
		case '*':
			checkNeg((sockfd = initSocketClient(adr,port)),"error initialisation socket - client switchCase -1");
			checkNeg(write(sockfd,commande,TAILLE_COMMANDE*sizeof(char)),"error write sockfd - client switchCase -1");
			executerUnPrgramme(sockfd);
			printf("\nfonctionne *");
			close(sockfd);
			continue;
			break;
		case '@':
			checkNeg((sockfd = initSocketClient(adr,port)),"error initialisation socket - client switchCase -1");
			checkNeg(write(sockfd,commande,TAILLE_COMMANDE*sizeof(char)),"error write sockfd - client switchCase -1");
			executerUnPrgramme(sockfd);
			printf("\nfonctionne @");
			close(sockfd);
			continue;
			break;
		case 'q':
			printf("je quitte l'application");
			exit(0);
			break;
		default:
			break;
		}
	}
	exit(0);
}