#include "Controller.hpp"

using namespace std;

int msgCounter = 0;

int main() {

#ifdef DEBUG
    cout << "DEBUG MODE" << endl;
#endif

    // network interface
    char interface[] = "eno1";
    int socket = RawSocketConnection(interface);

    if (socket < 0) {
        cerr << "\033[0;35m ### ERROR: Could not connect to socket. \033[0m\n";
        exit(1);
    }

    int option = 0;

    while (option != 9) {

        cout << "\033[1;36m SELECT ONE OPTION" << endl;
        cout << " 0 - Send a File" << endl;
        cout << " 1 - Send Group of Files" << endl;
        cout << " 2 - Recover a File" << endl;
        cout << " 3 - Recover Group of Files" << endl;
        cout << " 4 - Change Local Directory (CD)" << endl;
        cout << " 5 - Change Server Directory (CD)" << endl;
        cout << " 6 - Print Local Working Directory (PWD)" << endl;
        cout << " 7 - Print Server Working Directory (PWD)" << endl;
        cout << " 8 - Verify Backup (MD5)" << endl;
        cout << " 9 - Quit" << endl;
        cout << " option > \033[0m";
        cin >> option;

        switch (option) {
        case SEND_FILE: {
            string fileName;
            cout << "\033[1;36m file name > \033[0m";
            cin >> fileName;

            sendOneFile(socket, fileName, msgCounter);
            break;
        }
        case 1: {
            cout << "\033[1;36m patterns > \033[0m";

            string filesPattern;
            // user input patterns from the console, "*.txt" for example
            getline(cin >> ws, filesPattern);

            sendGroupOfFiles(socket, filesPattern, msgCounter);
            break;
        }
        case 2: {
            string fileName;
            cout << "\033[1;36m file name > \033[0m";
            cin >> fileName;
            restoreOneFile(socket, interface, fileName, msgCounter);
            break;
        }
        case 3: {
            cout << "\033[1;36m patterns > \033[0m";

            string filesPattern;
            // user input patterns from the console, "*.txt" for example
            getline(cin >> ws, filesPattern);

            restoreGroupOfFiles(socket, interface, filesPattern, msgCounter);
            break;
        }
        case 4: {
            string dirName;
            cout << "\033[1;36m directory name > \033[0m";
            cin >> dirName;

            changeDirectory(dirName);
            break;
        }
        case 5: {
            string dirName;
            cout << "\033[1;36m directory name > \033[0m";
            cin >> dirName;

            sendServerDirectory(socket, dirName, msgCounter);
            break;
        }
        case 6: {
            cout << "\033[1;36m Local Working Directory: \033[0m";
            cout << getCurrentDirectory() << endl;
            break;
        }
        case 7: {
            getServerWorkingDirectory(socket, msgCounter);
            break;
        }
        case 8: {
            string fileName;
            cout << "\033[1;36m file name > \033[0m";
            cin >> fileName;

            verifyBackup(socket, fileName, msgCounter);
            break;
        }
        case EXIT:
            exit(0);
            break;
        }
    }

    return 0;
}
