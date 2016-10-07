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
static void
_ch_pr_close_free_connections(ch_chirp_t* chirp, ch_connection_t* connections);
//
//    Close and free all remaining connections
//
//    TODO params
//
// .. c:function::
static void
_ch_pr_free_receipts(ch_chirp_t* chirp, ch_receipt_t* receipts);
//
//    Free all remaining items in a receipts set
//
//    TODO params
//
// .. c:function::
static void
_ch_pr_new_connection_cb(uv_stream_t *server, int status);
//
//    Callback from libuv on new connection
//
//    TODO params
//
// .. c:function::
static void
_ch_pr_read_data_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
//
//  Callback from libuv when data was read
//
//    TODO params
//

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
        ch_cn_shutdown(t);
    } // NOCOV TODO remove
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
        ch_free(t); // NOCOV TODO remove
    } // NOCOV TODO remove
}

// .. c:function::
static void
_ch_pr_new_connection_cb(uv_stream_t* server, int status)
//    :noindex:
//
//    see: :c:func:`_ch_pr_new_connection_cb`
//
// .. code-block:: cpp
//
{
    CH_GET_CHIRP(server); // NOCOV TODO
    ch_protocol_t* protocol = &chirp->_->protocol;
    if (status < 0) { // NOCOV TODO
        L(
            chirp,
            "New connection error %s. ch_chirp_t:%p",
            uv_strerror(status),
            chirp
        ); // NOCOV TODO
        return; // NOCOV TODO
    }

    ch_connection_t* conn = (ch_connection_t*) ch_alloc(
        sizeof(ch_connection_t)
    );
    ch_connection_init(chirp, conn);
    uv_tcp_t* client = &conn->client;
    uv_tcp_init(server->loop, client);
    client->data = conn;
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        L(chirp, "Accepted connection. ch_connection_t:%p, ch_chirp_t:%p", conn, chirp);
        sglib_ch_connection_t_add(&protocol->connections, conn);
        uv_read_start(
            (uv_stream_t*) client,
            ch_cn_read_alloc_cb,
            _ch_pr_read_data_cb
        );
    }
    else {
        // TODO uv_close on cleanup and, on close and on remove close
        // uv_close((uv_handle_t*) client, ch_cn_close_cb);
    }
}

// .. c:function::
static void
_ch_pr_read_data_cb(
        uv_stream_t* stream,
        ssize_t nread,
        const uv_buf_t* buf
)
//    :noindex:
//
//    see: :c:func:`_ch_pr_read_data_cb`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = stream->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_protocol_t* protocol = &chirp->_->protocol;
    if(nread == UV_EOF) {
        ch_cn_shutdown(conn);
        if(sglib_ch_connection_t_is_member(protocol->connections, conn))
            sglib_ch_connection_t_delete(&protocol->connections, conn);
        else {
            L(
                chirp,
                "Error: closing unknown connection. ch_connection_t:%p "
                "ch_chirp_t:%p",
                conn,
                chirp
            );
        }
    }
    conn->flags &= ~CH_CN_BUF_USED;
    // ch_chirp_close_ts(chirp);
}

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
    int tmp_err;
    ch_text_address_t tmp_addr;
    ch_chirp_t* chirp = protocol->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    ch_config_t* config = &ichirp->config;
    // IPv4
    uv_tcp_init(ichirp->loop, &protocol->serverv4);
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
        fprintf(
            stderr,
            "%s:%d Fatal: cannot bind port (ipv4:%d). ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            config->PORT,
            chirp
        );
        return tmp_err;  // NOCOV UV_EADDRINUSE can happen in tcp_bind or
                         // listen on my systems it happends in listen
    }
    if(uv_tcp_nodelay(&protocol->serverv4, 1) < 0) {
        return CH_UV_ERROR;  // NOCOV don't know how to produce
    }
    if(uv_listen(
            (uv_stream_t*) &protocol->serverv4,
            config->BACKLOG,
            _ch_pr_new_connection_cb
    ) < 0) {
        fprintf(
            stderr,
            "%s:%d Fatal: cannot listen port (ipv4:%d). ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            config->PORT,
            chirp
        );
        return CH_EADDRINUSE;
    }

    // IPv6, as the dual stack feature doesn't work everywhere we bind both
    uv_tcp_init(ichirp->loop, &protocol->serverv6);
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
        fprintf(
            stderr,
            "%s:%d Fatal: cannot bind port (ipv6:%d). ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            config->PORT,
            chirp
        );
        return tmp_err; // NOCOV errors happend for IPV4
    }
    if(uv_tcp_nodelay(&protocol->serverv6, 1) < 0) {
        return CH_UV_ERROR; // NOCOV errors happend for IPV4
    }
    if(uv_listen(
            (uv_stream_t*) &protocol->serverv6,
            config->BACKLOG,
            _ch_pr_new_connection_cb
    ) < 0) {
        fprintf(
            stderr,
            "%s:%d Fatal: cannot listen port (ipv6:%d). ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            config->PORT,
            chirp
        );
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
    L(chirp, "Closing protocol. ch_chirp_t:%p", chirp);
    _ch_pr_close_free_connections(chirp, protocol->connections);
    uv_close((uv_handle_t*) &protocol->serverv4, ch_chirp_close_cb);
    uv_close((uv_handle_t*) &protocol->serverv6, ch_chirp_close_cb);
    chirp->_->closing_tasks += 2;
    _ch_pr_free_receipts(chirp, protocol->receipts);
    _ch_pr_free_receipts(chirp, protocol->late_receipts);
    return CH_SUCCESS;
}
//
