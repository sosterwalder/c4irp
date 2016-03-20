// ===========
// Message API
// ===========
// 
// .. code-block:: cpp

#ifndef ch_message_h
#define ch_message_h

#include <stdint.h>

// .. c:type:: ch_message_t
//
//    Represents a message.
//  
//    .. c:member:: char[16] address
//
//       IPv4/6 address of the sender if the message was received.  IPv4/6
//       address of the recipient if the message is going to be sent.
//
// .. code-block:: cpp

typedef struct {
    char     address[16];
    int32_t  port;
    char     identity[16];
    char     serial[16];
    int8_t   host_order;
    char*    actor;
    char*    data;
}
ch_message_t;

// .. c:function::
extern
void
ch_msg_init(ch_message_t* message);
//
//    Intialiaze a message.
//
//    :param ch_message_t* message: Pointer to the message
//
// .. code-block:: cpp

#endif //ch_message_h
