#include <stdio.h>
#include <stdlib.h>
#include "ConexaoRawSocket.h"

int main()
{
    int socket = ConexaoRawSocket("lo");

    send(socket, "a", BUFSIZ, 0);

    return 0;
}
