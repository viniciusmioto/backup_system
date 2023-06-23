#ifndef MESSAGE_H
#define MESSAGE_H

#include "FileHandler.hpp"
#include <sys/time.h>

#define MAX_SIZE 67
#define MAX_DATA_SIZE 63
#define INIT_MARKER 126

using namespace std;



class Message {
public:
    unsigned int initMarker : 8;
    unsigned int size : 6;
    unsigned int sequence : 6;
    unsigned int type : 4;
    unsigned char data[MAX_DATA_SIZE];
    unsigned int parity : 8;

    Message();
    Message(unsigned int sizeValue, unsigned int sequenceValue, unsigned int typeValue, const unsigned char *dataValue, unsigned int parityValue);
};

#endif