#include "Controller.hpp"

using namespace std;

int msgCounter = 0;

int main() {

#ifdef DEBUG
    cout << "DEBUG MODE" << endl;
#endif

    // network interface eno1 (two computers)
    char interface[] = "lo";
    int socket = RawSocketConnection(interface);
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
            adjustMsgCounter(&msgCounter);

            if (recvMessage.sequence == msgCounter) { // check sequence

                switch (recvMessage.type) {
                case BACKUP_ONE_FILE:
                    cout << "...BACKUP_ONE_FILE" << endl;
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    adjustMsgCounter(&msgCounter);
                    receiveOneFile(socket, interface, msgCounter);
                    break;

                case BACKUP_GROUP_OF_FILES:
                    cout << "...BACKUP_GROUP_OF_FILES" << endl;
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    adjustMsgCounter(&msgCounter);
                    receiveGroupOfFiles(socket, interface, msgCounter);
                    break;

                case RESTORE_ONE_FILE:
                    cout << "...RESTORE_ONE_FILE" << endl;
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    adjustMsgCounter(&msgCounter);
                    cout << " restore " << recvMessage.data << endl;
                    sendOneFile(socket, string(reinterpret_cast<const char*>(recvMessage.data)), msgCounter);
                    break;

                case RESTORE_GROUP_OF_FILES:
                    cout << "...RESTORE_GROUP_OF_FILES" << endl;
                    sendACK(socket, msgCounter);
                    msgCounter++;
                    adjustMsgCounter(&msgCounter);
                    cout << " restore " << recvMessage.data << endl;
                    sendGroupOfFiles(socket, string(reinterpret_cast<const char*>(recvMessage.data)), msgCounter);
                    break;

                case SERVER_DIR:
                    cout << "...SERVER_DIR" << endl;
                    receiveServerDirectory(socket, recvMessage, msgCounter);
                    break;

                case SERVER_PWD:
                    cout << "...SERVER_PWD" << endl;
                    msgCounter++;
                    adjustMsgCounter(&msgCounter);
                    sendServerWorkingDirectory(socket, msgCounter);
                    break;

                default:
                    break;
                }
            }
        }
    }

    return 0;
}
