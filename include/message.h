#ifndef ch_message_h
#define ch_message_h

#include <stdint.h>

typedef struct {
    char     address[16];
    int32_t  port;
    char     identity[16];
    char     serial[16];
    char     host_order;
    char*    actor;
    char*    data;
}
ch_message_t;

extern
void
ch_msg_init(ch_message_t* message);

#endif //ch_message_h
