
CFLAGS = -Wall

server: server.o
	$(CC)  server.o -lpthread -o server

server.o: server.c
	$(CC) $(CFLAGS) -c server.c -lpthread
	 



client: client.o
	$(CC) $(CFLAGS) client.o -o client

client.o: client.c
	$(CC) $(CFLAGS) -c client.c
	 
clean:
	rm *.o


