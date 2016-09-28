// ================
// Chirp Obj Header
// ================
//
// .. code-block:: cpp

#ifndef ch_inc_chirp_obj_h
#define ch_inc_chirp_obj_h

#include "common.h"
#include "callbacks.h"
#include "wrappers.h"

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
//    .. c:member:: ch_alloc_cb_t ALLOC_CB
//
//       Callback used by chirp to request memory. If NULL: the system malloc
//       function is used.
//
//    .. c:member:: ch_free_cb_t FREE_CB
//
//       Callback used by chirp to free memory. If NULL: the system free
//       function is used.
//
// .. code-block:: cpp

typedef struct {
    int           REUSE_TIME;
    float         TIMEOUT;
    int           PORT;
    int           BACKLOG;
    char          BIND_V6[16];
    char          BIND_V4[4];
    char*         CERT_CHAIN_PEM;
    ch_alloc_cb_t ALLOC_CB;
    ch_free_cb_t  FREE_CB;
} ch_config_t;


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

typedef struct ch_chirp_int_s ch_chirp_int_t;

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

typedef struct ch_chirp_s {
    unsigned char   identity[16];
    uv_loop_t*      loop;
    ch_config_t*    config;
    uint8_t         flags;
    ch_log_cb_t     _log;
    int             _init;
    ch_chirp_int_t* _;
} ch_chirp_t;

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
ch_error_t
ch_chirp_init(
        ch_chirp_t* chirp,
        ch_config_t* config,
        uv_loop_t* loop,
        ch_log_cb_t log_cb
);
//
//    Intialiaze a chirp object. Memory is provided by caller. You must call
//    :c:func:`ch_chirp_close` to cleanup the object.
//
//    :param ch_chirp_t* chirp: Out: Chirp object
//    :param ch_config_t* config: Chirp config
//    :param uv_loop_t* loop: Reference to a libuv loop
//    :param ch_log_cb_t log_cb: Callback to logging facility, can be NULL
//
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
