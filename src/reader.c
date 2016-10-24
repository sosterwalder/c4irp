// ======
// Reader
// ======
//
// .. code-block:: cpp
//
#include "reader.h"

#include "connection.h"

// .. c:function::
void
ch_rd_read(ch_connection_t* conn, char* buf, size_t read)
//    :noindex:
//
//    see: :c:func:`ch_rd_read`
//
// .. code-block:: cpp
//
{
    ch_chirp_t* chirp = conn->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_reader_t* reader = &conn->reader;
    switch(reader->state) {
        case CH_RD_START:
            break;
        default:
            A(0, "Unknown reader state");
    }
}
