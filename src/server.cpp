#include "Communication.hpp"

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
    string fileName;
    Message recvMessage;

    if (socket < 0) {
        cerr << "\033[0;35m ### ERROR: Could not connect to socket. \033[0m\n";
        exit(1);
    }


    while (1) {
        if (msgCounter > MAX_DATA_SIZE)
            msgCounter = 0;

        recv(socket, &recvMessage, MAX_SIZE, 0); // receive the recvMessage from the client

        if (recvMessage.initMarker == INIT_MARKER) { // check if the recvMessage is valid
#ifdef DEBUG
            cout << "\033[0;33m waiting for message: " << msgCounter << "\033[0m" << endl;
#endif
            if (recvMessage.sequence == msgCounter) { // check sequence

                if (recvMessage.type == FILE_NAME) {
                    sendACK(socket, msgCounter);

                    // get original file name 
                    fileName = (char *)(recvMessage.data);

                    // if is in loopback interface, insert a 'b' in the beginning
                    if (strcmp(sock, "lo") == 0)
                        fileName.insert(0, 1, 'b');

                    write_to_file(fileName, NULL, false, 0);

                    cout << "\033[0;32m backup: " << fileName << " started...\033[0m" << endl;
                } else if (recvMessage.type != END_FILE && recvMessage.data != NULL) {
                    size_t size = recvMessage.size;

                    if (checkVerticalParity(recvMessage)) {
                        write_to_file(fileName, recvMessage.data, true, size);
                        sendACK(socket, msgCounter);
                    } else {
                        write_to_file(fileName, recvMessage.data, true, size);
                        sendNACK(socket, msgCounter);
                        msgCounter--; // prevent msgCounter from incrementing
                    }

                } else {
                    sendACK(socket, msgCounter);
                    if (recvMessage.type == END_FILE)
                        msgCounter = -1;
                    cout << "\033[0;32m backup: " << fileName << " complete.\033[0m" << endl;
                }

                msgCounter++;
            }
        }
    }

    return 0;
}
