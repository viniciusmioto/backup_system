#include "file_handler.h"

using namespace std;

string get_file_content(string fileName, unsigned int startPosition, int fileSize) {
    ifstream file(fileName, ios::binary); // Open the file in binary mode

    if (file.is_open()) {
        file.seekg(startPosition); // Set the starting position
        string content(MAX_DATA_SIZE, '\0');
        file.read(&content[0], MAX_DATA_SIZE);
        file.close(); // Close the file
        return content;
    } else {
        cerr << "\033[0;31m Failed to open the file. \033[0m" << endl;
        return "";
    }
}

unsigned int get_file_size(string fileName) {
    ifstream arq(fileName);

    if (!arq) {
        cout << "\033[0;31mFailed to open the file.\033[0m" << endl;
        return 0;
    }

    stringstream buffer;
    buffer << arq.rdbuf(); // Read the file buffer into the stringstream

    return buffer.str().size(); // Convert the stringstream to a string
}

void write_to_file(string filename, unsigned char data[MAX_DATA_SIZE], bool append) {
    ofstream file;

    if (append) {
        file.open(filename, ios::app);
    } else {
        file.open(filename);
    }

    if (file.is_open()) {
        file << data;
        file.close();

    } else {
        cout << "\033[0;31mError opening the file.\033[0m" << endl;
    }
}
