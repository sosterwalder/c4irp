// ============
// Chirp Header
// ============
//
// .. code-block:: cpp

#ifndef ch_c4irp_h
#define ch_c4irp_h

#include "../include/c4irp_obj.h"

#include <uv.h>

struct ch_chirp_s {
    char identity[16];
    uv_loop_t loop;
};

#endif //ch_c4irp_h
