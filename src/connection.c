// ==========
// Connection
// ==========
//
// .. code-block:: cpp
//
#include "connection.h"
#include "chirp.h"
#include "util.h"

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
    if(conn->buffer) {
        ch_chirp_free(chirp, conn->buffer);
    }
    ch_chirp_free(chirp, conn);
    chirp->_->closing_tasks -= 1;
    L(
        chirp,
        "Closed connection, closing semaphore (%d). ch_connection_t:%p, ch_chirp_t:%p",
        chirp->_->closing_tasks,
        conn,
        chirp
    );
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
    if(conn->flags & CH_CN_SHUTTING_DOWN) {
        return CH_IN_PRORESS;
    }
    conn->flags |= CH_CN_SHUTTING_DOWN;
    tmp_err = _ch_uv_error_map(uv_shutdown(
        &conn->shutdown_req,
        (uv_stream_t*) &conn->client,
        ch_cn_shutdown_cb
    ));
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;
    }
    chirp->_->closing_tasks += 1;
    L(chirp, "Shutdown connection. ch_connection_t:%p, ch_chirp_t:%p", conn, chirp);
    return CH_SUCCESS;
}
// .. c:function::
void
ch_cn_shutdown_cb(uv_shutdown_t* req, int status)
//    :noindex:
//
//    see: :c:func:`ch_cn_shutdown_cb`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = req->handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    L(chirp, "Shutdown callback called. ch_connection_t:%p, ch_chirp_t:%p", conn,  chirp);
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
        L(
            chirp,
            "Closing connection after shutdown. "
            "ch_connection_t:%p, ch_chirp_t:%p",
            conn,
            chirp
        );
        // chirp->_->closing_tasks += 0; One is finished, one is added
    }
}
// .. c:function::
void
ch_cn_read_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
//    :noindex:
//
//    see: :c:func:`ch_cn_read_alloc_cb`
//
// .. code-block:: cpp
//
{
    size_t provided_size;
    ch_connection_t* conn = handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    if(!conn->buffer) {
        conn->buffer = ch_chirp_alloc_var(
            chirp,
            suggested_size,
            CH_LIB_UV_MIN_BUFFER,
            &provided_size
        );
        conn->buffer_size = provided_size;
        conn->flags |= CH_CN_BUF_USED;
    } else {
        A(!conn->flags & CH_CN_BUF_USED, "Buffer still used");
    }
    buf->base = conn->buffer;
    buf->len = conn->buffer_size;
}


SGLIB_DEFINE_RBTREE_FUNCTIONS( // NOCOV
    ch_connection_t,
    left,
    right,
    color_field,
    CH_CONNECTION_CMP
);
