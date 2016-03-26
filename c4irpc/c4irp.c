// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "c4irp.h"
#include "message.h"

#include <stdlib.h>
#include <uv.h>

//
// .. c:var:: ch_config_defaults
//
//    Default config of c4irp.
//
// .. code-block:: cpp
//
ch_config_t ch_config_defaults = {
    .REUSE_TIME = 30,
    .TIMEOUT    = 5,
    .PORT       = 2998,
    .BIND_V6    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4    = {0, 0, 0, 0},
};

// .. c:function::
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t config)
//    :noindex:
//
//    see: :c:func:`ch_chirp_init`
//
// .. code-block:: cpp
//
{
    ch_text_address_t tmp_addr;
    uv_loop_init(&chirp->loop);

    struct sockaddr_in addrv4;
    struct sockaddr_in6 addrv6;
    uv_tcp_t serverv4;
    uv_tcp_t serverv6;

    // IPv4
    if(uv_inet_ntop(
            AF_INET, config.BIND_V4, tmp_addr.data, sizeof(ch_text_address_t)
    )) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip4_addr(tmp_addr.data, config.PORT, &addrv4)) {
        return CH_VALUE_ERROR;
    }
    if(uv_tcp_bind(
            &serverv4,
            (const struct sockaddr*)&addrv4,
            0
    ) == UV_EADDRINUSE) {
        return CH_EADDRINUSE;
    }

    // IPv6, as the dual stack feature doesn't work everywhere we bind both
    if(uv_inet_ntop(
            AF_INET6, config.BIND_V6, tmp_addr.data, sizeof(ch_text_address_t)
    )) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip6_addr(tmp_addr.data, config.PORT, &addrv6)) {
        return CH_VALUE_ERROR;
    }
    if(uv_tcp_bind(
            &serverv6,
            (const struct sockaddr*)&addrv6,
            UV_TCP_IPV6ONLY
    ) == UV_EADDRINUSE) {
        return CH_EADDRINUSE;
    }
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_close(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_free`
//
// .. code-block:: cpp
//
{
    if(uv_loop_close(&chirp->loop)) {
        return CH_UV_ERROR; // NOCOV
    }
    return CH_SUCCESS;
}
