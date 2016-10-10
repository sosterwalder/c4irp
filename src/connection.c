// ==========
// Connection
// ==========
//
// .. code-block:: cpp
//
#include "connection.h"
#include "chirp.h"
#include "util.h"

// Sglib Prototypes
// ================
//
// .. code-block:: cpp
//
SGLIB_DEFINE_RBTREE_FUNCTIONS( // NOCOV
    ch_connection_t,
    left,
    right,
    color_field,
    CH_CONNECTION_CMP
);

// Declarations
// ============

// .. c:function::
static
void
_ch_cn_shutdown_cb(uv_shutdown_t* req, int status);
//
//    Called by libuv after shutting a connection down.
//
//    :param uv_shutdown_t* req: Shutdown request type, holding the
//                               connection handle
//    :param int status: The status after the shutdown. 0 in case of
//                       success, < 0 otherwise

// .. c:function::
static
ch_inline
ch_error_t
_ch_cn_shutdown_gen(
        ch_connection_t* conn,
        uv_shutdown_cb shutdown_cb,
        uv_timer_cb timer_cb
);
//
//    Generic version of shutdown, called by ch_cn_shutdown and
//    ch_ch_shutdown_end.
//
//    :param ch_connection_t* conn: Connection dictionary holding a
//                                  chirp instance.
//    :param uv_shutdown_cb shutdown_cb: Callback which gets called
//                                       after the shutdown is
//                                       complete
//    :param uv_timer_cb timer_cb: Callback which gets called after
//                                 the timer has reached 0
//    :return: A chirp error. see: :c:type:`ch_error_t`
//    :rtype: ch_error_t

// .. c:function::
static
ch_inline
void
_ch_cn_shutdown_gen_cb(
        uv_shutdown_t* req,
        int status,
        uv_close_cb close_cb
);
//
//    Generic version of the shutdown callback, called by ch_cn_shutdown_cb and
//    ch_ch_shutdown_end_cb.
//
//    :param uv_shutdown_t* req: Shutdown request type, holding the
//                               connection handle
//    :param int status: The status after the shutdown. 0 in case of
//                       success, < 0 otherwise
//    :param uv_close_cb close_cb: Callback which will be called
//                                 asynchronously after uv_close
//                                 has been called

// .. c:function::
static
void
_ch_cn_shutdown_timeout_cb(uv_timer_t* handle);
//
//    Called after shutdown timeout. Closing the connection even though
//    shutdown was delayed.
//
//    :param uv_timer_t* handle: Timer handle to schedule callback

// .. c:function::
static
ch_inline
void
_ch_cn_shutdown_timeout_gen_cb(
        uv_timer_t* handle,
        uv_shutdown_cb shutdown_cb
);
//
//    Generic version of the shutdown callback, called by
//    _ch_cn_shutdown_timeout_cb and _ch_cn_shutdown_timeout_end_cb.
//
//    :param uv_timer_t* handle: Timer handle to schedule callback
//    :param uv_shutdown_cb shutdown_cb: Callback which gets called
//                                       after the shutdown is
//                                       complete

// Definitions
// ===========

// .. c:function::
void
ch_cn_close_cb(uv_handle_t* handle)
//    :noindex:
//
//    see: :c:func:`ch_cn_close_cb`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    if(ichirp->flags & CH_CHIRP_CLOSING)
        chirp->_->closing_tasks -= 1;
    conn->shutdown_tasks -= 1;
    A(conn->shutdown_tasks > -1, "Shutdown semaphore dropped below zero");
    L(
        chirp,
        "Shutdown semaphore (%d). ch_connection_t:%p, ch_chirp_t:%p",
        conn->shutdown_tasks,
        conn,
        chirp
    );
    // In production we allow the semaphore to drop below 0, but we log an
    // error
    if(conn->shutdown_tasks < 1) {
        if(conn->buffer) {
            ch_free(conn->buffer);
        }
        ch_free(conn);
        L(
            chirp,
            "Closed connection, closing semaphore (%d). ch_connection_t:%p, ch_chirp_t:%p",
            chirp->_->closing_tasks,
            conn,
            chirp
        );
    }
    if(conn->shutdown_tasks < 0) {
        E(
            chirp,
            "Shutdown semaphore dropped blow 0. ch_chirp_t:%p",
            chirp
        );
    }
}


// .. c:function::
static
ch_inline
void
_ch_cn_shutdown_gen_cb(
        uv_shutdown_t* req,
        int status,
        uv_close_cb close_cb
)
//    :noindex:
//
//    see: :c:func:`_ch_cn_shutdown_gen_cb`
//
// .. code-block:: cpp
//
{
    int tmp_err;
    ch_connection_t* conn = req->handle->data;
    ch_chirp_t* chirp = conn->chirp;
    ch_chirp_int_t* ichirp = chirp->_;
    L(chirp, "Shutdown callback called. ch_connection_t:%p, ch_chirp_t:%p", conn,  chirp);
    tmp_err = uv_timer_stop(&conn->shutdown_timeout);
    if(tmp_err != CH_SUCCESS) {
        E(
            chirp,
            "Stopping shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    uv_handle_t* handle = (uv_handle_t*) req->handle;
    if(uv_is_closing(handle)) {
        if(ichirp->flags & CH_CHIRP_CLOSING)
            chirp->_->closing_tasks -= 1;
        E(
            chirp,
            "Connection already closed after shutdown. "
            "ch_connection_t:%p, ch_chirp_t:%p",
            conn,
            chirp
        );
    } else {
        uv_close((uv_handle_t*) req->handle, close_cb);
        uv_close((uv_handle_t*) &conn->shutdown_timeout, close_cb);
        if(ichirp->flags & CH_CHIRP_CLOSING)
            chirp->_->closing_tasks += 1;
        conn->shutdown_tasks += 2;
        L(
            chirp,
            "Closing connection after shutdown. "
            "ch_connection_t:%p, ch_chirp_t:%p",
            conn,
            chirp
        );
    }
}

// .. c:function::
static
void
_ch_cn_shutdown_cb(uv_shutdown_t* req, int status)
//    :noindex:
//
//    see: :c:func:`_ch_cn_shutdown_cb`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = req->handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    return _ch_cn_shutdown_gen_cb(
        req,
        status,
        ch_cn_close_cb
    );
}

// .. c:function::
static
ch_inline
ch_error_t
_ch_cn_shutdown_gen(
    ch_connection_t* conn,
    uv_shutdown_cb shutdown_cb,
    uv_timer_cb timer_cb
)
//    :noindex:
//
//    see: :c:func:`_ch_cn_shutdown_gen`
//
// .. code-block:: cpp
//
{
    int tmp_err;
    ch_chirp_t* chirp = conn->chirp;
    ch_chirp_int_t* ichirp = chirp->_;
    if(conn->flags & CH_CN_SHUTTING_DOWN) {
        E(
            chirp,
            "Shutdown in progress. ch_connection_t:%p, ch_chirp_t:%p",
            conn,
            chirp
        );
        return CH_IN_PRORESS;
    }
    conn->flags |= CH_CN_SHUTTING_DOWN;
    tmp_err = uv_shutdown(
        &conn->shutdown_req,
        (uv_stream_t*) &conn->client,
        shutdown_cb
    );
    if(tmp_err != CH_SUCCESS) {
        E(
            chirp,
            "uv_shutdown returned error: %d. ch_connection_t:%p, ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
        return _ch_uv_error_map(tmp_err);
    }
    tmp_err = uv_timer_init(ichirp->loop, &conn->shutdown_timeout);
    if(tmp_err != CH_SUCCESS) {
        E(
            chirp,
            "Initializing shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    if(ichirp->flags & CH_CHIRP_CLOSING)
        chirp->_->closing_tasks += 1;
    conn->shutdown_timeout.data = conn;
    tmp_err = uv_timer_start(
        &conn->shutdown_timeout,
        timer_cb,
        ichirp->config.TIMEOUT * 1000,
        0
    );
    if(tmp_err != CH_SUCCESS) {
        E(
            chirp,
            "Starting shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    L(chirp, "Shutdown connection. ch_connection_t:%p, ch_chirp_t:%p", conn, chirp);
    return CH_SUCCESS;
}

// .. c:function::
static
void
_ch_cn_shutdown_timeout_cb(uv_timer_t* handle)
//    :noindex:
//
//    see: :c:func:`_ch_cn_shutdown_timeout_cb`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    return _ch_cn_shutdown_timeout_gen_cb(
        handle,
        _ch_cn_shutdown_cb
    );
}

// .. c:function::
static
ch_inline
void
_ch_cn_shutdown_timeout_gen_cb(
        uv_timer_t* handle,
        uv_shutdown_cb shutdown_cb
)
//    :noindex:
//
//    see: :c:func:`_ch_cn_shutdown_timeout_gen_cb`
//
// .. code-block:: cpp
//
{
    int tmp_err;
    ch_connection_t* conn = handle->data;
    ch_chirp_t* chirp = conn->chirp;
    ch_chirp_int_t* ichirp = chirp->_;
    if(ichirp->flags & CH_CHIRP_CLOSING)
        chirp->_->closing_tasks -= 1;
    shutdown_cb(&conn->shutdown_req, 1);
    tmp_err = uv_cancel((uv_req_t*) &conn->shutdown_req);
    if(tmp_err != CH_SUCCESS) {
        E(
            chirp,
            "Canceling shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    L(
        chirp,
        "Shutdown timed out closing. ch_connection_t:%p, ch_chirp_t:%p",
        conn,
        chirp
    );
}

// .. c:function::
ch_error_t
ch_cn_init(ch_chirp_t* chirp, ch_connection_t* conn)
//    :noindex:
//
//    see: :c:func:`ch_cn_init`
//
// .. code-block:: cpp
//
{
    memset(conn, 0, sizeof(ch_connection_t));
    conn->chirp = chirp;
    return CH_SUCCESS;
}

// .. c:function::
void
ch_cn_read_alloc_cb(
        uv_handle_t* handle,
        size_t suggested_size,
        uv_buf_t* buf
)
//    :noindex:
//
//    see: :c:func:`ch_cn_read_alloc_cb`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    if(!conn->buffer) {
        if(ichirp->config.BUFFER_SIZE == 0) {
            conn->buffer      = ch_alloc(suggested_size);
            conn->buffer_size = suggested_size;
        } else {
            conn->buffer      = ch_alloc(ichirp->config.BUFFER_SIZE);
            conn->buffer_size = ichirp->config.BUFFER_SIZE;
        }
        conn->flags |= CH_CN_BUF_USED;
    } else {
        A(!(conn->flags & CH_CN_BUF_USED), "Buffer still used");
    }
    buf->base = conn->buffer;
    buf->len = conn->buffer_size;
}

// .. c:function::
ch_error_t
ch_cn_shutdown(ch_connection_t* conn)
//    :noindex:
//
//    see: :c:func:`ch_cn_shutdown`
//
// .. code-block:: cpp
//
{
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    return _ch_cn_shutdown_gen(
        conn,
        _ch_cn_shutdown_cb,
        _ch_cn_shutdown_timeout_cb
    );
}
