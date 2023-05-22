#include <stdio.h>
#include <stdlib.h>
#include "ConexaoRawSocket.h"
#include "message.h"

int msg_counter = 0;

int main()
{
    // interface de rede eno1
    int socket = ConexaoRawSocket("lo");
    if (socket < 0)
    {
        perror("\033[0;31m ### ERRO: Falha ao criar conexão raw socket. \033[0m\n");
        exit(1);
    }

    message_t message;
    while(1) {
        recv(socket, &message, 67, 0);
        if(message.init_marker == 126)
            if (message.sequence == msg_counter)
            {
                printf("%s\n", message.data);
                msg_counter++;
            }
    }

    return 0;
}
