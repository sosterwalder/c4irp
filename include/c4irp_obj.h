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

//
// .. c:var:: ch_config_defaults
//
//    Default config of c4irp.
//
// .. code-block:: cpp

extern ch_config_t ch_config_defaults;

// .. c:type:: ch_chirp_t
//
//    Chirp object.
//  
// .. code-block:: cpp

typedef struct {
    char identity[16];
    uv_loop_t* loop;
    ch_config_t config;
    struct sockaddr_in _addrv4;
    struct sockaddr_in6 _addrv6;
    uv_tcp_t _serverv4;
    uv_tcp_t _serverv6;
} ch_chirp_t;

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
ch_chirp_run(ch_config_t config);
//
//    Initializes, runs and cleans everything. Everything being:
//    TODO: Add message callback
//
//      * c4irp object
//      * uv-loop
//      * uv-sockets
//      * callbacks
//
//    :param ch_config_t config: Chirp config
//
// .. c:function::
extern
ch_error_t
ch_chirp_close(ch_chirp_t* chirp);
//
//    Cleanup chirp object. Will remove all callbacks. Pending outs will be
//    ignored after calling free.
//
//    :param ch_chirp_t chirp: Chirp object
//
// .. code-block:: cpp

#endif //ch_inc_c4irp_obj_h
