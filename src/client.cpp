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
        case SEND_FILE: {
            cout << "file name > ";
            cin >> fileName;

            fileSize = get_file_size(fileName);

            // send first message to inform the file name
            Message fileNameMsg(sizeof(fileName), msgCounter, FILE_NAME, (unsigned char *)fileName.c_str(), 0);
            sendMessage(socket, fileNameMsg);
            msgCounter++;

            // loop until get all content (limit of MAX_DATA_SIZE bytes each message)
            while (fileSize > 0) {
                Message packageMsg;

                mountPackage(&fileSize, fileName, &filePosition, fileContent, packageMsg, msgCounter);
                sendMessage(socket, packageMsg); // send the file content

                msgCounter++;
            }

            // send last message to inform that the whole file was sent
            Message endFileMsg(sizeof(""), msgCounter, END_FILE, (unsigned char *)"", 0);
            sendMessage(socket, endFileMsg);
            msgCounter = 0;

            break;
        }
        case EXIT:
            exit(0);
            break;
        }
    }

    return 0;
}
