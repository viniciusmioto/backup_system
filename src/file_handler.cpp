#include "file_handler.h"

using namespace std;

string get_file_content(string file_name, unsigned int start_position, int file_size) {
    ifstream file(file_name);

    if (!file) {
        cout << "\033[0;31mFailed to open the file.\033[0m" << endl;
        return "";
    }

    file.seekg(start_position);

    // Read the file buffer into the stringstream
    if (file_size >= 63) {
        char buffer[MAX_DATA_SIZE];
        file.read(buffer, MAX_DATA_SIZE);
        return buffer;
    }  
    if (file_size == 1) {
        char buffer_char;
        file.get(buffer_char);
        return string(1, buffer_char);
    }

    char buffer[file_size];
    file.read(buffer, file_size);

    return buffer; // Convert the stringstream to a string
}

unsigned int get_file_size(string file_name) {
    ifstream arq(file_name);

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
