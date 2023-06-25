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

        Message packageMsg;

        mountPackage(&fileSize, fileName, &filePosition, fileContent, packageMsg, msgCounter, &bytesRead);
        sendMessage(socket, packageMsg); // send the file content
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

    Message backupOneMsg(sizeof(""), msgCounter, BACKUP_ONE_FILE, (unsigned char *)"", 0);
    sendMessage(socket, backupOneMsg);
    guaranteeSend(socket, backupOneMsg, msgCounter);
    msgCounter++;
    adjustMsgCounter(&msgCounter);

    sendFile(socket, fileName, msgCounter);
}

vector<string> getGlobResults(string pattern) {
    vector<string> results;

    glob_t globResult;
    int globStatus = glob(pattern.c_str(), GLOB_TILDE, nullptr, &globResult);

    if (globStatus == 0) {
        // Iterate over the matched file paths and add them to the results vector
        for (size_t i = 0; i < globResult.gl_pathc; ++i) {
            results.push_back(globResult.gl_pathv[i]);
        }
    }

    // Free the memory allocated by glob() by calling globfree()
    globfree(&globResult);

    return results;
}

vector<string> getGroupOfFiles(string filePatterns) {
    vector<string> matched_files;

    istringstream iss(filePatterns);
    string pattern;

    // Process each pattern individually
    while (iss >> pattern) {
        // Call the getGlobResults function to get the matching file paths for each pattern
        vector<string> patternResults = getGlobResults(pattern);

        // Append the pattern matched_files to the overall matched_files vector
        matched_files.insert(matched_files.end(), patternResults.begin(), patternResults.end());
    }

    return matched_files;
}

void sendGroupOfFiles(int socket, string filesPattern, int &msgCounter) {
    vector<string> files = getGroupOfFiles(filesPattern);

    if (files.empty()) {
        cout << "\033[0;33m Warning: No file was found \033[0m" << endl;
        return;
    } else {
        Message backupGroupMsg(sizeof(""), msgCounter, BACKUP_GROUP_OF_FILES, (unsigned char *)"", 0);
        sendMessage(socket, backupGroupMsg);
        guaranteeSend(socket, backupGroupMsg, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);

        for (const auto &file : files) {
            sendFile(socket, file, msgCounter);
        }

        adjustMsgCounter(&msgCounter);
        Message endGroupOfFilesMsg(sizeof(""), msgCounter, END_GROUP_OF_FILES, (unsigned char *)"", 0);
        sendMessage(socket, endGroupOfFilesMsg);
        guaranteeSend(socket, endGroupOfFilesMsg, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
    }
}

string getFileName(int socket, char interface[], Message recvMessage, int &msgCounter) {
    string fileName;

    // get original file name
    fileName = (char *)(recvMessage.data);

    // if is in loopback interface, insert a 'b' in the beginning
    if (strcmp(interface, "lo") == 0)
        fileName.insert(0, 1, 'b');

    write_to_file(fileName, NULL, false, 0);

    return fileName;
}

void receiveOneFile(int socket, char interface[], int &msgCounter) {
    Message recvMessage;
    string fileName;

    while (recvMessage.type != END_FILE) {
        recv(socket, &recvMessage, MAX_SIZE, 0);

        adjustMsgCounter(&msgCounter);

        if (recvMessage.initMarker == INIT_MARKER && recvMessage.sequence == msgCounter) {

            if (recvMessage.type == FILE_NAME) {
                sendACK(socket, msgCounter);
                // get original file name
                fileName = getFileName(socket, interface, recvMessage, msgCounter);
                cout << "\033[0;32m backup: " << fileName << " started...\033[0m" << endl;

                msgCounter++;
                adjustMsgCounter(&msgCounter);

            } else if (recvMessage.type == DATA && recvMessage.data != NULL) {
                size_t size = recvMessage.size;

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
                fileName = getFileName(socket, interface, recvMessage, msgCounter);
                cout << "\033[0;32m backup: " << fileName << " started...\033[0m" << endl;

                msgCounter++;
                adjustMsgCounter(&msgCounter);

            } else if (recvMessage.type == DATA && recvMessage.data != NULL) {
                size_t size = recvMessage.size;

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

bool fileExists(string fileName) {
    ifstream file(fileName);
    return file.good();
}

string getCurrentDirectory() {
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        return std::string(buffer);
    } else {
        std::cerr << "Failed to get the current directory." << std::endl;
        return "";
    }
}

bool changeDirectory(const string &path) {
    if (chdir(path.c_str()) == 0) {
        cout << "Current directory changed to: " << getCurrentDirectory() << endl;
        return true;
    } else {
        cerr << "\033[0;33m Warning: Could not Change Directory \033[0m" << path << endl;
        return false;
    }
}

void sendServerDirectory(int socket, string path, int &msgCounter) {
    Message chooseDirMsg(sizeof(""), msgCounter, SERVER_DIR, (unsigned char *)"", 0);
    memcpy(&chooseDirMsg.data, path.c_str(), sizeof(chooseDirMsg.data));

    sendMessage(socket, chooseDirMsg);
    if (guaranteeSend(socket, chooseDirMsg, msgCounter)) 
        cout << " SERVER_DIR: changed to " << path << endl;
    else 
        cout << "\033[0;33m Warning SERVER_DIR: failed to change \033[0m" << endl;
    msgCounter++;
    adjustMsgCounter(&msgCounter);
}

void receiveServerDirectory(int socket, Message recvMessage, int &msgCounter) {
    if (changeDirectory((char *)recvMessage.data)) {
        sendACK(socket, msgCounter);
        msgCounter++;
        adjustMsgCounter(&msgCounter);
        return;
    }
    sendERROR(socket, msgCounter);
    msgCounter++;
    adjustMsgCounter(&msgCounter);
}

// 0 - test2.txt
// 8 - ./test/
// 0 - test.txt
// 4 - ./test/
// 0 - test.txt