// ====
// Util
// ====
//
// .. code-block:: cpp
//
#include "util.h"

ch_alloc_cb_t _ch_alloc_cb = malloc;
ch_free_cb_t _ch_free_cb = free;
ch_realloc_cb_t _ch_realloc_cb = realloc;
