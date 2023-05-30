CC = g++
OBJECTS = ConexaoRawSocket.o FileHandler.o Message.o 

all: client server

client: client.o $(OBJECTS)

server: server.o $(OBJECTS)

clean:
	-rm -f *.o

purge: clean
	-rm -f client server *.out