#include "Message.h"

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
    std::copy(dataValue, dataValue + 63, data);
    parity = parityValue;
}

void mount_package(int *fileSize, string fileName, int *filePosition, string fileContent, Message &message, int *msgCounter) {
    if (*fileSize > 0)
        fileContent = get_file_content(fileName, *filePosition, *fileSize);

    memcpy(&message.data, fileContent.c_str(), sizeof(message.data));

    message.sequence = *msgCounter;
    *msgCounter += 1;
    message.type = BACKUP_1_ARQ;

    *filePosition += (MAX_DATA_SIZE);
    *fileSize -= (MAX_DATA_SIZE);
}
