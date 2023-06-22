#include "Message.hpp"

using namespace std;

int msgCounter = 0;

int main() {

#ifdef DEBUG
    cout << "DEBUG MODE" << endl;
#endif

    // network interface eno1 (two computers)
    char sock[] = "lo";
    int socket = ConexaoRawSocket(sock);
    unsigned int parity = 0;

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
#ifdef DEBUG
            cout << "\033[33;0mwaiting for message: " << msgCounter << endl;
#endif
            if (recvMessage.sequence == msgCounter) {  // check sequence

                sendACK(socket, msgCounter);

                if (recvMessage.type == FILE_NAME) {
                    // get file name and insert a 'b' in the beginning
                    fileName = (char *)(recvMessage.data);
                    fileName.insert(0, 1, 'b');

                    write_to_file(fileName, NULL, false, 0);

                    cout << "\033[0;32mbackup: " << fileName << " started...\033[0m" << endl;
                } else if (recvMessage.type != END_FILE && recvMessage.data != NULL) {
                    size_t size = recvMessage.size;
                    
                    if (checkVerticalParity) {
                        write_to_file(fileName, recvMessage.data, true, size);
                        sendACK(socket, msgCounter);
                    }
                    else
                        sendNACK(socket, msgCounter);

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
