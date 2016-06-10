// ========
// Protocol
// ========
//
// .. code-block:: cpp
//
#include "protocol.h"

#include "common.h"
#include "message.h"
#include "c4irp.h"

// .. c:function::
ch_error_t
ch_pr_start(ch_protocol_t* protocol)
//
//  Start the protocol
//
// .. code-block:: cpp
//
{
    int                      tmp_err;
    ch_text_address_t        tmp_addr;
    ch_config_t* config = protocol->config;
    // IPv4
    if(config->PORT < -1 || config->PORT > ((1<<16) - 1)) {
        return CH_VALUE_ERROR;
    }
    uv_tcp_init(protocol->loop, &protocol->serverv4);
    if(uv_inet_ntop(
            AF_INET, config->BIND_V4, tmp_addr.data, sizeof(ch_text_address_t)
    ) < 0) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip4_addr(tmp_addr.data, config->PORT, &protocol->addrv4) < 0) {
        return CH_VALUE_ERROR; // NOCOV uv will just wrap bad port
    }
    tmp_err = _ch_uv_error_map(uv_tcp_bind(
            &protocol->serverv4,
            (const struct sockaddr*)&protocol->addrv4,
            0
    ));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;  // NOCOV UV_EADDRINUSE can happen in tcp_bind or listen
                         // on my systems it happends in listen
    }
    if(uv_tcp_nodelay(&protocol->serverv4, 1) < 0) {
        return CH_UV_ERROR;  // NOCOV don't know how to produce
    }
    if(uv_listen(
            (uv_stream_t*) &protocol->serverv4,
            config->BACKLOG,
            _ch_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE;
    }

    // IPv6, as the dual stack feature doesn't work everywhere we bind both
    uv_tcp_init(protocol->loop, &protocol->serverv6);
    if(uv_inet_ntop(
            AF_INET6, config->BIND_V6, tmp_addr.data, sizeof(ch_text_address_t)
    ) < 0) {
        return CH_VALUE_ERROR; // NOCOV there is no bad binary IP-addr
    }
    if(uv_ip6_addr(tmp_addr.data, config->PORT, &protocol->addrv6) < 0) {
        return CH_VALUE_ERROR; // NOCOV errors happend for IPV4
    }
    tmp_err = _ch_uv_error_map(uv_tcp_bind(
            &protocol->serverv6,
            (const struct sockaddr*) &protocol->addrv6,
            UV_TCP_IPV6ONLY
    ));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err; // NOCOV errors happend for IPV4
    }
    if(uv_tcp_nodelay(&protocol->serverv6, 1) < 0) {
        return CH_UV_ERROR; // NOCOV errors happend for IPV4
    }
    if(uv_listen(
            (uv_stream_t*) &protocol->serverv6,
            config->BACKLOG,
            _ch_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE; // NOCOV errors happend for IPV4
    }
    return CH_SUCCESS;
}
// .. c:function::
ch_error_t
ch_pr_stop(ch_protocol_t* protocol)
//
//  Stop the protocol
//
// .. code-block:: cpp
//
{
    uv_close((uv_handle_t*) &protocol->serverv4, NULL);
    uv_close((uv_handle_t*) &protocol->serverv6, NULL);
    return CH_SUCCESS;
}

// .. c:function::
static void
_ch_on_new_connection(uv_stream_t *server, int status)
//    :noindex:
//
//    see: :c:func:`_ch_on_new_connection`
//
// .. code-block:: cpp
//
{
    return; // NOCOV
}
