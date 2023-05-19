#include <stdio.h>
#include <stdlib.h>
#include "ConexaoRawSocket.h"

int main()
{
    int socket = ConexaoRawSocket("lo");
    if (socket < 0)
    {
        perror("\033[0;31m ### ERRO: Falha ao criar conexão raw socket. \033[0m\n");
        exit(1);
    }

    char buffer[BUFSIZ];

    recv(socket, buffer, BUFSIZ, 0);

    printf("%s\n", buffer);

    return 0;
}
