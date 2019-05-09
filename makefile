CFLAGS=-D_GNU_SOURCE -std=c11 -pedantic -Wall -Wvla -Werror -D_DEFAULT_SOURCE

all: gstat maint serveur client

gstat: gstat.o sem.o shm.o  utils.o
	cc $(CFLAGS) -o gstat gstat.o sem.o shm.o  utils.o

maint: maint.o sem.o shm.o  utils.o
	cc $(CFLAGS) -o maint maint.o sem.o shm.o  utils.o

serveur: serveur.o sem.o shm.o  utils.o socket.o
	cc $(CFLAGS) -o serveur serveur.o sem.o shm.o  utils.o socket.o

client: client.o sem.o shm.o  utils.o socket.o
	cc $(CFLAGS) -o client client.o sem.o shm.o  utils.o socket.o

sem.o : sem.h sem.c
	cc $(CFLAGS) -c sem.c

shm.o : shm.h shm.c
	cc $(CFLAGS) -c shm.c
	 
#utils = fork + check + readAndWrite( lire sur un fd et ecrire sur l'autre "creation d'un fichier" ) 
utils.o: utils.h utils.c   
	cc $(CFLAGS) -c utils.c 

socket.o: socket.h socket.c
	cc $(CFLAGS) -c socket.c

gstat.o: sem.h shm.h structures.h utils.h gstat.c
	cc $(CFLAGS) -c gstat.c

maint.o: sem.h shm.h structures.h utils.h maint.c
	cc $(CFLAGS) -c maint.c

serveur.o: sem.h shm.h structures.h utils.h socket.h serveur.c
	cc $(CFLAGS) -c serveur.c

client.o:  sem.h shm.h structures.h utils.h socket.h client.c
	cc $(CFLAGS) -c client.c

clean:
	rm *.o
	rm gstat
	rm maint
	rm serveur 
	rm client