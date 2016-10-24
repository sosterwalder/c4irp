// =============
// Reader Header
// =============
//
// .. code-block:: cpp

#ifndef ch_reader_h
#define ch_reader_h

#include <stdlib.h>

struct ch_connection_s;

// .. c:type:: ch_reader_t
//
//    Contains the state of the reader
//
//    .. c:member:: state
//
//    Statemachine-state of the reader
//
// .. code-block:: cpp

typedef struct ch_reader_s {
  unsigned char state;
} ch_reader_t;

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

// .. c:function::
void
ch_rd_read(struct ch_connection_s* conn, char* buf, size_t read);
//
//    Implements the wire protocol reader part.
//
//    :param ch_connection_t* conn: Connection the data was read from
//    :param char* buf: The that read
//    :param size_t read: Count of bytes read
//
// .. code-block:: cpp

#endif //ch_reader_h
