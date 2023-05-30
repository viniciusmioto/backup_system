#ifndef MESSAGE_H
#define MESSAGE_H

#include "FileHandler.hpp"
#include <sys/time.h>

#define MAX_SIZE 67
#define MAX_DATA_SIZE 63
#define INIT_MARKER 126
#define TIMEOUT 500
#define MAX_ATTEMPTS 8

using namespace std;

typedef enum {
    BACKUP_1_ARQ = 0,
    FILE_NAME = 6,
    END_FILE = 9,
    EXIT_CLIENT = 11,
    ERRO = 12,
    OK = 13,
    ACK = 14,
    NACK = 15,
} msg_type_t;

typedef enum {
    SEND_FILE = 0,
    EXIT = 9,
} client_options_t;

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

/*!
 * \brief Receives information of a message and mount a package to be sent
 * \param fileSize
 * \param fileName
 * \param filePosition
 * \param fileContent
 * \param message
 * \param msgCounter
 */
void mountPackage(int *fileSize, string fileName, int *filePosition, string fileContent, Message &message, int msgCounter, int *bytesRead);

/*!
 * \brief Sends a message to the socket
 * \param socket
 * \param message
 * \return 0 if success, -1 if error
 */
int sendMessage(int socket, Message message);

long long getCurrentTime();

int waitForACK(int socket, int msgCounter);

void verifySend(int socket, Message message, int msgCounter);

void adjustMsgCounter(int *msgCounter);

#endif