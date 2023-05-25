#include "message.h"

using namespace std;

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