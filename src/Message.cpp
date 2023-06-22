#include "Message.hpp"

using namespace std;

Message::Message() {
    // Initialize members with default values
    initMarker = INIT_MARKER;
    size = 0;
    sequence = 0;
    type = 0;
    memset(data, 0, sizeof(data));
    parity = 0;
}

Message::Message(unsigned int sizeValue, unsigned int sequenceValue,
                 unsigned int typeValue, const unsigned char *dataValue, unsigned int parityValue) {
    initMarker = INIT_MARKER;
    size = sizeValue;
    sequence = sequenceValue;
    type = typeValue;
    memcpy(data, dataValue, MAX_DATA_SIZE);
    parity = parityValue;
}

void mountPackage(int *fileSize, string fileName, int *filePosition, string fileContent, Message &message, int msgCounter, int *bytesRead) {
    if (*fileSize > 0)
        fileContent = get_file_content(fileName, *filePosition, *fileSize, bytesRead);

    memcpy(&message.data, fileContent.c_str(), sizeof(message.data));

    // calculate vertical parity
    for (int i = 0; i < *bytesRead; i++)
        message.parity ^= message.data[i];

    message.sequence = msgCounter;
    message.type = BACKUP_1_ARQ;
    message.size = *bytesRead;

    *filePosition += (MAX_DATA_SIZE);
    *fileSize -= (MAX_DATA_SIZE);
}

int sendMessage(int socket, Message message) {

    unsigned char buffer[MAX_SIZE] = {0};
    memcpy(buffer, &message, MAX_SIZE);

#ifdef DEBUG
    if (message.type != ACK && message.type != NACK)
        cout << "\033[0;32m >> Send [" << message.sequence << "] \033[0m" << message.data << endl;
#endif

    return send(socket, buffer, MAX_SIZE, 0);
}

long long getCurrentTime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int waitForACK(int socket, int msgCounter) {
    long long start = getCurrentTime();
    struct timeval timeout = {.tv_sec = 0, .tv_usec = TIMEOUT * 1000};
    Message recvMessage;

    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    do {
        recv(socket, &recvMessage, MAX_DATA_SIZE, 0);
#ifdef DEBUG
        cout << "\033[0;33m Waiting for ACK " << msgCounter << "\033[0m" << endl;
#endif
        if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == ACK && recvMessage.sequence == msgCounter) {
#ifdef DEBUG
            cout << "\033[0;34m << Recived ACK [" << recvMessage.sequence << "] \033[0m" << recvMessage.data << endl;
#endif
            return ACK;
        } else if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == NACK && recvMessage.sequence == msgCounter) {
#ifdef DEBUG
            cout << "\033[0;35m << Recived NACK [" << recvMessage.sequence << "] \033[0m" << recvMessage.data << endl;
#endif
            return NACK;
        }

    } while (getCurrentTime() - start <= TIMEOUT);

#ifdef DEBUG
    cout << "\033[0;35m ### ERROR: Lost Message (Timeout) \033[0m" << endl;
#endif

    return -1;
}

void verifySend(int socket, Message message, int msgCounter) {
    int attempts = 0;
    while (waitForACK(socket, msgCounter) != ACK) {
#ifdef DEBUG
        cout << "\033[0;33m -> Trying to send again..." << attempts << "\033[0m" << endl;
#endif
        sendMessage(socket, message);
        attempts++;
        if (attempts >= MAX_ATTEMPTS) {
#ifdef DEBUG
            cout << "\033[0;35m ### ERROR: Could not send file name. \033[0m" << endl;
#endif
            exit(1);
        }
    }
}

void adjustMsgCounter(int *msgCounter) {
    if (*msgCounter > MAX_DATA_SIZE)
        *msgCounter = 0;
}

bool checkVerticalParity(Message message) {
    unsigned char parity = 0;
    for (int i = 0; i < message.size; i++)
        parity ^= message.data[i];

    return parity == message.parity;
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