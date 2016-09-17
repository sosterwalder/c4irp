// ========
// Protocol
// ========
//
// .. code-block:: cpp
//
#include "protocol.h"
#include "chirp.h"
#include "util.h"

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
    int                   tmp_err;
    ch_text_address_t     tmp_addr;
    ch_chirp_t*           chirp;
    chirp               = protocol->chirp;
    ch_config_t* config = chirp->config;
    // IPv4
    if(config->PORT < -1 || config->PORT > ((1<<16) - 1)) {
        return CH_VALUE_ERROR;
    }
    uv_tcp_init(chirp->loop, &protocol->serverv4);
    protocol->serverv4.data = chirp;
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
        return tmp_err;  // NOCOV UV_EADDRINUSE can happen in tcp_bind or
                         // listen on my systems it happends in listen
    }
    if(uv_tcp_nodelay(&protocol->serverv4, 1) < 0) {
        return CH_UV_ERROR;  // NOCOV don't know how to produce
    }
    if(uv_listen(
            (uv_stream_t*) &protocol->serverv4,
            config->BACKLOG,
            _ch_pr_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE;
    }

    // IPv6, as the dual stack feature doesn't work everywhere we bind both
    uv_tcp_init(chirp->loop, &protocol->serverv6);
    protocol->serverv6.data = chirp;
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
            _ch_pr_on_new_connection
    ) < 0) {
        return CH_EADDRINUSE; // NOCOV errors happend for IPV4
    }
    protocol->receipts = NULL;
    protocol->late_receipts = NULL;
    protocol->connections = NULL;
    /*ch_receipt_t* t;
    struct sglib_ch_receipt_t_iterator it;
    for(int i = 0; i < 10; ++i) {
        t = ch_chirp_alloc(chirp, sizeof(ch_receipt_t));
        // TODO fill t->receipt
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
    ch_chirp_t* chirp = protocol->chirp;
    L(chirp, "Closing protocol %p", chirp);
    _ch_pr_close_free_connections(chirp, protocol->connections);
    uv_close((uv_handle_t*) &protocol->serverv4, NULL);
    uv_close((uv_handle_t*) &protocol->serverv6, NULL);
    _ch_pr_free_receipts(chirp, protocol->receipts);
    _ch_pr_free_receipts(chirp, protocol->late_receipts);
    return CH_SUCCESS;
}

// .. c:function::
static void
_ch_pr_on_new_connection(uv_stream_t* server, int status) // NOCOV TODO
//    :noindex:
//
//    see: :c:func:`_ch_pr_on_new_connection`
//
// .. code-block:: cpp
//
{
    CH_GET_CHIRP(server); // NOCOV TODO
    ch_protocol_t* protocol = &chirp->_->protocol;
    if (status < 0) { // NOCOV TODO
        L(chirp, "New connection error %s", uv_strerror(status)); // NOCOV TODO
        return; // NOCOV TODO
    }

    ch_connection_t* conn = (ch_connection_t*) ch_chirp_alloc( // NOCOV TODO
        chirp,
        sizeof(ch_connection_t)
    ); // NOCOV TODO
    ch_connection_init(chirp, conn);
    uv_tcp_t* client = &conn->client; // NOCOV TODO
    uv_tcp_init(server->loop, client); // NOCOV TODO
    client->data = conn;
    if (uv_accept(server, (uv_stream_t*) client) == 0) { // NOCOV TODO
        L(chirp, "Accepted connection %p", conn);
        sglib_ch_connection_t_add(&protocol->connections, conn);
        uv_read_start(
            (uv_stream_t*) client,
            ch_cn_read_alloc,
            _ch_pr_on_read_data
        ); // NOCOV TODO
    } // NOCOV TODO
    else {
        // TODO uv_close on cleanup and, on close and on remove close
        uv_close((uv_handle_t*) client, NULL); // NOCOV TODO
        ch_chirp_free(chirp, conn); // NOCOV TODO
    }
} // NOCOV TODO remove
//
static void
_ch_pr_on_read_data(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) // NOCOV TODO
//    :noindex:
//
//    see: :c:func:`_ch_pr_on_read_data`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = stream->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    conn->buffer_used = 0;
}
// .. c:function::
static void
_ch_pr_free_receipts(ch_chirp_t* chirp, ch_receipt_t* receipts)
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
            t = sglib_ch_receipt_t_it_next(&it) // NOCOV TODO remove
    ) {
        ch_chirp_free(chirp, t); // NOCOV TODO remove
    } // NOCOV TODO remove
}
// .. c:function::
static void
_ch_pr_close_free_connections(ch_chirp_t* chirp, ch_connection_t* connections)
//    :noindex:
//
//    see: :c:func:`_ch_pr_close_free_connections`
//
// .. code-block:: cpp
//
{
    ch_connection_t* t;
    struct sglib_ch_connection_t_iterator it;
    for(
            t = sglib_ch_connection_t_it_init(
                &it,
                connections
            );
            t != NULL;
            t = sglib_ch_connection_t_it_next(&it) // NOCOV TODO remove
    ) {
        uv_close((uv_handle_t*) &t->client, NULL); // NOCOV TODO
        ch_chirp_free(chirp, t); // NOCOV TODO remove
    } // NOCOV TODO remove
}
