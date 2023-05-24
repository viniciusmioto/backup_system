#include "message.h"

using namespace std;

int msg_counter = 0;

int main() {
    // network interface eno1 (two computers)
    char sock[] = "lo";
    int socket = ConexaoRawSocket(sock);

    if (socket < 0) {
        perror("\033[0;31m ### ERRO: Could not connect to socket. \033[0m\n");
        exit(1);
    }

    message_t message;
    string file_name;

    while (1) {
        recv(socket, &message, MAX_SIZE, 0);      // receive the message from the client
        if (message.init_marker == INIT_MARKER) { // check if the message is valid

            if (message.sequence == msg_counter) { // check sequence

                if (message.type == FILE_NAME) {
                    // memcpy(file_name.data(), message.data, sizeof(message.data));

                    // get file name and insert a 'b' in the beginning
                    file_name = (char*)(message.data);
                    file_name.insert(0, 1, 'b');

                    cout << "\033[0;32mbackup: " << file_name << " started...\033[0m" << endl;

                } else if (message.type != END_FILE) {
                    write_to_file(file_name, message.data, true);

                } else {
                    msg_counter = 0;
                    cout << "\033[0;32mbackup: " << file_name << " complete.\033[0m" << endl;
                
                }

                msg_counter++;
            }
        }
    }

    return 0;
}
