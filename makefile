CFLAGS=-D_GNU_SOURCE -std=c11 -pedantic -Wall -Wvla -Werror -D_DEFAULT_SOURCE

all: gstat maint server client

gstat: gstat.o sem.o shm.o  utils.o
	cc $(CFLAGS) -o gstat gstat.o sem.o shm.o  utils.o

maint: maint.o sem.o shm.o  utils.o
	cc $(CFLAGS) -o maint maint.o sem.o shm.o  utils.o

server: server.o sem.o shm.o  utils.o socket.o
	cc $(CFLAGS) -o server server.o sem.o shm.o  utils.o socket.o

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

server.o: sem.h shm.h structures.h utils.h socket.h server.c
	cc $(CFLAGS) -c server.c

client.o:  sem.h shm.h structures.h utils.h socket.h client.c
	cc $(CFLAGS) -c client.c

clean:
	rm *.o
	rm gstat
	rm maint
	rm server 
	rm client