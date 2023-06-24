#include "Controller.hpp"

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
                receiveOneFile(socket, sock, recvMessage, msgCounter, fileName);
            }
        }
    }

    return 0;
}
