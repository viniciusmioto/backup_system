#include "message.h"

using namespace std;

int msg_counter = 0;

int main()
{
    // network interface eno1 (two computers)
    char sock[] = "lo";
    int socket = ConexaoRawSocket(sock);

    if (socket < 0)
    {
        perror("\033[0;31m ### ERRO: Could not connect to socket. \033[0m\n");
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
