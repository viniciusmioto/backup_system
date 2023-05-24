#include "message.h"

using namespace std;

void mount_package(int *file_size, string file_name, int *file_position, string file_content, message_t &message, int *msg_counter) {
    if (*file_size > 0)
        file_content = get_file_content(file_name, *file_position, *file_size);

    memcpy(&message.data, file_content.c_str(), sizeof(message.data));

    message.sequence = *msg_counter;
    *msg_counter += 1;
    message.type = BACKUP_1_ARQ;

    *file_position += (MAX_DATA_SIZE);
    *file_size -= (MAX_DATA_SIZE);
}