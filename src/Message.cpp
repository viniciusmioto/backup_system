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

void mountPackage(int *fileSize, string fileName, int *filePosition, string fileContent, Message &message, int msgCounter) {
    if (*fileSize > 0)
        fileContent = get_file_content(fileName, *filePosition, *fileSize);

    memcpy(&message.data, fileContent.c_str(), sizeof(message.data));

    message.sequence = msgCounter;
    message.type = BACKUP_1_ARQ;

    *filePosition += (MAX_DATA_SIZE);
    *fileSize -= (MAX_DATA_SIZE);
}

int sendMessage(int socket, Message message) {

    unsigned char buffer[MAX_SIZE] = {0};
    memcpy(buffer, &message, MAX_SIZE);
    cout << "\033[0;32m Send Message \033[0m" << message.sequence << endl;
    return send(socket, buffer, MAX_SIZE, 0);
}

long long getCurrentTime() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int waitForACK(int socket) {
    long long start = getCurrentTime();
    struct timeval timeout = {.tv_sec = 0, .tv_usec = TIMEOUT * 1000};
    Message recvMessage;

    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    do {
        recv(socket, &recvMessage, MAX_DATA_SIZE, 0);
        if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == ACK) {
            cout << "\033[0;34m Recived ACK \033[0m" << recvMessage.sequence << endl;
            return ACK;
        }
    } while (getCurrentTime() - start <= TIMEOUT);

    cout << "\033[0;31m ### ERROR: Lost Message (Timeout) \033[0m" << endl;

    return -1;
}

void verifySend(int socket, Message message) {
    int attempts = 0;
    while (waitForACK(socket) != ACK) {
        cout << "\033[0;33m -> Trying to send again \033[0m" << endl;
        sendMessage(socket, message);
        attempts++;
        if (attempts >= 5) {
            cout << "\033[0;31m ### ERROR: Could not send file name. \033[0m" << endl;
            exit(1);
        }
    }
}