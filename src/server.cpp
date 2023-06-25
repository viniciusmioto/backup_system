#include "Controller.hpp"

using namespace std;

int msgCounter = 0;

int main() {

#ifdef DEBUG
    cout << "DEBUG MODE" << endl;
#endif

    // network interface eno1 (two computers)
    char sock[] = "lo";
    int socket = RawSocketConnection(sock);
    unsigned int parity = 0;
    string fileName;
    Message recvMessage;

    if (socket < 0) {
        cerr << "\033[0;35m ### ERROR: Could not connect to socket. \033[0m\n";
        exit(1);
    }

    while (1) {
        adjustMsgCounter(&msgCounter);

        // receive the recvMessage from the client (TYPE)
        recv(socket, &recvMessage, MAX_SIZE, 0); // waiting for option

        if (recvMessage.initMarker == INIT_MARKER) { // check if the recvMessage is valid

            if (recvMessage.sequence == msgCounter) { // check sequence

                switch (recvMessage.type) {
                case BACKUP_ONE_FILE:
#ifdef DEBUG
                    cout << "BACKUP_ONE_FILE" << endl;
#endif
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    receiveOneFile(socket, sock, msgCounter);
                    break;

                case BACKUP_GROUP_OF_FILES:
#ifdef DEBUG
                    cout << "BACKUP_GROUP_OF_FILES" << endl;
#endif
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    receiveGroupOfFiles(socket, sock, msgCounter);
                    break;

                default:
                    break;
                }
            }
        }
    }

    return 0;
}
