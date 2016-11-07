// =============
// Reader Header
// =============
//
// .. code-block:: cpp

#ifndef ch_reader_h
#define ch_reader_h

#include "../include/common.h"

struct ch_connection_s;

// .. c:type:: ch_rd_state
//
//    Represents connection flags.
//
//    .. c:member:: CH_RD_START
//
//       Initial state, chirp handshake has to be done
//
//    .. c:member:: CH_RD_WAIT
//
//       Wait for the next message
//
//    .. c:member:: CH_RD_SIZE
//
//       Size of the next message was received
//
// .. code-block:: cpp
//
typedef enum {
    CH_RD_START = 0,
    CH_RD_WAIT  = 1,
    CH_RD_SIZE  = 2
} ch_rd_state_t;

// .. c:type:: ch_rd_handshake_t
//
//    Connection handshake data
//
//    .. c:member:: port
//
//    Public port
//
//    .. c:member:: port
//
//    Maximum timeout
//
//    .. c:member:: identity
//
//    identity
//
// .. code-block:: cpp

typedef struct ch_rd_handshake_s {
    uint16_t port;
    uint16_t max_timeout;
    unsigned char identity[16];
} ch_rd_handshake_t;

// .. c:type:: ch_reader_t
//
//    Contains the state of the reader
//
//    .. c:member:: unsinged char state
//
//    Statemachine-state of the reader
//
//    .. c:member:: ch_rd_handshake_t hs
//
//    Handshake structure to send over the network
//
// .. code-block:: cpp

typedef struct ch_reader_s {
    unsigned char state;
    ch_rd_handshake_t hs;
} ch_reader_t;


// .. c:function::
static
ch_inline
void
ch_rd_init(ch_reader_t* reader)
//
//    Initialize the reader structure
//
//    :param ch_reader_t* reader: The reader
//
// .. code-block:: cpp
//
{
    reader->state = CH_RD_START;
}

// .. c:function::
void
ch_rd_read(struct ch_connection_s* conn, void* buf, size_t read);
//
//    Implements the wire protocol reader part.
//
//    :param ch_connection_t* conn: Connection the data was read from
//    :param void* buf: The that read
//    :param size_t read: Count of bytes read
//
// .. code-block:: cpp

#endif //ch_reader_h
