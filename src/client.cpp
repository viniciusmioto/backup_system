#include "Controller.hpp"

using namespace std;

int main() {
    // interface de rede eno1 (dois computadores)
    const char *interface = "lo";
    int socket = ConexaoRawSocket(interface);

    if (socket < 0) {
        cerr << "\033[0;35m ### ERROR: Could not connect to socket. \033[0m\n";
        exit(1);
    }

    int option = 0;

    while (option != 9) {

        cout << "\033[1;36mSELECT ONE OPTION" << endl;
        cout << "0 - Send a File" << endl;
        cout << "1 - List Files" << endl;
        cout << "9 - Quit" << endl;
        cout << "option > \033[0m";
        cin >> option;

        switch (option) {
        case SEND_FILE: {
            
            sendOneFile(socket);

            break;
        }
        case 1: {
            getGroupOfFiles();
            break;
        }
        case EXIT:
            exit(0);
            break;
        }
    }

    return 0;
}
