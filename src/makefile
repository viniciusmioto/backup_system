CC = g++
DEBUG = -DDEBUG -g
OBJECTS = ConexaoRawSocket.o FileHandler.o Message.o Communication.o Controller.o

all: client server

debug: CC += $(DEBUG)
debug: CXX += $(DEBUG)
debug: all

client: client.o $(OBJECTS)

server: server.o $(OBJECTS)

clean:
	-rm -f *.o

purge: clean
	-rm -f client server *.out