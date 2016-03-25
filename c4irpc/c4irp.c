// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "c4irp.h"

#include <stdlib.h>
#include <uv.h>

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
