// =================
// Connection Header
// =================
//
// .. code-block:: cpp

#ifndef ch_connection_h
#define ch_connection_h

#include "../include/message.h"

#include "sglib.h"

// .. c:type:: ch_connection_t
//
//    Connection dictionary implemented as rbtree.
//
//    .. c:member:: uint8_t[16] address
//
//       IPv4/6 address of the sender if the message was received.  IPv4/6
//       address of the recipient if the message is going to be sent.
//
// .. code-block:: cpp

typedef struct ch_connection {
    uint8_t               ip_protocol;
    uint8_t               address[16];
    int32_t               port;
    uv_tcp_t              client;
    char                  color_field;
    struct ch_connection* left;
    struct ch_connection* right;
} ch_connection_t;

static
ch_inline
int
ch_connection_cmp(ch_connection_t* x, ch_connection_t* y);

// .. c:function::
static
ch_inline
int
ch_connection_cmp(ch_connection_t* x, ch_connection_t* y)
//
//    Compare operator for connections
//
//    :param ch_connection_t* x: x
//    :param ch_connection_t* y: y
//
// .. code-block:: cpp
//
{
    if(x->ip_protocol != y->ip_protocol) {
        return x->ip_protocol - y->ip_protocol;
    } else {
        int tmp_cmp = memcmp(x->address, y->address, 16);
        if(tmp_cmp != 0) {
            return tmp_cmp;
        } else {
            return x->port - y->port;
        }
    }
}
#define CH_CONNECTION_CMP(x,y) ch_connection_cmp(x, y)

SGLIB_DEFINE_RBTREE_PROTOTYPES(
    ch_connection_t,
    left,
    right,
    color_field,
    CH_CONNECTION_CMP
);

#endif //ch_connection_h
