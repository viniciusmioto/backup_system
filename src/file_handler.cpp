#include "file_handler.h"

using namespace std;

string get_file_content(const string &file_name, unsigned int start_position, int file_size) {
    ifstream file(file_name);

    if (!file) {
        cout << "Failed to open the file." << endl;
        return "";
    }

    file.seekg(start_position);

    // Read the file buffer into the stringstream
    if (file_size >= 63) {
        char buffer[MAX_DATA_SIZE + 1];
        file.read(buffer, MAX_DATA_SIZE);
        return buffer;
    }

    char buffer[file_size + 1];
    file.read(buffer, file_size);

    return buffer; // Convert the stringstream to a string
}

unsigned int get_file_size(const string &file_name) {
    ifstream arq(file_name);

    if (!arq) {
        cout << "Failed to open the file." << endl;
        return 0;
    }

    stringstream buffer;
    buffer << arq.rdbuf(); // Read the file buffer into the stringstream

    return buffer.str().size(); // Convert the stringstream to a string
}

void write_to_file(const string &filename, unsigned char data[MAX_DATA_SIZE], bool append) {
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
        cout << "\033[0;31mError opening the file.\033[0m" << std::endl;
    }
}

