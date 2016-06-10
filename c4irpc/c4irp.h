// ============
// Chirp Header
// ============
//
// .. code-block:: cpp

#ifndef ch_c4irp_h
#define ch_c4irp_h

#include "../include/c4irp_obj.h"
#include "protocol.h"

#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>

// .. c:type:: ch_chirp_int_t
//
//    Chirp object.
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
//    .. c:member:: uv_async_t close
//
//       async handler to close c4irp on the main-loop
//
//    .. c:member:: int auto_start
//
//       true if we have to close the libuv loop, otherwise the loop was
//       supplied by the user
//
//    .. c:member:: mbedtls_entropy_context entropy
//
//       our entropy source provided by mbedtls
//
//    .. c:member:: mbedtls_ctr_drbg_context rng
//
//       non-cryptographic random number generator
//
// .. code-block:: cpp
//
struct ch_chirp_int {
    struct sockaddr_in       addrv4;
    struct sockaddr_in6      addrv6;
    uv_tcp_t                 serverv4;
    uv_tcp_t                 serverv6;
    uv_async_t               close;
    int                      auto_start;
    mbedtls_entropy_context  entropy;
    mbedtls_ctr_drbg_context rng;
    ch_protocol_t            protocol;
};

// .. c:function::
static void
_ch_on_new_connection(uv_stream_t *server, int status);
//
//  Callback from libuv on new connection
//
// .. code-block:: cpp

#endif //ch_c4irp_h
