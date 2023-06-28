#include "Message.hpp"

using namespace std;

Message::Message() {
    // Initialize members with default values
    memset(ethernetHeader, 0, sizeof(ethernetHeader));
    initMarker = INIT_MARKER;
    size = 0;
    sequence = 0;
    type = 0;
    memset(data, 0, sizeof(data));
    parity = 0;
}

Message::Message(unsigned int sizeValue, unsigned int sequenceValue,
                 unsigned int typeValue, const unsigned char *dataValue, unsigned int parityValue) {
    memset(ethernetHeader, 0, sizeof(ethernetHeader));
    initMarker = INIT_MARKER;
    size = sizeValue;
    sequence = sequenceValue;
    type = typeValue;
    memcpy(data, dataValue, MAX_DATA_SIZE);
    parity = parityValue;
}
