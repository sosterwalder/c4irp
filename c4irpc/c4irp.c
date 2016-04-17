// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "common.h"
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
    .BACKLOG    = 100,
    .BIND_V6    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4    = {0, 0, 0, 0},
};

// .. c:function::
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t config, uv_loop_t loop)
//    :noindex:
//
//    see: :c:func:`ch_chirp_init`
//
// .. code-block:: cpp
//
{
    ch_error_t        tmp_err;
    ch_text_address_t tmp_addr;
    chirp->loop   = loop;
    chirp->config = config;


    // IPv4
    uv_tcp_init(&chirp->loop, &chirp->serverv4);
    if(uv_inet_ntop(
            AF_INET, config.BIND_V4, tmp_addr.data, sizeof(ch_text_address_t)
    ) < 0) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip4_addr(tmp_addr.data, config.PORT, &chirp->addrv4) < 0) {
        return CH_VALUE_ERROR;
    }
    tmp_err = _ch_uv_error_map(uv_tcp_bind(
            &chirp->serverv4,
            (const struct sockaddr*)&chirp->addrv4,
            0
    ));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    if(uv_tcp_nodelay(&chirp->serverv4, 1) < 0) {
        return CH_UV_ERROR;
    }
    if(uv_listen(
            (uv_stream_t*) &chirp->serverv4,
            config.BACKLOG,
            _ch_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE;
    }

    // IPv6, as the dual stack feature doesn't work everywhere we bind both
    uv_tcp_init(&chirp->loop, &chirp->serverv6);
    if(uv_inet_ntop(
            AF_INET6, config.BIND_V6, tmp_addr.data, sizeof(ch_text_address_t)
    ) < 0) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip6_addr(tmp_addr.data, config.PORT, &chirp->addrv6) < 0) {
        return CH_VALUE_ERROR;
    }
    tmp_err = _ch_uv_error_map(uv_tcp_bind(
            &chirp->serverv6,
            (const struct sockaddr*) &chirp->addrv6,
            UV_TCP_IPV6ONLY
    ));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    if(uv_tcp_nodelay(&chirp->serverv6, 1) < 0) {
        return CH_UV_ERROR;
    }
    if(uv_listen(
            (uv_stream_t*) &chirp->serverv6,
            config.BACKLOG,
            _ch_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE;
    }
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_close(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close`
//
// .. code-block:: cpp
//
{
    if(uv_loop_close(&chirp->loop)) {
        return CH_UV_ERROR; // NOCOV
    }
    return CH_SUCCESS;
}

// .. c:function::
void
_ch_on_new_connection(uv_stream_t *server, int status)
//
//  Callback from libuv on new connection
//
// .. code-block:: cpp
//
{
    return;
}
