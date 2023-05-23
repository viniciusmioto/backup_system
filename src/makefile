CC = g++
OBJECTS = ConexaoRawSocket.o  file_handler.o

all: client server

client: client.o $(OBJECTS)

server: server.o $(OBJECTS)

clean:
	-rm -f *.o

purge: clean
	-rm -f client server *.out