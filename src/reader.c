// ======
// Reader
// ======
//
// .. code-block:: cpp
//
#include "reader.h"

#include "chirp.h"
#include "connection.h"


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
    ch_rd_handshake_t hs;
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    ch_reader_t* reader = &conn->reader;
    switch(reader->state) {
        case CH_RD_START:
            hs.port = ichirp->public_port;
            hs.max_timeout = (
                (ichirp->config.RETRIES + 2) * ichirp->config.TIMEOUT
            );
            memcpy(hs.identity, ichirp->identity, 16);
            ch_cn_write(conn, &hs, sizeof(hs), NULL);
            break;
        default:
            A(0, "Unknown reader state");
    }
}
