#include "Message.hpp"

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

        unsigned char buffer[MAX_SIZE] = {0};
        Message message;
        int filePosition = 0;
        int fileSize = 0;
        string fileName;
        string fileContent;

        if (msgCounter >= MAX_DATA_SIZE)
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
            message.sequence = msgCounter++;
            sendMessage(socket, message);


            // loop until get all content (limit of MAX_DATA_SIZE bytes each message)
            while (fileSize > 0) {
                mountPackage(&fileSize, fileName, &filePosition, fileContent, message, msgCounter);

                sendMessage(socket, message); // send the file content

                msgCounter++;
            }

            // send last message to inform that the whole file was sent
            memcpy(&message.data, "", MAX_SIZE);
            message.sequence = msgCounter;
            message.type = END_FILE;
            sendMessage(socket, message);
            msgCounter = 0;

            break;

        case EXIT:
            exit(0);
            break;
        }
    }

    return 0;
}
