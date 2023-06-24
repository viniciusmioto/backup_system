#include "FileHandler.hpp"

using namespace std;

string get_file_content(string fileName, unsigned int startPosition, int fileSize, int* bytesRead) {
    ifstream file(fileName, ios::binary); // Open the file in binary mode

    if (file.is_open()) {
        file.seekg(startPosition); // Set the starting position
        string content(MAX_DATA_SIZE, '\0');
        file.read(&content[0], MAX_DATA_SIZE);
        *bytesRead = file.gcount();
        file.close(); // Close the file
        return content;
    } else {
        cerr << "\033[0;35m ### ERROR: Failed to open the file. \033[0m" << endl;
        return "";
    }
}

unsigned int get_file_size(string fileName) {
    ifstream arq(fileName);

    if (!arq) {
        cerr << "\033[0;35m ### ERROR: Failed to open the file.\033[0m" << endl;
        return 0;
    }

    stringstream buffer;
    buffer << arq.rdbuf(); // Read the file buffer into the stringstream

    return buffer.str().size(); // Convert the stringstream to a string
}

void write_to_file(string fileName, unsigned char data[MAX_DATA_SIZE], bool append, size_t dataSize) {
    ofstream file;

    // check if the data will be appended
    if (append) {
        file.open(fileName, ios::binary | ios::app);
    } else { // otherwise, overwrite the file
        file.open(fileName, ios::binary);
    }

    if (file.is_open()) {

        file.write((const char *)(data), dataSize);
        file.close();
        
    } else {
        cerr << "\033[0;35m ### ERROR: Could not write file  " << fileName <<".\033[0m" << endl;
    }
}
