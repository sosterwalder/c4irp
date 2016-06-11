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
#include <stdlib.h>

SGLIB_DEFINE_RBTREE_FUNCTIONS( // NOCOV
    ch_receipt_t,
    left,
    right,
    color_field,
    CH_RECEIPT_CMP
);

// .. c:function::
ch_error_t
ch_pr_start(ch_protocol_t* protocol)
//    :noindex:
//
//    see: :c:func:`ch_pr_start`
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
    protocol->receipts = NULL;
    protocol->late_receipts = NULL;
    /*ch_receipt_t* t;
    struct sglib_ch_receipt_t_iterator it;
    for(int i = 0; i < 10; ++i) {
        t = malloc(sizeof(ch_receipt_t));
        mbedtls_ctr_drbg_random(
            protocol->rng,
            t->receipt,
            16
        );
        sglib_ch_receipt_t_add(&protocol->receipts, t);
    }
    for(
            t = sglib_ch_receipt_t_it_init_inorder(
                &it,
                protocol->receipts
            );
            t != NULL;
            t = sglib_ch_receipt_t_it_next(&it)
    ) {
        printf("%d ", t->receipt[0]);
    } */
    return CH_SUCCESS;
}
// .. c:function::
ch_error_t
ch_pr_stop(ch_protocol_t* protocol)
//    :noindex:
//
//    see: :c:func:`ch_pr_stop`
//
// .. code-block:: cpp
//
{
    uv_close((uv_handle_t*) &protocol->serverv4, NULL);
    uv_close((uv_handle_t*) &protocol->serverv6, NULL);
    _ch_pr_free_receipts(protocol->receipts);
    _ch_pr_free_receipts(protocol->late_receipts);
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
    /*if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }*/
}
// .. c:function::
static void
_ch_pr_free_receipts(ch_receipt_t* receipts)
//    :noindex:
//
//    see: :c:func:`_ch_pr_free_receipts`
//
// .. code-block:: cpp
//
{
    ch_receipt_t* t;
    struct sglib_ch_receipt_t_iterator it;
    for(
            t = sglib_ch_receipt_t_it_init(
                &it,
                receipts
            );
            t != NULL;
            t = sglib_ch_receipt_t_it_next(&it)
    ) {
        free(t);
    }
}
