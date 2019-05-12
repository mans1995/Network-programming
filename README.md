# Network-programming

This program has to be used on an UNIX OS because it explicitely uses some UNIX system calls.

server : a server you launch to create, compile and execute files..
client : the client that connects to the server to add .c files on it and asks the server to execute the files it sent to it.
maint  : the program create and delete the shared memory and the semaphores the server needs.
gstat : access the information about the created programs

How to use it?

./maint 1                 # creates the semaphore and shared memory
./server PORT             # launches a server on the PORT
./client HOST PORT DELAY  # launches a client thats connects to the HOST at the PORT and executes the desired programs each DELAY
./maint 2                 # deletes the semaphore and shared memory
