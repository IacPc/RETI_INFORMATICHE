# RETI_INFORMATICHE
simple tftp server and client for the university of Pisa networking exam.
to create the executable just type make server(client)

once created the executable the syntax is:

./server [port] [path where the file are located] (example ./server 69 /home/user/files/)

./client [server address] [port] (./client 127.0.0.1 69)

Server side:
Server.c  contains the main thread in which the server is listening for fylesystem reading requests
miothread.h contains the code of the thread responsible for transfering the file requested

Due to the limitation of the course we were told not to implement the writing request and the retransmition
of corrupted packet, just the reading requests.

client side:
client.c implements !help,!mode and !quit code
client_funz.h contains the !get code, i used another file to simplify the while(1) part.

client/server side:

tftp.h contains routine responsible for serializing/deserializing packet accordingly rfc 1350 and some constants
describing the width of the various packets.


