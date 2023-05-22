#include <stdio.h>
#include <stdlib.h>
#include "ConexaoRawSocket.h"
#include "message.h"

int msg_counter = 0;

int main()
{
    // interface de rede eno1 (dois computadores)
    int socket = ConexaoRawSocket("lo");
    if (socket < 0)
    {
        perror("\033[0;31m ### ERRO: Falha ao criar conexão raw socket. \033[0m\n");
        exit(1);
    }

    unsigned char buffer[67];

    message_t message;
    message.init_marker = 126;

    while (msg_counter < 10)
    {
        strcpy(message.data, "hello world");
        message.sequence = msg_counter++;
        printf("sequence: %d\n", message.sequence);
        memcpy(buffer, &message, 67);
        send(socket, buffer, 67, 0);
    }

    return 0;
}
