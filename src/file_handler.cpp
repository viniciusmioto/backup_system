#include "file_handler.h"

using namespace std;

string get_file_content(string fileName, unsigned int start_position, int fileSize) {
    ifstream file(fileName);

    if (!file) {
        cout << "\033[0;31mFailed to open the file.\033[0m" << endl;
        return "";
    }

    file.seekg(start_position);

    // Read the file buffer into the stringstream
    if (fileSize >= 63) {
        char buffer[MAX_DATA_SIZE];
        file.read(buffer, MAX_DATA_SIZE);
        return buffer;
    }  
    if (fileSize == 1) {
        char buffer_char;
        file.get(buffer_char);
        return string(1, buffer_char);
    }

    char buffer[fileSize];
    file.read(buffer, fileSize);

    return buffer; // Convert the stringstream to a string
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
