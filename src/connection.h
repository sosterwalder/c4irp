// =================
// Connection Header
// =================
//
// .. code-block:: cpp
//
#ifndef ch_connection_h
#define ch_connection_h

// ================
// Project includes
// ================
#include "../include/message.h"

// ===============
// System includes
// ===============
#include <openssl/bio.h>
#include "sglib.h"

// .. c:type:: ch_cn_flags_t
//
//    Represents connection flags.
//
//    .. c:member:: CH_CN_BUF_USED
//
//       The connections buffer is currently used by libuv
//
//    .. c:member:: CH_CN_SHUTTING_DOWN
//
//       The connection is shutting down
//
// .. code-block:: cpp
//
typedef enum {
    CH_CN_BUF_USED       = 1 << 0,
    CH_CN_SHUTTING_DOWN  = 1 << 1,
} ch_cn_flags_t;


struct ch_chirp_s;

// .. c:type:: ch_connection_t
//
//    Connection dictionary implemented as rbtree.
//
//    .. c:member:: uint8_t[16] address
//
//       IPv4/6 address of the sender if the message was received.  IPv4/6
//       address of the recipient if the message is going to be sent.
//
// TODO Complete
//
// .. code-block:: cpp

typedef struct ch_connection_s {
    uint8_t                 ip_protocol;
    uint8_t                 address[16];
    int32_t                 port;
    uv_tcp_t                client;
    void*                   buffer;
    size_t                  buffer_size;
    struct ch_chirp_s*      chirp;
    uv_shutdown_t           shutdown_req;
    uv_timer_t              shutdown_timeout;
    int8_t                  shutdown_tasks;
    uint8_t                 flags;
    BIO*                    bio_ssl;
    BIO*                    bio_app;
    char                    color_field;
    struct ch_connection_s* left;
    struct ch_connection_s* right;
} ch_connection_t;

// Sglib Prototypes
// ================
//
// .. code-block:: cpp
//
#define CH_CONNECTION_CMP(x,y) ch_connection_cmp(x, y)

SGLIB_DEFINE_RBTREE_PROTOTYPES(
    ch_connection_t,
    left,
    right,
    color_field,
    CH_CONNECTION_CMP
);

// .. c:function::
void
ch_cn_read_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
//
//    Allocates a buffer on the connection and reuses it for each subsequent
//    reads.
//
//    :param uv_handle_t* handle: The libuv handle holding the
//                                connection
//    :param size_ti suggested_size: The size of the connection buffer
//                                   in bit (unsigned integer, at
//                                   least 16 bit). This is used if
//                                   the buffer needs to be allocated
//    :param uv_buf_t* buf: Libuv buffer which will hold the
//                          connection

// .. c:function::
ch_error_t
ch_cn_shutdown(ch_connection_t* conn);
//
//    Shutdown this connection.
//
//    :param ch_connection_t* conn: Connection dictionary holding a
//                                  chirp instance.
//    :return: A chirp error. see: :c:type:`ch_error_t`
//    :rtype: ch_error_t

// .. c:function::
static
ch_inline
int
ch_connection_cmp(ch_connection_t* x, ch_connection_t* y)
//
//    Compare operator for connections.
//
//    :param ch_connection_t* x: First connection instance to compare
//    :param ch_connection_t* y: Second connection instance to compare
//    :return: the comparision between
//                 - the IP protocols, if they are not the same, or
//                 - the addresses, if they are not the same, or
//                 - the ports
//    :rtype: int
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

// .. c:function::
static
ch_inline
void
ch_connection_init(struct ch_chirp_s* chirp, ch_connection_t* conn)
//
//    Initialize a connection.
//
//    :param ch_chirp_t* chirp: Chirp instance
//    :param ch_connection_t* conn: Connection to initialize
//
// .. code-block:: cpp
//
{
    memset(conn, 0, sizeof(ch_connection_t));
    conn->chirp = chirp;
}

#endif //ch_connection_h
