#include <bits/stdc++.h>

#include <fstream>
#include <iostream>
#include <string>

#include "ConexaoRawSocket.h"
#include "message.h"

using namespace std;

int msg_counter = 0;

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

    return buffer;  // Convert the stringstream to a string
}

unsigned int get_file_size(const string &file_name) {
    ifstream arq(file_name);

    if (!arq) {
        cout << "Failed to open the file." << endl;
        return 0;
    }

    stringstream buffer;
    buffer << arq.rdbuf();  // Read the file buffer into the stringstream

    return buffer.str().size();  // Convert the stringstream to a string
}

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
        cout << "\033[0;35mSELECIONE AS OPÇÕES ABAIXO" << endl;
        cout << "0 - Enviar arquivo" << endl;
        cout << "9 - Sair" << endl;
        cout << "opção > \033[0m";
        cin >> option;

        switch (option) {
            case SEND_FILE:
                cout << "mensagem > ";
                cin >> file_name;

                file_size = get_file_size(file_name);  // pega o tamanho total do arquivo

                while (file_size > 0) {

                    if (file_size > 0)
                        file_content = get_file_content(file_name, file_position, file_size);
                    memcpy(&message.data, file_content.c_str(), sizeof(message.data));  // read the file content

                    message.sequence = msg_counter++;
                    message.type = BACKUP_1_ARQ;

                    memcpy(buffer, &message, MAX_SIZE);
                    send(socket, buffer, MAX_SIZE, 0);  // send the file content

                    file_position += 64;
                    file_size -= 64;
                    cout << file_size << endl;
                }
                memcpy(&message.data, "", MAX_SIZE);
                message.sequence = msg_counter++;
                message.type = END_FILE;

                memcpy(buffer, &message, MAX_SIZE);
                send(socket, buffer, MAX_SIZE, 0);

                // need to send message with type END_FILE

                break;

            case EXIT:
                cout << "saindo..." << endl;
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
