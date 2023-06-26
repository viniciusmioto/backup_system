CC = g++
DEBUG = -DDEBUG -g
OBJECTS = RawSocketConnection.o FileHandler.o Message.o Communication.o Controller.o

all: client server

debug: CC += $(DEBUG)
debug: CXX += $(DEBUG)
debug: all

client: client.o $(OBJECTS) -lcrypto

server: server.o $(OBJECTS) -lcrypto

clean:
	-rm -f *.o

purge: clean
	-rm -f client server *.out