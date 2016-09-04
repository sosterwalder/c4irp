// ============
// Chirp Header
// ============
//
// .. code-block:: cpp

#ifndef ch_chirp_h
#define ch_chirp_h

#include "../include/chirp_obj.h"
#include "protocol.h"

// .. c:type:: ch_chirp_int_t
//
//    Chirp object.
//
//
//    .. c:member:: uv_async_t close
//
//       async handler to close chirp on the main-loop
//
//    .. c:member:: int auto_start
//
//       true if we have to close the libuv loop, otherwise the loop was
//       supplied by the user
//
// .. code-block:: cpp
//
struct ch_chirp_int {
    int                      auto_start;
    uv_async_t               close;
    ch_protocol_t            protocol;
};

// .. c:function::
static
ch_inline
void*
ch_chirp_alloc_var(
        ch_chirp_t* chirp,
        size_t suggested_size,
        size_t required_size,
        size_t* provided_size
)
//
//   Allocate variable memory. The user can decide to allocate less memory.
//   This is the interface provided by libuv, we hand the choice down to our
//   user.
//
// .. code-block:: cpp
//
{
    void* handle;
    handle = chirp->config->ALLOC_CB(
        suggested_size,
        required_size,
        provided_size
    );
    A(
        *provided_size >= required_size,
        "Not enough memory provided by ALLOC_CB"
    );
    return handle;
}

// .. c:function::
static
ch_inline
void*
ch_chirp_alloc(
        ch_chirp_t* chirp,
        size_t size
)
//
//   Allocate fixed amount of memory.
//
// .. code-block:: cpp
//
{
    void* handle;
    size_t provided_size;
    handle = chirp->config->ALLOC_CB(
        size,
        size,
        &provided_size
    );
    A(provided_size >= size, "Not enough memory provided by ALLOC_CB");
    return handle;
}

// .. c:function::
static
ch_inline
void
ch_chirp_free(
        ch_chirp_t* chirp,
        void* buf
)
//
//   Free a memory handle.
//
// .. code-block:: cpp
//
{
    chirp->config->FREE_CB(buf);
}

#endif //ch_chirp_h
