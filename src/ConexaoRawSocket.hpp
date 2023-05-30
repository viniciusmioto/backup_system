#ifndef RAW_SOCKET_H
#define RAW_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;

/*!
    * \brief Create a raw socket connection
    * \param device
    * \return int with socket descriptor
*/
int ConexaoRawSocket(const char *device);

#endif