typedef struct message {
    unsigned int init_marker :8;
    unsigned int size :6;
    unsigned int sequence :6;
    unsigned int type: 4;
    unsigned char data[63];
    unsigned int parity :8;
} message_t;
