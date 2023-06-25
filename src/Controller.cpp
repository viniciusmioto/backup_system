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
        adjustMsgCounter(&msgCounter);

        Message packageMsg;

        mountPackage(&fileSize, fileName, &filePosition, fileContent, packageMsg, msgCounter, &bytesRead);
        sendMessage(socket, packageMsg); // send the file content
        guaranteeSend(socket, packageMsg, msgCounter);
        msgCounter++;
    }
}

void sendFile(int socket, string fileName) {
    int msgCounter = 1;

    // send first message to inform the file name - msgCounter = 0
    Message fileNameMsg(sizeof(fileName), msgCounter, FILE_NAME, (unsigned char *)fileName.c_str(), 0);
    sendMessage(socket, fileNameMsg);
    guaranteeSend(socket, fileNameMsg, msgCounter);
    msgCounter++;

    sendFileData(socket, fileName, msgCounter);

    adjustMsgCounter(&msgCounter);

    // send last message to inform that the whole file was sent
    Message endFileMsg(sizeof(""), msgCounter, END_FILE, (unsigned char *)"", 0);
    sendMessage(socket, endFileMsg);
    guaranteeSend(socket, endFileMsg, msgCounter);
    msgCounter = 0;
}

void sendOneFile(int socket, string fileName) {
    int msgCounter = 0;

    Message backupOneMsg(sizeof(""), msgCounter, BACKUP_ONE_FILE, (unsigned char *)"", 0);
    sendMessage(socket, backupOneMsg);
    guaranteeSend(socket, backupOneMsg, msgCounter);
    msgCounter++;

    sendFile(socket, fileName);
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

void sendGroupOfFiles(int socket, string filesPattern) {
    vector<string> files = getGroupOfFiles(filesPattern);

    if (files.empty()) {
        cout << "No files matched to input." << endl;
    } else {
        for (const auto &file : files) {
            sendOneFile(socket, file);
        }
    }
}

string getFileName(int socket, char sock[], Message recvMessage, int &msgCounter) {
    string fileName;

    // get original file name
    fileName = (char *)(recvMessage.data);

    // if is in loopback interface, insert a 'b' in the beginning
    if (strcmp(sock, "lo") == 0)
        fileName.insert(0, 1, 'b');

    write_to_file(fileName, NULL, false, 0);

    return fileName;
}

void receiveOneFile(int socket, char sock[], int &msgCounter, string &fileName) {
    Message recvMessage;

    while (recvMessage.type != END_FILE) {
        recv(socket, &recvMessage, MAX_SIZE, 0);

        adjustMsgCounter(&msgCounter);

        if (recvMessage.initMarker == INIT_MARKER && recvMessage.sequence == msgCounter) {

            if (recvMessage.type == FILE_NAME) {
                sendACK(socket, msgCounter);
                // get original file name
                fileName = getFileName(socket, sock, recvMessage, msgCounter);
                cout << "\033[0;32m backup: " << fileName << " started...\033[0m" << endl;

                msgCounter++;

            } else if (recvMessage.type == DATA && recvMessage.data != NULL) {
                size_t size = recvMessage.size;

                if (checkVerticalParity(recvMessage)) {
                    write_to_file(fileName, recvMessage.data, true, size);
                    sendACK(socket, msgCounter);
                    msgCounter++;
                } else {
                    sendNACK(socket, msgCounter);
                    // do not increment msgCounter!
                }
            }
        }
    }

    if (recvMessage.initMarker == INIT_MARKER && recvMessage.type == END_FILE) {
        sendACK(socket, msgCounter);
        cout << "\033[0;32m backup: " << fileName << " complete.\033[0m" << endl;
    }

    msgCounter = 0;
}

void receiveGroupOfFiles(int socket, char sock[], int &msgCounter, string &fileName) {
    receiveOneFile(socket, sock, msgCounter, fileName);
}