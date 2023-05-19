CC = gcc
CFLAGS = -Wall
OBJECTS = ConexaoRawSocket.o
PROG = client server

all: $(PROG)

client: client.c $(OBJECTS)
	$(CC) $(CFLAGS) client.c $(OBJECTS) -o client

server: server.c $(OBJECTS)
	$(CC) $(CFLAGS) server.c $(OBJECTS) -o server

clean:
	-rm -f *.o

purge: clean
	-rm -f client server *.out