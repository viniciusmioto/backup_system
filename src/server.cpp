#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include "ConexaoRawSocket.h"
#include "message.h"

using namespace std;

int msg_counter = 0;

int main() {
    // interface de rede eno1
    char sock[] = "lo";
    int socket = ConexaoRawSocket(sock);

    if (socket < 0) {
        perror("\033[0;31m ### ERRO: Falha ao criar conexão raw socket. \033[0m\n");
        exit(1);
    }

    message_t message;

    while (1) {
        recv(socket, &message, 67, 0);  // receive the message from the client
        if (message.init_marker == 126) {  // check if the message is valid
            if (message.sequence == msg_counter) {
                cout << message.data << endl;
                msg_counter++;
                if(message.type == END_FILE){
                    msg_counter = 0;
                    cout << "END OF FILE" << endl;
                }
            }
        }
    }

    return 0;
}
