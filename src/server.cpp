#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "ConexaoRawSocket.h"
#include "message.h"

using namespace std;

int msg_counter = 0;


void write_to_file(const char* filename, unsigned char data[MAX_DATA_SIZE], bool append) {
    ofstream file;

    if (append) {
        file.open(filename, ios::app);
    } else {
        file.open(filename);
    }

    if (file.is_open()) {
        file << data;
        file.close();

    } else {
        cout << "\033[0;31mError opening the file.\033[0m" << std::endl;
    }
}

int main()
{
    // interface de rede eno1
    char sock[] = "lo";
    int socket = ConexaoRawSocket(sock);

    if (socket < 0)
    {
        perror("\033[0;31m ### ERRO: Falha ao criar conexão raw socket. \033[0m\n");
        exit(1);
    }

    message_t message;

    while (1)
    {
        recv(socket, &message, MAX_SIZE, 0); // receive the message from the client
        if (message.init_marker == INIT_MARKER)
        { // check if the message is valid
            if (message.sequence == msg_counter && message.type != END_FILE)
            {
                write_to_file("output.txt", message.data, true);

                msg_counter++;
                if (message.type == END_FILE)
                {
                    msg_counter = 0;
                    cout << "END OF FILE" << endl;
                }
            }
        }
    }

    return 0;
}
