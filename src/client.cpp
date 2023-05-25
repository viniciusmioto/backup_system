#include "Message.h"

using namespace std;

int msgCounter = 0;

int main() {
    // interface de rede eno1 (dois computadores)
    const char *interface = "lo";
    int socket = ConexaoRawSocket(interface);

    if (socket < 0) {
        perror("\033[0;31m ### ERRO: Falha ao criar conexão raw socket. \033[0m\n");
        exit(1);
    }

    int option = 0;

    while (option != 9) {

        unsigned char buffer[MAX_SIZE];
        Message message;
        int filePosition = 0;
        int fileSize = 0;
        string fileName;
        string fileContent;

        if (msgCounter >= 63)
            msgCounter = 0;

        cout << "\033[1;36mSELECT ONE OPTION" << endl;
        cout << "0 - Send a File" << endl;
        cout << "9 - Quit" << endl;
        cout << "option > \033[0m";
        cin >> option;

        switch (option) {
        case SEND_FILE:
            cout << "file name > ";
            cin >> fileName;

            fileSize = get_file_size(fileName);

            // send first message to inform the file name
            memcpy(&message.data, fileName.c_str(), MAX_SIZE);
            message.type = FILE_NAME;
            msgCounter = 0;
            message.sequence = 0;
            cout << "sending: " << message.sequence << " " << message.data << endl;
            memcpy(buffer, &message, MAX_SIZE);
            send(socket, buffer, MAX_SIZE, 0);
            msgCounter = 1;

            // loop until get all content (limit of 63 bytes each message)
            while (fileSize > 0) {

                mount_package(&fileSize, fileName, &filePosition, fileContent, message, msgCounter);

                memcpy(buffer, &message, MAX_SIZE);
                send(socket, buffer, MAX_SIZE, 0); // send the file content
                cout << "sending: " << message.sequence << " " << message.data << endl;
                msgCounter++;
            }

            // send last message to inform that the whole file was sent
            memcpy(&message.data, "", MAX_SIZE);
            message.sequence = msgCounter;
            message.type = END_FILE;
            memcpy(buffer, &message, MAX_SIZE);
            send(socket, buffer, MAX_SIZE, 0);
            msgCounter = 0;

            break;

        case EXIT:
            exit(0);
            break;
        }
    }

    return 0;
}
