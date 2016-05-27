// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "../config.h"
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
void 
_ch_close_async_cb(uv_async_t* handle)
//
//    Internal callback to close chirp. Makes ch_chirp_close_ts thread-safe
//
// .. code-block:: cpp
//
{
    ch_chirp_t* chirp = (ch_chirp_t*) handle->data;
    uv_close((uv_handle_t*) &chirp->_serverv4, NULL);
    uv_close((uv_handle_t*) &chirp->_serverv6, NULL);
    uv_close((uv_handle_t*) &chirp->_close, NULL);
    if(chirp->_auto_start) {
        uv_stop(chirp->loop);
        L(chirp, "UV-Loop %p stopped by c4irp", chirp->loop);
    }
    L(chirp, "Closed chirp %p", chirp);
}

// .. c:function::
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t config, uv_loop_t* loop)
//    :noindex:
//
//    see: :c:func:`ch_chirp_init`
//
// .. code-block:: cpp
//
{
    ch_error_t           tmp_err;
    ch_text_address_t    tmp_addr;
    chirp->loop        = loop;
    chirp->config      = config;
    chirp->_auto_start = 0;
    chirp->_log        = NULL;

    // IPv4
    if(config.PORT < -1 || config.PORT > ((1<<16) - 1)) {
        return CH_VALUE_ERROR;
    }
    uv_tcp_init(chirp->loop, &chirp->_serverv4);
    if(uv_inet_ntop(
            AF_INET, config.BIND_V4, tmp_addr.data, sizeof(ch_text_address_t)
    ) < 0) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip4_addr(tmp_addr.data, config.PORT, &chirp->_addrv4) < 0) {
        return CH_VALUE_ERROR; // NOCOV uv will just wrap bad port
    }
    tmp_err = _ch_uv_error_map(uv_tcp_bind(
            &chirp->_serverv4,
            (const struct sockaddr*)&chirp->_addrv4,
            0
    ));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    if(uv_tcp_nodelay(&chirp->_serverv4, 1) < 0) {
        return CH_UV_ERROR;
    }
    if(uv_listen(
            (uv_stream_t*) &chirp->_serverv4,
            config.BACKLOG,
            _ch_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE;
    }

    // IPv6, as the dual stack feature doesn't work everywhere we bind both
    uv_tcp_init(chirp->loop, &chirp->_serverv6);
    if(uv_inet_ntop(
            AF_INET6, config.BIND_V6, tmp_addr.data, sizeof(ch_text_address_t)
    ) < 0) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip6_addr(tmp_addr.data, config.PORT, &chirp->_addrv6) < 0) {
        return CH_VALUE_ERROR;
    }
    tmp_err = _ch_uv_error_map(uv_tcp_bind(
            &chirp->_serverv6,
            (const struct sockaddr*) &chirp->_addrv6,
            UV_TCP_IPV6ONLY
    ));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    if(uv_tcp_nodelay(&chirp->_serverv6, 1) < 0) {
        return CH_UV_ERROR;
    }
    if(uv_listen(
            (uv_stream_t*) &chirp->_serverv6,
            config.BACKLOG,
            _ch_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE;
    }
    if(uv_async_init(chirp->loop, &chirp->_close, &_ch_close_async_cb) < 0) {
        return CH_UV_ERROR;
    }
    chirp->_init = CH_CHIRP_MAGIC;
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_run(ch_config_t config, ch_chirp_t** chirp_out)
//    :noindex:
//
//    TODO testme
//
//    see: :c:func:`ch_chirp_run`
//
// .. code-block:: cpp
//
{
    ch_chirp_t chirp;
    uv_loop_t  loop;
    ch_error_t tmp_err;
    if(chirp_out != NULL) {
        *chirp_out = &chirp;
    }

    tmp_err = _ch_uv_error_map(ch_loop_init(&loop));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    tmp_err = ch_chirp_init(&chirp, config, &loop);
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    chirp._auto_start = 1;
    L((&chirp), "UV-Loop %p run by c4irp", &loop);
    tmp_err = _ch_uv_error_map(ch_run(&loop, UV_RUN_DEFAULT));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    if(ch_loop_close(chirp.loop)) {
        return CH_UV_ERROR; // NOCOV
    }
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_close_ts(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close`
//
//    This function is thread-safe
//
// .. code-block:: cpp
//
{
    A(chirp->_init == CH_CHIRP_MAGIC, "Chirp not initialized");
    chirp->_close.data = chirp;
    if(uv_async_send(&chirp->_close) < 0) {
        return CH_UV_ERROR;
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
