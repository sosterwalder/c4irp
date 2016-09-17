// ===============
// Protocol Header
// ===============
//
// .. code-block:: cpp

#ifndef ch_protocol_h
#define ch_protocol_h

#include "../include/chirp_obj.h"
#include "connection.h"
#include "sglib.h"

// .. c:type:: ch_receipt_t
//
//    Receipt set implemented as rbtree.
//
//    .. c:member:: unsigned char receipt[16]
//
//    the receipt generated using TODO
//
// .. code-block:: cpp

typedef struct ch_receipt {
  unsigned char      receipt[16];
  char               color_field;
  struct ch_receipt* left;
  struct ch_receipt* right;
} ch_receipt_t;

#define CH_RECEIPT_CMP(x,y) \
    memcmp(x->receipt, y->receipt, 16)

SGLIB_DEFINE_RBTREE_PROTOTYPES(
    ch_receipt_t,
    left,
    right,
    color_field,
    CH_RECEIPT_CMP
);

// .. c:type:: ch_protocol_t
//
//    Protocol object.
//
//    .. c:member:: struct sockaddr_in addrv4
//
//       BIND_V4 addr converted to a sockaddr_in
//
//    .. c:member:: struct sockaddr_in addrv4
//
//       BIND_V6 addr converted to a sockaddr_in6
//
//    .. c:member:: uv_tcp_t serverv4
//
//       reference to the libuv tcp server
//
//    .. c:member:: uv_tcp_t serverv6
//
//       reference to the libuv tcp server
//
//       TODO Complete
//
// .. code-block:: cpp

struct  ch_chirp;

typedef struct ch_protocol {
    struct sockaddr_in        addrv4;
    struct sockaddr_in6       addrv6;
    uv_tcp_t                  serverv4;
    uv_tcp_t                  serverv6;
    ch_connection_t*          connections;
    ch_receipt_t*             receipts;
    ch_receipt_t*             late_receipts;
    struct ch_chirp*          chirp;
} ch_protocol_t;

// .. c:function::
ch_error_t
ch_pr_start(ch_protocol_t* protocol);
//
//    Start the protocol
//
//    TODO params
//
// .. c:function::
ch_error_t
ch_pr_stop(ch_protocol_t* protocol);
//
//    Stop the protocol
//
//    TODO params
//
// .. c:function::
static void
_ch_on_new_connection(uv_stream_t *server, int status);
//
//    Callback from libuv on new connection
//
//    TODO params
//
// .. c:function::
static void
_ch_pr_free_receipts(ch_chirp_t* chirp, ch_receipt_t* receipts);
//
//    Free all remaining items in a receipts set
//
//    TODO params
//
// .. c:function::
static void
_ch_pr_close_free_connections(ch_chirp_t* chirp, ch_connection_t* connections);
//
//    Close and free all remaining connections
//
//    TODO params
//
// .. c:function::
static void
_ch_pr_on_new_connection(uv_stream_t *server, int status);
//
//  Callback from libuv on new connection
//
//    TODO params
//
// .. c:function::
static void
_ch_pr_on_read_data(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
//
//  Callback from libuv when data was read
//
//    TODO params
//
// .. code-block:: cpp

#endif //ch_protocol_h
