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

void sendOneFile(int socket) {
    int msgCounter = 0;
    string fileName;

    cout << "\033[1;36mfile name > \033[0m";
    cin >> fileName;

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

vector<string> getGlobResults(const string& pattern) {
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

void getGroupOfFiles() {
    string patterns;
    cout << "\033[1;36mpatterns > \033[0m";

    // Read the user input patterns from the console
    getline(cin >> ws, patterns);

    vector<string> results;

    istringstream iss(patterns);
    string pattern;

    // Process each pattern individually
    while (iss >> pattern) {
        // Call the getGlobResults function to get the matching file paths for each pattern
        vector<string> patternResults = getGlobResults(pattern);

        // Append the pattern results to the overall results vector
        results.insert(results.end(), patternResults.begin(), patternResults.end());
    }

    if (results.empty()) {
        // If no files match the patterns, display a message
        cout << "No files matching the patterns found." << endl;
    } else {
        // Iterate over the results vector and print each file path
        for (const auto& file : results) {
            cout << file << endl;
        }
    }
}
