#include "message.h"

using namespace std;

int msgCounter = 0;

int main() {
    // network interface eno1 (two computers)
    char sock[] = "lo";
    int socket = ConexaoRawSocket(sock);

    if (socket < 0) {
        perror("\033[0;31m ### ERRO: Could not connect to socket. \033[0m\n");
        exit(1);
    }

    Message message;
    string fileName;

    while (1) {
        recv(socket, &message, MAX_SIZE, 0);      // receive the message from the client
        if (message.initMarker == INIT_MARKER) { // check if the message is valid

            if (message.sequence == msgCounter) { // check sequence

                if (message.type == FILE_NAME) {
                    // get file name and insert a 'b' in the beginning
                    fileName = (char *)(message.data);
                    fileName.insert(0, 1, 'b');
                    write_to_file(fileName, NULL, false);

                    cout << "\033[0;32mbackup: " << fileName << " started...\033[0m" << endl;

                } else if (message.type != END_FILE && message.data != NULL) {
                    write_to_file(fileName, message.data, true);

                } else {
                    msgCounter = 0;
                    cout << "\033[0;32mbackup: " << fileName << " complete.\033[0m" << endl;
                }

                msgCounter++;
            }

            cout << "waiting for: " << msgCounter << endl;
        }
    }

    return 0;
}
