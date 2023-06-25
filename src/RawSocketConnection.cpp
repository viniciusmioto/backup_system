#include "RawSocketConnection.hpp"

int RawSocketConnection(const char *device)
{
    int interface;
    struct ifreq ir;
    struct sockaddr_ll address;
    struct packet_mreq mr;

    interface = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); /*cria socket*/
    if (interface == -1)
    {
        cout << "\033[0;35mErro no Socket\033[0m" << endl;
        exit(-1);
    }

    memset(&ir, 0, sizeof(struct ifreq)); /*dispositivo eth0*/
    memcpy(ir.ifr_name, device, sizeof(device));
    if (ioctl(interface, SIOCGIFINDEX, &ir) == -1)
    {
        cout << "\033[0;35mErro no ioctl\033[0m" << endl;
        exit(-1);
    }

    memset(&address, 0, sizeof(address)); /*IP do dispositivo*/
    address.sll_family = AF_PACKET;
    address.sll_protocol = htons(ETH_P_ALL);
    address.sll_ifindex = ir.ifr_ifindex;
    if (bind(interface, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        cout << "\033[0;35mErro no bind\033[0m" << endl;
        exit(-1);
    }

    memset(&mr, 0, sizeof(mr)); /*Modo Promiscuo*/
    mr.mr_ifindex = ir.ifr_ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(interface, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1)
    {
        cout << "\033[0;35mErro ao fazer setsockopt\033[0m" << endl;
        exit(-1);
    }

    return interface;
}
