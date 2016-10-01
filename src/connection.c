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

// .. c:function::
static
void
_ch_cn_shutdown_timeout_cb(uv_timer_t* handle);
//
//    Called after shutdown timeout. Closing the connection even though
//    shutdown was delayed.
//

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
    chirp->_->closing_tasks -= 1;
    conn->shutdown_tasks -= 1;
    A(conn->shutdown_tasks > -1, "Shutdown semaphore dropped below zero");
    L(
        chirp,
        "Shutdown sempaphore (%d). ch_connection_t:%p, ch_chirp_t:%p",
        conn->shutdown_tasks,
        conn,
        chirp
    );

    if(conn->shutdown_tasks == 0) {
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
    int tmp_err;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    if(conn->flags & CH_CN_SHUTTING_DOWN) {
        L(
            chirp,
            "Error: Shutdown in progress. ch_connection_t:%p, ch_chirp_t:%p",
            conn,
            chirp
        );
        return CH_IN_PRORESS;
    }
    conn->flags |= CH_CN_SHUTTING_DOWN;
    tmp_err = uv_shutdown(
        &conn->shutdown_req,
        (uv_stream_t*) &conn->client,
        ch_cn_shutdown_cb
    );
    if(tmp_err != CH_SUCCESS) {
        L(
            chirp,
            "Error: uv_shutdown returned error: %d. ch_connection_t:%p, ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
        return _ch_uv_error_map(tmp_err);
    }
    tmp_err = uv_timer_init(ichirp->loop, &conn->shutdown_timeout);
    if(tmp_err != CH_SUCCESS) {
        L(
            chirp,
            "Error: Initializing shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    chirp->_->closing_tasks += 1;
    conn->shutdown_timeout.data = conn;
    tmp_err = uv_timer_start(
        &conn->shutdown_timeout,
        _ch_cn_shutdown_timeout_cb,
        ichirp->config.TIMEOUT * 1000,
        0
    );
    if(tmp_err != CH_SUCCESS) {
        L(
            chirp,
            "Error: Starting shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    L(chirp, "Shutdown connection. ch_connection_t:%p, ch_chirp_t:%p", conn, chirp);
    return CH_SUCCESS;
}
// .. c:function:
void
ch_cn_shutdown_cb(uv_shutdown_t* req, int status)
//    :noindex:
//
//    see: :c:func:`_ch_cn_shutdown_cb`
//
// .. code-block:: cpp
//
{
    int tmp_err;
    ch_connection_t* conn = req->handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    L(chirp, "Shutdown callback called. ch_connection_t:%p, ch_chirp_t:%p", conn,  chirp);
    tmp_err = uv_timer_stop(&conn->shutdown_timeout);
    if(tmp_err != CH_SUCCESS) {
        L(
            chirp,
            "Error: Stopping shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    uv_handle_t* handle = (uv_handle_t*) req->handle;
    if(uv_is_closing(handle)) {
        chirp->_->closing_tasks -= 1;
        L(
            chirp,
            "Error: connection already closed after shutdown. "
            "ch_connection_t:%p, ch_chirp_t:%p",
            conn,
            chirp
        );
    } else {
        uv_close((uv_handle_t*) req->handle, ch_cn_close_cb);
        uv_close((uv_handle_t*) &conn->shutdown_timeout, ch_cn_close_cb);
        conn->shutdown_tasks += 2;
        chirp->_->closing_tasks += 1;
        L(
            chirp,
            "Closing connection after shutdown. "
            "ch_connection_t:%p, ch_chirp_t:%p",
            conn,
            chirp
        );
    }
}
// .. c:function:
static
void
_ch_cn_shutdown_timeout_cb(uv_timer_t* handle)
//    :noindex:
//
//    see: :c:func:`ch_cn_shutdown_timeout_cb`
//
// .. code-block:: cpp
//
{
    int tmp_err;
    ch_connection_t* conn = handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    chirp->_->closing_tasks -= 1;
    ch_cn_shutdown_cb(&conn->shutdown_req, 1);
    tmp_err = uv_cancel((uv_req_t*) &conn->shutdown_req);
    if(tmp_err != CH_SUCCESS) {
        L(
            chirp,
            "Error: Candling shutdown timeout failed: %d. ch_connection_t:%p,"
            " ch_chirp_t:%p",
            tmp_err,
            conn,
            chirp
        );
    }
    L(chirp, "Shutdown timed out closing. ch_connection_t:%p, ch_chirp_t:%p", conn, chirp);
}
