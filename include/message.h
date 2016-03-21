// ===========
// Message API
// ===========
// 
// .. code-block:: cpp

#ifndef ch_inc_message_h
#define ch_inc_message_h

#include "error.h"

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
    int16_t  actor_len;
    int32_t  data_len;
    char*    actor;
    char*    data;
} ch_message_t;

// .. c:function::
extern
ch_error_t
ch_msg_init(ch_message_t* message);
//
//    Intialiaze a message. Memory provided by caller (for performance).
//
//    :param ch_message_t* message: Pointer to the message
//
// .. code-block:: cpp

#endif //ch_inc_message_h
