#include "message.h"

using namespace std;

int msg_counter = 0;

int main() {
    // interface de rede eno1 (dois computadores)
    int socket = ConexaoRawSocket("lo");

    if (socket < 0) {
        perror("\033[0;31m ### ERRO: Falha ao criar conexão raw socket. \033[0m\n");
        exit(1);
    }

    unsigned char buffer[MAX_SIZE];
    message_t message;
    message.init_marker = 126;
    int option;
    unsigned int file_position = 0;
    int file_size = 0;
    string file_name;
    string file_content;

    while (option != 3) {
        cout << "\033[1;36mSELECT ONE OPTION" << endl;
        cout << "0 - Send a File" << endl;
        cout << "9 - Quit" << endl;
        cout << "option > \033[0m";
        cin >> option;

        switch (option) {
        case SEND_FILE:
            cout << "file name > ";
            cin >> file_name;

            file_size = get_file_size(file_name); 

            // loop until get all content (limit of 63 bytes each message)
            while (file_size > 0) {
                mount_package(file_size, file_name, file_position, file_content, message, &msg_counter);

                memcpy(buffer, &message, MAX_SIZE);
                send(socket, buffer, MAX_SIZE, 0); // send the file content

                file_position += (MAX_DATA_SIZE + 1);
                file_size -= (MAX_DATA_SIZE + 1);
            }

            // send last message to inform that the whole file was sent
            memcpy(&message.data, "", MAX_SIZE);
            message.sequence = msg_counter++;
            message.type = END_FILE;

            memcpy(buffer, &message, MAX_SIZE);
            send(socket, buffer, MAX_SIZE, 0);

            break;

        case EXIT:
            cout << "exiting..." << endl;
            message.sequence = msg_counter++;
            message.type = END_FILE;
            memcpy(&message.data, "", MAX_SIZE);
            memcpy(buffer, &message, MAX_SIZE);
            send(socket, buffer, MAX_SIZE, 0);
            exit(0);
            break;
        }
    }

    return 0;
}
