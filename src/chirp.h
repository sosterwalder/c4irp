// ============
// Chirp Header
// ============
//
// .. code-block:: cpp

#ifndef ch_chirp_h
#define ch_chirp_h

#include "../include/chirp_obj.h"
#include "protocol.h"

// .. c:type:: ch_chirp_int_t
//
//    Chirp object.
//
//
//    .. c:member:: uv_async_t close
//
//       async handler to close chirp on the main-loop
//
//    .. c:member:: int auto_start
//
//       true if we have to close the libuv loop, otherwise the loop was
//       supplied by the user
//
// .. code-block:: cpp
//
struct ch_chirp_int {
    int                      auto_start;
    uv_async_t               close;
    ch_protocol_t            protocol;
};

#endif //ch_chirp_h
