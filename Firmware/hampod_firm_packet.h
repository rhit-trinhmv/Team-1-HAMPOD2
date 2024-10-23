#ifndef HAMPOD_PACKET
#define HAMPOD_PACKET

typedef enum {
    KEYPAD,
    AUDIO,
    SERIAL,
    CONFIG
} Packet_type;

typedef struct Inst_packet {
    Packet_type type;
    unsigned short data_len;
    unsigned short tag;
    unsigned char *data;
} Inst_packet;

Inst_packet* create_inst_packet(Packet_type new_type, unsigned short new_len, unsigned char *new_data, unsigned short tag);
void destroy_inst_packet(Inst_packet** packet);
#ifndef SHAREDLIB
#include "hampod_firm_packet.c"
#endif
#endif
