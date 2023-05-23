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
        cout << "\033[0;35mSELECIONE AS OPÇÕES ABAIXO" << endl;
        cout << "0 - Enviar arquivo" << endl;
        cout << "9 - Sair" << endl;
        cout << "opção > \033[0m";
        cin >> option;

        switch (option) {
        case SEND_FILE:
            cout << "mensagem > ";
            cin >> file_name;

            file_size = get_file_size(file_name); // pega o tamanho total do arquivo

            while (file_size > 0) {
                mount_package(file_size, file_name, file_position, file_content, message, msg_counter);

                msg_counter++;

                memcpy(buffer, &message, MAX_SIZE);
                send(socket, buffer, MAX_SIZE, 0); // send the file content

                file_position += 64;
                file_size -= 64;
                cout << "file_size: " << file_size << endl;
                cout << "msg: " << message.data << endl;
                cout << "msg_counter: " << msg_counter << endl;
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
