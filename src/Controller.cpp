#include "Controller.hpp"
#include "Communication.hpp"

void sendFileData(int socket, string fileName, int &msgCounter) {
    unsigned char buffer[MAX_SIZE] = {0};
    int fileSize = 0;
    int filePosition = 0;
    int bytesRead = 0;
    string fileContent;

    fileSize = get_file_size(fileName);

    // loop until get all content (limit of MAX_DATA_SIZE bytes each message)
    while (fileSize > 0) {
        // mount package with full data capacity and send it
        Message packageMsg;
        mountPackage(&fileSize, fileName, &filePosition, fileContent, packageMsg, msgCounter, &bytesRead);
        cout << "before mask" << endl;
        for (int i = 0; i < packageMsg.size; i++)
            cout << static_cast<int>(packageMsg.data[i]) << " "; // Print the byte as an integer
        cout << " " << endl;

        maskMessage(packageMsg);
        cout << "after: " << endl;
        sendMessage(socket, packageMsg);

        // check if the message was received
        guaranteeSend(socket, packageMsg, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
    }
}

void sendFile(int socket, string fileName, int &msgCounter) {

    // send first message to inform the file name
    Message fileNameMsg(sizeof(fileName), msgCounter, FILE_NAME, (unsigned char *)fileName.c_str(), 0);
    sendMessage(socket, fileNameMsg);
    guaranteeSend(socket, fileNameMsg, msgCounter);
    msgCounter++;
    adjustMsgCounter(&msgCounter);

    sendFileData(socket, fileName, msgCounter);

    // send last message to inform that the whole file was sent
    Message endFileMsg(sizeof(""), msgCounter, END_FILE, (unsigned char *)"", 0);
    sendMessage(socket, endFileMsg);
    guaranteeSend(socket, endFileMsg, msgCounter);
    msgCounter++;
    adjustMsgCounter(&msgCounter);
}

void sendOneFile(int socket, string fileName, int &msgCounter) {
    if (!fileExists(fileName)) {
        cout << "\033[0;33m Warning: File does not exist. \033[0m" << endl;
        return;
    }

    // send to server that it will receive a file
    Message backupOneMsg(sizeof(""), msgCounter, BACKUP_ONE_FILE, (unsigned char *)"", 0);
    sendMessage(socket, backupOneMsg);

    // check if the message was received
    guaranteeSend(socket, backupOneMsg, msgCounter);
    msgCounter++;
    adjustMsgCounter(&msgCounter);

    sendFile(socket, fileName, msgCounter);
}

void sendGroupOfFiles(int socket, string filesPattern, int &msgCounter) {
    vector<string> files = getGroupOfFiles(filesPattern);

    if (files.empty()) {
        cout << "\033[0;33m Warning: No file was found \033[0m" << endl;
        return;
    } else {
        // send to server that a group of files will be sent
        Message backupGroupMsg(sizeof(""), msgCounter, BACKUP_GROUP_OF_FILES, (unsigned char *)"", 0);
        sendMessage(socket, backupGroupMsg);

        // check if the message was sent correctly
        guaranteeSend(socket, backupGroupMsg, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);

        // send each file
        for (const auto &file : files) {
            sendFile(socket, file, msgCounter);
        }

        // send to server that the group of files was completely sent
        adjustMsgCounter(&msgCounter);
        Message endGroupOfFilesMsg(sizeof(""), msgCounter, END_GROUP_OF_FILES, (unsigned char *)"", 0);
        sendMessage(socket, endGroupOfFilesMsg);
        guaranteeSend(socket, endGroupOfFilesMsg, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
    }
}

string getFileName(char interface[], Message recvMessage) {
    string fileName;

    // get original file name
    fileName = (char *)(recvMessage.data);

    write_to_file(fileName, NULL, false, 0);

    return fileName;
}

void receiveOneFile(int socket, char interface[], int &msgCounter) {
    Message recvMessage;
    string fileName;

    while (recvMessage.type != END_FILE) {
        recv(socket, &recvMessage, MAX_SIZE, 0);

#ifdef DEBUG
        cout << "\033[0;31m Waiting for msg " << msgCounter << "\033[0m" << endl;
#endif

        adjustMsgCounter(&msgCounter);

        if (recvMessage.initMarker == INIT_MARKER && recvMessage.sequence == msgCounter) {

            if (recvMessage.type == FILE_NAME) {
                sendACK(socket, msgCounter);
                // get original file name
                fileName = getFileName(interface, recvMessage);
                cout << "\033[0;32m backup: " << fileName << " started...\033[0m" << endl;

                msgCounter++;
                adjustMsgCounter(&msgCounter);

            } else if (recvMessage.type == DATA && recvMessage.data != NULL) {
                size_t size = recvMessage.size;

                unmaskMessage(recvMessage);
                for (int i = 0; i < recvMessage.size; i++)
                    cout << static_cast<int>(recvMessage.data[i]) << " "; // Print the byte as an integer
                cout << " " << endl;

                // parity check: ACK or NACK
                if (checkVerticalParity(recvMessage)) {

                    write_to_file(fileName, recvMessage.data, true, size);
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    adjustMsgCounter(&msgCounter);

                } else {
                    sendNACK(socket, msgCounter);
                    // do not increment msgCounter!
                }
            }
        }
    }

    if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == END_FILE) {
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
        cout << "\033[0;32m backup: " << fileName << " complete.\033[0m" << endl;
    }
}

void receiveGroupOfFiles(int socket, char interface[], int &msgCounter) {
    Message recvMessage;
    string fileName;

    while (recvMessage.type != END_GROUP_OF_FILES) {
        recv(socket, &recvMessage, MAX_SIZE, 0);

        adjustMsgCounter(&msgCounter);

        if (recvMessage.initMarker == INIT_MARKER && recvMessage.sequence == msgCounter) {

            if (recvMessage.type == FILE_NAME) {
                sendACK(socket, msgCounter);
                // get original file name
                fileName = getFileName(interface, recvMessage);
                cout << "\033[0;32m backup: " << fileName << " started...\033[0m" << endl;

                msgCounter++;
                adjustMsgCounter(&msgCounter);

            } else if (recvMessage.type == DATA && recvMessage.data != NULL) {
                size_t size = recvMessage.size;

                // parity check: ACK or NACK
                if (checkVerticalParity(recvMessage)) {
                    write_to_file(fileName, recvMessage.data, true, size);
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    adjustMsgCounter(&msgCounter);
                } else {
                    sendNACK(socket, msgCounter);
                    // do not increment msgCounter!
                }
            } else if (recvMessage.type == END_FILE) {
                sendACK(socket, msgCounter);
                cout << "\033[0;32m backup: " << fileName << " complete.\033[0m" << endl;
                msgCounter++;
                adjustMsgCounter(&msgCounter);
            }
        }
    }

    if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == END_GROUP_OF_FILES) {
        cout << " BACKUP_GROUP_OF_FILES: complete.\033[0m" << endl;
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
    }
}

string getCurrentDirectory() {
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL)
        return string(buffer);
    else
        cerr << "Failed to get the current directory." << endl;

    return "";
}

bool changeDirectory(const string &path) {
    // change local directory to path
    if (chdir(path.c_str()) == 0) {
        cout << "Current directory changed to: " << getCurrentDirectory() << endl;
        return true;
    } else {
        cerr << "\033[0;33m Warning: Could not Change Directory \033[0m" << path << endl;
        return false;
    }
}

void sendServerDirectory(int socket, string path, int &msgCounter) {
    // send a path to the server to change directory
    Message chooseDirMsg(sizeof(""), msgCounter, SERVER_DIR, (unsigned char *)"", 0);
    memcpy(&chooseDirMsg.data, path.c_str(), sizeof(chooseDirMsg.data));
    sendMessage(socket, chooseDirMsg);

    // check if the server changed its directory
    if (guaranteeSend(socket, chooseDirMsg, msgCounter))
        cout << " SERVER_DIR: changed to " << path << endl;
    else
        cout << "\033[0;33m Warning SERVER_DIR: failed to change \033[0m" << endl;
    msgCounter++;
    adjustMsgCounter(&msgCounter);
}

void receiveServerDirectory(int socket, Message recvMessage, int &msgCounter) {
    // check if the directory exists in the server
    if (changeDirectory((char *)recvMessage.data)) {
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
        return;
    }
    // could not find the directory in the server
    sendERROR(socket, msgCounter);
    msgCounter++;
    adjustMsgCounter(&msgCounter);
}

void getServerWorkingDirectory(int socket, int &msgCounter) {
    // ask server for current directory
    Message getServerDirMsg(sizeof(""), msgCounter, SERVER_PWD, (unsigned char *)"", 0);
    sendMessage(socket, getServerDirMsg);

    // wait for server to send current directory
    Message recvMessage;
    while (recvMessage.initMarker != INIT_MARKER || recvMessage.type != SERVER_PWD || recvMessage.sequence != msgCounter + 1) {
        recv(socket, &recvMessage, MAX_SIZE, 0);
    }

    // check if server sent the message
    if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == SERVER_PWD && recvMessage.data != NULL) {
        cout << " SERVER_DIR: " << recvMessage.data << endl;
        msgCounter++;
        adjustMsgCounter(&msgCounter);
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
    }
}

void sendServerWorkingDirectory(int socket, int &msgCounter) {
    // get current server directory and send it to client
    Message currentServerDirMsg(sizeof(""), msgCounter, SERVER_PWD, (unsigned char *)"", 0);
    string serverDir = getCurrentDirectory();
    memcpy(&currentServerDirMsg.data, serverDir.c_str(), sizeof(currentServerDirMsg.data));
    sendMessage(socket, currentServerDirMsg);

    // check if client received the message
    if (guaranteeSend(socket, currentServerDirMsg, msgCounter))
        msgCounter++;
    else
        cout << "\033[0;33m Warning SERVER_DIR: failed to send \033[0m" << endl;

    adjustMsgCounter(&msgCounter);
}

void restoreOneFile(int socket, char interface[], string fileName, int &msgCounter) {
    // send to server that we want to restore a file
    Message recoverFileMsg(sizeof(""), msgCounter, RESTORE_ONE_FILE, (unsigned char *)"", 0);
    memcpy(&recoverFileMsg.data, fileName.c_str(), sizeof(recoverFileMsg.data));
    sendMessage(socket, recoverFileMsg);

    // check if server received the message
    if (guaranteeSend(socket, recoverFileMsg, msgCounter))
        cout << " RESTORE_ONE_FILE: " << fileName << " started...\033[0m" << endl;
    else {
        cout << "\033[0;33m Warning RESTORE_ONE_FILE: failed to ask server \033[0m" << endl;
        return;
    }
    msgCounter++;
    adjustMsgCounter(&msgCounter);

    // receive information to backup one file
    Message recvMessage;
    while (recvMessage.initMarker != INIT_MARKER || recvMessage.type != BACKUP_ONE_FILE || recvMessage.sequence != msgCounter) {
        recv(socket, &recvMessage, MAX_SIZE, 0);
    }

    // receive file data and write to file
    if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == BACKUP_ONE_FILE && recvMessage.data != NULL) {
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
        receiveOneFile(socket, interface, msgCounter);
    }
}

void restoreGroupOfFiles(int socket, char *interface, string filesPattern, int &msgCounter) {
    // send to server that we want to restore a group of files
    Message recoverGroupOfFilesMsg(sizeof(""), msgCounter, RESTORE_GROUP_OF_FILES, (unsigned char *)"", 0);
    memcpy(&recoverGroupOfFilesMsg.data, filesPattern.c_str(), sizeof(recoverGroupOfFilesMsg.data));
    sendMessage(socket, recoverGroupOfFilesMsg);

    // check if server received the message
    if (guaranteeSend(socket, recoverGroupOfFilesMsg, msgCounter))
        cout << " RESTORE_GROUP_OF_FILES: " << filesPattern << " started...\033[0m" << endl;
    else {
        cout << "\033[0;33m Warning: RESTORE_GROUP_OF_FILES: failed to ask server \033[0m" << endl;
        return;
    }
    msgCounter++;
    adjustMsgCounter(&msgCounter);

    // receive information to backup group of files
    Message recvMessage;
    while (recvMessage.initMarker != INIT_MARKER || recvMessage.type != BACKUP_GROUP_OF_FILES || recvMessage.sequence != msgCounter) {
        recv(socket, &recvMessage, MAX_SIZE, 0);
    }

    // receive file data and write to file
    if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == BACKUP_GROUP_OF_FILES && recvMessage.data != NULL) {
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
        receiveGroupOfFiles(socket, interface, msgCounter);
    }
}

void verifyBackup(int socket, string fileName, int &msgCounter) {
    Message verifyBackupMsg(sizeof(""), msgCounter, VERIFY_BACKUP, (unsigned char *)"", 0);
    memcpy(&verifyBackupMsg.data, fileName.c_str(), sizeof(verifyBackupMsg.data));
    sendMessage(socket, verifyBackupMsg);

    Message recvMessage;
    while (recvMessage.initMarker != INIT_MARKER || recvMessage.type != FILE_MD5 || recvMessage.sequence != msgCounter + 1) {
        recv(socket, &recvMessage, MAX_SIZE, 0);
    }

    if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == FILE_MD5 && recvMessage.data != NULL) {
        msgCounter++;
        adjustMsgCounter(&msgCounter);
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);

        string serverFileMD5 = (char *)recvMessage.data;
        string localFileMD5 = calculateMD5(fileName);

        verifyFileIntegrity(localFileMD5, serverFileMD5);
    }
}

void sendVerifyBackup(int socket, char *interface, string fileName, int &msgCounter) {
    Message backupMD5Msg(sizeof(""), msgCounter, FILE_MD5, (unsigned char *)"", 0);
    string backupMD5 = calculateMD5(fileName);
    memcpy(&backupMD5Msg.data, backupMD5.c_str(), sizeof(backupMD5Msg.data));
    sendMessage(socket, backupMD5Msg);

    if (guaranteeSend(socket, backupMD5Msg, msgCounter))
        cout << " VERIFY_BACKUP: " << fileName << endl;
    else
        cout << "\033[0;33m Warning: VERIFY_BACKUP: failed to ask server \033[0m" << endl;

    msgCounter++;
    adjustMsgCounter(&msgCounter);
}