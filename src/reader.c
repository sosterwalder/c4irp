// ======
// Reader
// ======
//
// .. code-block:: cpp
//
#include "reader.h"

#include "chirp.h"
#include "connection.h"
#include "util.h"

// Declarations
// ============

// .. c:function::
static
ch_inline
void
_ch_rd_handshake(
        ch_connection_t* conn,
        ch_reader_t* reader,
        void* buf,
        size_t read
);
//
//    Handle
//
//    :param ch_connection_t* conn: Connection
//    :param ch_readert* reader: Reader
//    :param void* buf: Buffer containing bytes read
//    :param size_t read: Count of bytes read
//
//
// Definitions
// ===========

// .. c:function::
static
ch_inline
void
_ch_rd_handshake(
        ch_connection_t* conn,
        ch_reader_t* reader,
        void* buf,
        size_t read
)
//    :noindex:
//
//    see: :c:func:`_ch_rd_handshake`
//
// .. code-block:: cpp
//
{
    ch_connection_t* old_conn;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    ch_protocol_t* protocol = &ichirp->protocol;
    if(read < sizeof(reader->hs)) {
        E(
            chirp,
            "Illegal handshake size -> shutdown. ch_chirp_t:%p, "
            "ch_connection_t:%p",
            chirp,
            conn
        );
        ch_cn_shutdown(conn);
        return;
    }
    memcpy(&reader->hs, buf, sizeof(reader->hs));
    conn->port = ntohs(reader->hs.port);
    conn->max_timeout = ntohs(
        reader->hs.max_timeout + (
            (ichirp->config.RETRIES + 2) * ichirp->config.TIMEOUT
        )
    );
    memcpy(
        conn->remote_identity,
        reader->hs.identity,
        sizeof(conn->remote_identity)
    );
    old_conn = sglib_ch_connection_t_find_member(
        protocol->connections,
        conn
    );
    // If there is a network race condition we replace the old connection and
    // leave the old one for garbage collection
    if(old_conn) {
        // Since we reuse the tree members we have to delete the connection
        // from the old data-structure, before adding it to the new.
        L(
            chirp,
            "ch_connection_t:%p replaced ch_connection_t:%p. "
            "ch_chirp_t:%p",
            conn,
            old_conn,
            chirp
        );
        sglib_ch_connection_t_delete(
            &protocol->connections,
            old_conn
        );
        sglib_ch_connection_set_t_add(
            &protocol->old_connections,
            old_conn
        );
    }
    sglib_ch_connection_t_add(
        &protocol->connections,
        conn
    );
#   ifndef NDEBUG
    {
        ch_text_address_t addr; 
        char identity[33];
        uv_inet_ntop(
            conn->ip_protocol == CH_IPV6 ? AF_INET6 : AF_INET,
            conn->address,
            addr.data,
            sizeof(addr)
        );
        ch_bytes_to_hex(
            conn->remote_identity,
            sizeof(conn->remote_identity),
            identity,
            sizeof(identity)
        );
        L(
            chirp,
            "Handshake with remote %s:%d (%s) done. ch_chirp_t:%p, "
            "ch_connection_t:%p",
            addr.data,
            conn->port,
            identity,
            chirp,
            conn
        );
    }
#   endif
}

// .. c:function::
void
ch_rd_read(ch_connection_t* conn, void* buf, size_t read)
//    :noindex:
//
//    see: :c:func:`ch_rd_read`
//
// .. code-block:: cpp
//
{
    // TODO excpect partial for buffers
    size_t bytes_handled = 0;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    ch_reader_t* reader = &conn->reader;
    do {
        switch(reader->state) {
            case CH_RD_START:
                reader->hs.port = htons(ichirp->public_port);
                reader->hs.max_timeout = htons(
                    (ichirp->config.RETRIES + 2) * ichirp->config.TIMEOUT
                );
                memcpy(reader->hs.identity, ichirp->identity, 16);
                ch_cn_write(
                    conn,
                    &reader->hs,
                    sizeof(ch_rd_handshake_t),
                    NULL
                );
                reader->state = CH_RD_HANDSHAKE;
                break;
            case CH_RD_HANDSHAKE:
                _ch_rd_handshake(
                    conn,
                    reader,
                    buf,
                    read
                );
                bytes_handled += sizeof(reader->hs);
                break;
            default:
                A(0, "Unknown reader state");
                break;
        }
    } while(bytes_handled < read);
}
