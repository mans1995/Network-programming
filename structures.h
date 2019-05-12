#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#define TAILLE_NOM 255
#define NOMBRE_PROGRAMMES 1000
#define CODE_PERE 0
#define CODE_FILS 1

#include <stdbool.h>

typedef struct {
	int id;
	char nomSource[TAILLE_NOM];
	bool errorGen;
	int nbExec;
	int tpsExec; // temps en ms
} Programme;

typedef struct {
	int code; // père = 0 ; filsMinuteur = 1   
	int sockfd;
	int numProgramme;
	int port;
	char* adr;
} Message;

typedef struct {
	Programme listeProgrammes[NOMBRE_PROGRAMMES];
	int tailleLog;
	int noExec;
} Programmes;

#endif
