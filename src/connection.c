// ==========
// Connection
// ==========
//
// .. code-block:: cpp
//
#include "connection.h"

SGLIB_DEFINE_RBTREE_FUNCTIONS( // NOCOV
    ch_connection_t,
    left,
    right,
    color_field,
    CH_CONNECTION_CMP
);
