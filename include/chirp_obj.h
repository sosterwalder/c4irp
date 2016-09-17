// ================
// Chirp Obj Header
// ================
//
// .. code-block:: cpp

#ifndef ch_inc_chirp_obj_h
#define ch_inc_chirp_obj_h

#include "common.h"

// .. c:type:: ch_alloc_cb
//
//    Callback used by chirp to request memory. It can be set in ch_config_t.
//
//    .. c:member:: size_t suggested_size
//
//       The size the user should allocate if possible
//
//    .. c:member:: size_t required_size
//
//       The size the user has to allocate
//
//    .. c:member:: size_t provided_size
//
//       Out: The size the user has allocated
//
//    Libuv wants a buffer of 65536 bytes per stream. It will work with less
//    though.  So we decided that 1k is the absolute minimum see
//    CH_LIB_UV_MIN_BUFFER in config.h. Please provide the suggested_size, except
//    if you're on very restricted embedded system.
//
// .. code-block:: cpp
//
typedef void* (*ch_alloc_cb)(
        size_t suggested_size,
        size_t required_size,
        size_t* provided_size
);

// .. c:type:: ch_free_cb
//
//    Callback used by chirp to free memory. It can be set in ch_config_t.
//
// .. code-block:: cpp
//
typedef void (*ch_free_cb)(void* buf);

// .. c:type:: ch_config_t
//
//    Chirp configuration.
//
//    .. c:member:: int REUSE_TIME
//
//       Time till a connection gets garbage collected during this time the
//       connection will be reused.
//
//    .. c:member:: int TIMEOUT
//
//       General IO related timeout.
//
//    .. c:member:: int PORT
//
//       Listen-port.
//
//    .. c:member:: int BACKLOG
//
//       TCP-Listen socket backlog.
//
//    .. c:member:: char[16] BIND_V6
//
//       Override IPv6 bind address.
//
//    .. c:member:: char[16] BIND_V4
//
//       Override IPv4 bind address.
//
//    .. c:member:: ch_alloc_cb ALLOC_CB
//
//       Callback used by chirp to request memory. If NULL: the system malloc
//       function is used.
//
//    .. c:member:: ch_free_cb FREE_CB
//
//       Callback used by chirp to free memory. If NULL: the system free
//       function is used.
//
// .. code-block:: cpp

typedef struct {
    int          REUSE_TIME;
    float        TIMEOUT;
    int          PORT;
    int          BACKLOG;
    char         BIND_V6[16];
    char         BIND_V4[4];
    char*        CERT_CHAIN_PEM;
    ch_alloc_cb  ALLOC_CB;
    ch_free_cb   FREE_CB;
} ch_config_t;

// .. c:type:: ch_log_cb_t
//
//    Logging callback
//
// .. code-block:: cpp

typedef void (*ch_log_cb_t)(char msg[]);

//
// .. c:var:: ch_config_defaults
//    :noindex:
//
//    Default config of chirp.
//
//    see: :c:type:`ch_config_defaults`
//
// .. code-block:: cpp

extern ch_config_t ch_config_defaults;

// .. c:type:: ch_chirp_int_t
//    :noindex:
//
//    Opaque pointer to internals.
//
//    see: :c:type:`ch_chirp_int_t`
//
// .. code-block:: cpp

typedef struct ch_chirp_int ch_chirp_int_t;

// .. c:type:: ch_chirp_t
//
//    Chirp object.
//  
//    .. c:member:: char identity[16]
//
//       Identity of this chirp node, 16 bytes
//
//    .. c:member:: uv_loop_t* loop
//
//       libuv event loop used by chirp
//
//    .. c:member:: ch_config_t config
//
//       config used by chirp (DO NOT CHANGE)
//
// .. code-block:: cpp

typedef struct ch_chirp {
    unsigned char   identity[16];
    uv_loop_t*      loop;
    ch_config_t*    config;
    ch_log_cb_t     _log;
    int             _init;
    ch_chirp_int_t* _;
} ch_chirp_t;

// .. c:function::
static
ch_inline
void
ch_chirp_register_log_cb(ch_chirp_t* chirp, ch_log_cb_t log_cb)
//
//    Register a callback for sending log messages.
//
//    :param ch_chirp_t* chirp: Chirp instance
//    :param ch_log_cb_t   log: Callback to be called on log messages
//
// .. code-block:: cpp
//
{
    chirp->_log = log_cb;
}
// .. c:function::
static
ch_inline
int
ch_loop_init(uv_loop_t* loop)
//
//    An alias for uv_loop_init. Please refer to the libuv documentation.
//
//    :param uv_loop_t* loop: Loop struct allocated by user.
//
// .. code-block:: cpp
//
{
    return uv_loop_init(loop);
}
static
ch_inline
int
ch_loop_close(uv_loop_t* loop)
//
//    An alias for uv_loop_close. Please refer to the libuv documentation.
//
//    :param uv_loop_t* loop: Loop struct allocated by user.
//
// .. code-block:: cpp
//
{
    return uv_loop_close(loop);
}
// .. c:function::
static
ch_inline
int
ch_run(uv_loop_t* loop)
//
//    A wrapper for uv_run and runs the loop once again, in case closing chirp's
//    resources caused additional requests/handlers.
//
//    Please refer to the libuv documentation.
//
//    :param uv_loop_t*  loop: Loop struct allocated by user.
//
// .. code-block:: cpp
//
{
    int tmp_err = uv_run(loop, UV_RUN_DEFAULT);
    if(tmp_err != 0) {
        /* On Windows uv_run returns non-zero, which I expect, since we
         * uv_stop() inside a handler that just closed all kinds of resources,
         * which may cause new requests/handlers.
         *
         * I didn't find any documents assuring that my solution is good,
         * though.
         */
        tmp_err = uv_run(loop, UV_RUN_ONCE);
        /* Now we clearly have a problem */
        if(tmp_err != 0) {
            return tmp_err;  // NOCOV only breaking things will trigger this
        }
    }
    return tmp_err;
}
// .. c:function::
extern
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t* config, uv_loop_t* loop);
//
//    Intialiaze a chirp object. Memory is provided by caller. You must call
//    :c:func:`ch_chirp_close` to cleanup the object.
//
//    :param ch_chirp_t* chirp: Out: Chirp object
//    :param ch_config_t* config: Chirp config
//
// .. c:function::
extern
ch_error_t
ch_chirp_run(ch_config_t* config, ch_chirp_t** chirp);
//
//    Initializes, runs and cleans everything. Everything being:
//    TODO: Add message callback
//
//      * chirp object
//      * uv-loop
//      * uv-sockets
//      * callbacks
//
//     The method blocks, but chirp paramenter will be set. Can be used to run
//     chirp in a user defined thread. Use ch_chirp_close_ts to close it chirp
//     in any other thread.
//
//    :param ch_config_t* config: Chirp config
//    :param ch_chirp_t** chirp: Out: Pointer to chirp object pointer. Ca be
//                               NULL
//
// .. c:function::
extern
ch_error_t
ch_chirp_close_ts(ch_chirp_t* chirp);
//
//    Cleanup chirp object. Will remove all callbacks. Pending outs will be
//    ignored after calling free.
//
//    This function is thread-safe
//
//    :param ch_chirp_t chirp: Chirp object
//
// .. c:function::
extern
void
ch_chirp_set_auto_stop(ch_chirp_t* chirp);
//
//    After this function is called :c:func:`ch_chirp_close_ts` will also stop
//    the loop.
//
//    This function is thread-safe
//
//    :param ch_chirp_t chirp: Chirp object
//
// .. code-block:: cpp

#endif //ch_inc_chirp_obj_h
