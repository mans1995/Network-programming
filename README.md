# Network-programming

This program has to be used on an UNIX OS because it explicitely uses some UNIX system calls.

server : a server you launch to create, compile and execute files.<br>
client : the client that connects to the server to add .c files on it and asks the server to execute the files it sent to it.<br>
maint  : the program create and delete the shared memory and the semaphores the server needs.<br>
gstat : access the information about the created programs<br>

How to use it?<br>
<br>
./maint 1                 # creates the semaphore and shared memory<br>
./server PORT             # launches a server on the PORT<br>
./client HOST PORT DELAY  # launches a client thats connects to the HOST at the PORT and executes the desired programs each DELAY<br>
./maint 2                 # deletes the semaphore and shared memory

(project made by a group of two people including myself)
