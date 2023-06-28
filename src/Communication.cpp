#include "Communication.hpp"

void mountPackage(int *fileSize, string fileName, int *filePosition, string fileContent, Message &message, int msgCounter, int *bytesRead) {
    if (*fileSize > 0)
        fileContent = get_file_content(fileName, *filePosition, *fileSize, bytesRead);

    memcpy(&message.data, fileContent.c_str(), sizeof(message.data));

    // calculate vertical parity
    for (int i = 0; i < *bytesRead; i++)
        message.parity ^= message.data[i];

    message.sequence = msgCounter;
    message.type = DATA;
    message.size = *bytesRead;

    *filePosition += (MAX_DATA_SIZE);
    *fileSize -= (MAX_DATA_SIZE);
}

int sendMessage(int socket, Message message) {

    unsigned char buffer[MAX_SIZE] = {0};
    memcpy(buffer, &message, MAX_SIZE);

#ifdef DEBUG
    if (message.type != ACK && message.type != NACK)
        cout << "\033[0;32m >> Send [" << message.sequence << "](" << message.type << ")|" << message.size << "|{" << message.parity << "}\033[0m " << message.data << endl;
#endif

    return send(socket, buffer, MAX_SIZE, 0);
}

void sendACK(int socket, int msgCounter) {
    Message message;
    message.type = ACK;
    message.sequence = msgCounter;

#ifdef DEBUG
    cout << "\033[0;34m >> Send ACK [" << message.sequence << "] \033[0m" << endl;
#endif

    sendMessage(socket, message);
}

void sendNACK(int socket, int msgCounter) {
    Message message;
    message.type = NACK;
    message.sequence = msgCounter;

#ifdef DEBUG
    cout << "\033[0;35m >> Send NACK [" << message.sequence << "] \033[0m" << endl;
#endif

    sendMessage(socket, message);
}

void sendERROR(int socket, int msgCounter) {
    Message message;
    message.type = ERROR;
    message.sequence = msgCounter;

#ifdef DEBUG
    cout << "\033[0;35m >> Send ERROR # [" << message.sequence << "] \033[0m" << endl;
#endif

    sendMessage(socket, message);
}

long long getCurrentTime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int waitForACK(int socket, int msgCounter) {
    // timeout setup
    long long start = getCurrentTime();
    struct timeval timeout = {.tv_sec = 0, .tv_usec = TIMEOUT * 1000};
    Message recvMessage;

    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    do {
        // receive messages from server: waiting for ACK
        recv(socket, &recvMessage, MAX_DATA_SIZE, 0);
#ifdef DEBUG
        cout << "\033[0;31m Waiting for ACK " << msgCounter << "\033[0m" << endl;
#endif

        // check if it is the ACK that we are waiting for
        if (recvMessage.initMarker == INIT_MARKER && recvMessage.sequence == msgCounter) {

            // check if it is a ACK
            if (recvMessage.type == ACK) {
#ifdef DEBUG
                cout << "\033[0;34m << Received ACK [" << recvMessage.sequence << "] \033[0m" << recvMessage.data << endl;
#endif
                return ACK;
            }

            // check if it is a NACK
            else if (recvMessage.type == NACK) {
#ifdef DEBUG
                cout << "\033[0;35m << Received NACK [" << recvMessage.sequence << "] \033[0m" << recvMessage.data << endl;
#endif
                return NACK;
            } else if (recvMessage.type == ERROR) {
                cerr << "\033[0;35m << Received ERROR [" << recvMessage.sequence << "] \033[0m" << recvMessage.data << endl;

                return ERROR;
            }

            // until timeout is reached
        }
    } while (getCurrentTime() - start <= TIMEOUT);

// if timeout is reached, return -1
#ifdef DEBUG
    cerr << "\033[0;35m ### ERROR: Lost Message (Timeout) \033[0m" << endl;
#endif

    return -1;
}

bool guaranteeSend(int socket, Message message, int msgCounter) {
    int attempts = 0;
    int result = waitForACK(socket, msgCounter);

    // send message and wait for ACK
    while (result != ACK && result != ERROR) {

#ifdef DEBUG
        cout << "\033[0;33m -> Trying to send again... " << attempts << "º attempt\033[0m" << endl;
#endif
        

        if (attempts > MAX_ATTEMPTS) {

#ifdef DEBUG
            cerr << "\033[0;35m ### ERROR: Could not send file name. \033[0m" << endl;
#endif
            message.parity = 0;

        }

        sendMessage(socket, message);
        attempts++;

        result = waitForACK(socket, msgCounter);

    }
    if (result == ERROR)
        return false;
    
    return true;
}

void adjustMsgCounter(int *msgCounter) {
    if (*msgCounter > MAX_DATA_SIZE)
        *msgCounter = 0;
}

bool checkVerticalParity(Message message) {
    unsigned char parity = 0;

    if (message.parity == 0)
        return true;

    for (int i = 0; i < message.size; i++)
        parity ^= message.data[i];

    return parity == message.parity;
}

void maskMessage(Message &message)
{
    for(int i = 0; i < message.size; i++)
        message.data[i] += 0xFF00;
}

void unmaskMessage(Message &message)
{
    for(int i = 0; i < message.size; i++)
        message.data[i] -= 0xFF00;
}