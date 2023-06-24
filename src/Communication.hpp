#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Message.hpp"

#define TIMEOUT 500
#define MAX_ATTEMPTS 20

typedef enum {
    BACKUP_ONE_FILE = 0,
    BACKUP_GROUP_OF_FILES = 1,
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

/*!
 * \brief Receives information of a message and mount a package to be sent
 * \param fileSize: size of the file in bytes
 * \param fileName: name of the file with extension
 * \param filePosition: current position of the file content (bytes)
 * \param fileContent: content of the file (data)
 * \param message: message to be sent
 * \param msgCounter: message sequence number
 */
void mountPackage(int *fileSize, string fileName, int *filePosition, string fileContent, Message &message, int msgCounter, int *bytesRead);

/*!
 * \brief Sends a message to the socket
 * \param socket
 * \param message
 * \return 0 if success, -1 if error
 */
int sendMessage(int socket, Message message);

/*!
 * \brief Sends a ACK message
 * \param socket
 * \param msgCounter: message sequence number
 */
void sendACK(int socket, int msgCounter);

/*!
 * \brief Sends a NACK message
 * \param socket
 * \param msgCounter: message sequence number
 */
void sendNACK(int socket, int msgCounter);

/*!
 * \brief Waits for a ACK message for each message sent (TIMEOUT)
 * \param socket
 * \param msgCounter: message sequence number
 * \return 0 if success, -1 if error
 */
int waitForACK(int socket, int msgCounter);

/*!
 * \brief Certifies that the message was sent. Otherwise, tries again until MAX_ATTEMPTS
 * \param socket
 * \param message: message to be sent
 * \param msgCounter: message sequence number
 */
void guaranteeSend(int socket, Message message, int msgCounter);

/*!
 * \brief Adjusts the message counter, max value is 63
 * \param msgCounter: current message sequence number
 */
void adjustMsgCounter(int *msgCounter);

/*!
 * \brief Checks if the message is valid (parity)
 * \param message: message to be checked
 * \return true if valid, false if invalid
 */
bool checkVerticalParity(Message message);

/*!
 * \brief Gets the current time in milliseconds
 * \return current time in milliseconds
 */
long long getCurrentTime();

#endif