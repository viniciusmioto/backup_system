#define MAX_SIZE 67
#define MAX_DATA_SIZE 63
#define INIT_MARKER 126

typedef enum {
    BACKUP_1_ARQ = 0,
    END_FILE = 9,
    EXIT_CLIENT = 11,
    ERRO = 12,
    OK = 13,
    ACK = 14,
    NACK = 15,
} message_type_t;

typedef enum {
    SEND_FILE = 0,
    EXIT = 9,
} client_options_t;

typedef struct message {
    unsigned int init_marker :8;
    unsigned int size :6;
    unsigned int sequence :6;
    unsigned int type: 4;
    unsigned char data[63];
    unsigned int parity :8;
} message_t;
