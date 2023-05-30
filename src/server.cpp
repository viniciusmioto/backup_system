#include "Message.hpp"

using namespace std;

int msgCounter = 0;

int main() {
    // network interface eno1 (two computers)
    char sock[] = "lo";
    int socket = ConexaoRawSocket(sock);

    if (socket < 0) {
        perror("\033[0;31m ### ERROR: Could not connect to socket. \033[0m\n");
        exit(1);
    }

    Message recvMessage;
    string fileName;

    while (1) {
        if (msgCounter > MAX_DATA_SIZE)
            msgCounter = 0;

        recv(socket, &recvMessage, MAX_SIZE, 0);  // receive the recvMessage from the client

        if (recvMessage.initMarker == INIT_MARKER) {  // check if the recvMessage is valid

            cout << "\033[33;0mwaiting for message: " << msgCounter << endl;
            if (recvMessage.sequence == msgCounter) {  // check sequence

                Message ackMessage(sizeof(recvMessage.data), msgCounter, ACK, recvMessage.data, 0);
                sendMessage(socket, ackMessage);  // send ack

                if (recvMessage.type == FILE_NAME) {
                    // get file name and insert a 'b' in the beginning
                    fileName = (char *)(recvMessage.data);
                    fileName.insert(0, 1, 'b');
                    write_to_file(fileName, NULL, false, 0);

                    cout << "\033[0;32mbackup: " << fileName << " started...\033[0m" << endl;
                } else if (recvMessage.type != END_FILE && recvMessage.data != NULL) {
                    size_t size = recvMessage.size;
                    write_to_file(fileName, recvMessage.data, true, size);

                } else {
                    if (recvMessage.type == END_FILE)
                        msgCounter = -1;
                    cout << "\033[0;32mbackup: " << fileName << " complete.\033[0m" << endl;
                }

                msgCounter++;
            }
        }
    }

    return 0;
}
