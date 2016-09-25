// ============
// Chirp Header
// ============
//
// .. code-block:: cpp

#ifndef ch_chirp_h
#define ch_chirp_h

#include "../include/chirp_obj.h"
#include "protocol.h"

// .. c:type:: ch_chirp_flags_t
//
//    Represents chirps flags.
//
//    .. c:member:: CH_CHIRP_AUTO_STOP
//
//       Stop the loop on closing. This is useful if the loop is only used by
//       chirp.
//
//    .. c:member:: CH_CHIRP_CLOSED
//
//       Chirp is closed.
//
// .. code-block:: cpp
//
typedef enum {
    CH_CHIRP_AUTO_STOP   = 1 << 0,
    CH_CHIRP_CLOSED      = 1 << 1,
    CH_CHIRP_CLOSING     = 1 << 2,
} ch_chirp_flags_t;

// .. c:type:: ch_chirp_int_t
//
//    Chirp object.
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
    int           closing_tasks;
    uv_async_t    close;
    uv_prepare_t  close_check;
    ch_protocol_t protocol;
};

// .. c:function::
static void
_ch_close_async_cb(uv_async_t* handle);
//
//    Internal callback to close chirp. Makes ch_chirp_close_ts thread-safe
//
// .. c:function::
static
void*
_ch_chirp_std_alloc(
        size_t suggested_size,
        size_t required_size,
        size_t* provided_size
);
//
//    Standard memory allocator used if no allocator is supplied by the user.
//
// .. c:function::
static
void
_ch_chirp_std_free(void* buf);
//
//    Standard free if no free is supplied by the user.
//
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
// TODO param
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
// TODO param
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
// TODO param
//
// .. code-block:: cpp
//
{
    chirp->config->FREE_CB(buf);
}
// .. c:function::
void
ch_chirp_close_cb(uv_handle_t* handle);
//
//    Reduce callback semaphore.
//
//    TODO params
//
// .. c:function::
static void
_ch_chirp_check_closing_cb(uv_prepare_t* handle);
//
//    Close chirp when the closing semaphore reaches zero.
//
//    TODO params
//
// .. c:function::
static void
_ch_chirp_closing_down_cb(uv_handle_t* handle);
//
//    Closing chirp after the check callback has been closed.
//
//    TODO params
//

#endif //ch_chirp_h
