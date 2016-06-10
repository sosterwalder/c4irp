// ================
// Chirp Obj Header
// ================
//
// .. code-block:: cpp

#ifndef ch_inc_c4irp_obj_h
#define ch_inc_c4irp_obj_h

#include "error.h"

#include <uv.h>

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
// .. code-block:: cpp

typedef struct {
    int  REUSE_TIME;
    int  TIMEOUT;
    int  PORT;
    int  BACKLOG;
    char BIND_V6[16];
    char BIND_V4[4];
} ch_config_t;

// .. c:type:: ch_log_cb_t
//
//    Logging callback
//
// .. code-block:: cpp

typedef void (*ch_log_cb_t)(char msg[]);

//
// .. c:var:: ch_config_defaults
//
//    Default config of c4irp.
//
// .. code-block:: cpp

extern ch_config_t ch_config_defaults;

// .. c:type:: ch_chirp_int_t
//
//    Opaque pointer to internals.
//
// .. code-block:: cpp

typedef struct ch_chirp_int ch_chirp_int_t;

// .. c:type:: ch_chirp_t
//
//    Chirp object.
//  
// .. code-block:: cpp

typedef struct {
    char            identity[16];
    uv_loop_t*      loop;
    ch_config_t     config;
    ch_log_cb_t     _log;
    ch_chirp_int_t* _;
} ch_chirp_t;

// .. c:function::
static
inline
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
inline
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
inline
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
inline
int
ch_run(uv_loop_t* loop, uv_run_mode mode)
//
//    An alias for uv_run. Please refer to the libuv documentation.
//
//    :param uv_loop_t*  loop: Loop struct allocated by user.
//    :param uv_run_mode mode: Run mode.
//
// .. code-block:: cpp
//
{
    return uv_run(loop, mode);
}
// .. c:function::
extern
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t config, uv_loop_t* loop);
//
//    Intialiaze a chirp object. Memory is provided by caller. You must call
//    :c:func:`ch_chirp_close` to cleanup the object.
//
//    :param ch_chirp_t* chirp: Out: Chirp object
//    :param ch_config_t config: Chirp config
//
// .. c:function::
extern
ch_error_t
ch_chirp_run(ch_config_t config, ch_chirp_t** chirp);
//
//    Initializes, runs and cleans everything. Everything being:
//    TODO: Add message callback
//
//      * c4irp object
//      * uv-loop
//      * uv-sockets
//      * callbacks
//
//     The method blocks, but chirp paramenter will be set. Can be used to run
//     chirp in a user defined thread. Use ch_chirp_close_ts to close it chirp
//     in any other thread.
//
//    :param ch_config_t config: Chirp config
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
// .. code-block:: cpp

#endif //ch_inc_c4irp_obj_h
