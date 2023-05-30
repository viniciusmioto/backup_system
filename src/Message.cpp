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
    std::copy(dataValue, dataValue + MAX_DATA_SIZE, data);
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
    return send(socket, buffer, MAX_SIZE, 0);
}