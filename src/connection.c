// ==========
// Connection
// ==========
//
// .. code-block:: cpp
//
#include "connection.h"
#include "chirp.h"

// .. c:function::
void
ch_cn_read_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
//    :noindex:
//
//    see: :c:func:`_ch_cn_read_alloc`
//
// .. code-block:: cpp
//
{
    ch_connection_t* conn = handle->data;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    if(!conn->buffer) {
        conn->buffer = ch_chirp_alloc(
            chirp,
            suggested_size
        );
        conn->buffer_used = 1;
    } else {
        A(!conn->buffer_used, "Buffer still used");
    }
    buf->base = conn->buffer;
    buf->len = suggested_size;
}


SGLIB_DEFINE_RBTREE_FUNCTIONS( // NOCOV
    ch_connection_t,
    left,
    right,
    color_field,
    CH_CONNECTION_CMP
);
