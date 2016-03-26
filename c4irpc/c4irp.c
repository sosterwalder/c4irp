// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "c4irp.h"

#include <stdlib.h>
#include <uv.h>

//
// .. c:var:: ch_config_defaults
//
//    Default config of c4irp.
//
// .. code-block:: cpp
//
ch_config_t ch_config_defaults = {
    .REUSE_TIME = 30,
    .TIMEOUT    = 5,
    .PORT       = 2998,
    .BIND_V6    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// .. c:function::
ch_error_t
ch_chirp_init(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_init`
//
// .. code-block:: cpp
//
{
    ch_chirp_t chirp_p = malloc(sizeof(struct ch_chirp_s));
    uv_loop_init(&chirp_p->loop);
    *chirp =  chirp_p;
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_free(ch_chirp_t chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_free`
//
// .. code-block:: cpp
//
{
    if(uv_loop_close(&chirp->loop)) {
        return CH_UV_ERROR; // NOCOV
    }
    return CH_SUCCESS;
}
