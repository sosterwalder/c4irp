// ============
// Chirp Header
// ============
//
// .. code-block:: cpp

#ifndef ch_c4irp_h
#define ch_c4irp_h

#include "../include/c4irp_obj.h"

struct ch_chirp_int {
    struct sockaddr_in  addrv4;
    struct sockaddr_in6 addrv6;
    uv_tcp_t            serverv4;
    uv_tcp_t            serverv6;
    uv_async_t          close;
    int                 auto_start;
    int                 init;
};

static void
_ch_on_new_connection(uv_stream_t *server, int status);

#endif //ch_c4irp_h
