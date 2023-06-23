#include "Communication.hpp"

void sendOneFile(int socket) {
    unsigned char buffer[MAX_SIZE] = {0};
    int msgCounter = 0;
    int filePosition = 0;
    int fileSize = 0;
    int bytesRead = 0;
    string fileName;
    string fileContent;

    cout << "\033[1;36mfile name > \033[0m";
    cin >> fileName;

    fileSize = get_file_size(fileName);

    // send first message to inform the file name - msgCounter = 0
    Message fileNameMsg(sizeof(fileName), msgCounter, FILE_NAME, (unsigned char *)fileName.c_str(), 0);
    sendMessage(socket, fileNameMsg);
    guaranteeSend(socket, fileNameMsg, msgCounter);
    msgCounter++;

    // loop until get all content (limit of MAX_DATA_SIZE bytes each message)
    while (fileSize > 0) {
        adjustMsgCounter(&msgCounter);

        Message packageMsg;

        mountPackage(&fileSize, fileName, &filePosition, fileContent, packageMsg, msgCounter, &bytesRead);
        sendMessage(socket, packageMsg); // send the file content
        guaranteeSend(socket, packageMsg, msgCounter);
        msgCounter++;
    }

    adjustMsgCounter(&msgCounter);

    // send last message to inform that the whole file was sent
    Message endFileMsg(sizeof(""), msgCounter, END_FILE, (unsigned char *)"", 0);
    sendMessage(socket, endFileMsg);
    guaranteeSend(socket, endFileMsg, msgCounter);
    msgCounter = 0;
}
