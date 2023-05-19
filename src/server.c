#include <stdio.h>
#include <stdlib.h>
#include "ConexaoRawSocket.h"

int main()
{
    int socket = ConexaoRawSocket("lo");
    
    char buffer[BUFSIZ];

    recv(socket, buffer, BUFSIZ, 0);

    printf("%s\n", buffer);

    return 0;
}
