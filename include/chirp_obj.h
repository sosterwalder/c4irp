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
#include "message.h"

// .. c:type:: ch_config_t
//
//   TODO fix types
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
//    .. c:member:: uint8_t RETRIES
//
//       Count of retries till error is reported, by default 1.
//
//    .. c:member:: uint8_t MAX_HANDLERS
//
//       Count of handlers used. Allowed values between 1 and 32. Default: 16.
//       If FLOW_CONTROL is on it must be >= 16.
//
//    .. c:member:: char FLOW_CONTROL
//
//       Flow control prevents overload of one node in chain for workers.
//       Default: 1.
//
//    .. c:member:: char ACKNOWLEDGE
//
//       Acknowledge messages. Is needed for retry and flow-control. Disabling
//       acknowledge can improve performance on long delay connections, but at
//       the risk of overloading the remote and the local machine. You have to
//       set RETRIES and FLOW_CONTROL to 0 or chirp won't accept the config.
//
//    .. c:member:: char CLOSE_ON_SIGINT
//
//       By default chirp closes on SIGINT (Ctrl-C)
//
//    .. c:member:: uint32_t BUFFER_SIZE
//
//       Size of the buffer used for a connection. Defaults to 0, which means
//       use the size requested by libuv. Should not be set below 1024.
//
//    .. c:member:: char[16] BIND_V6
//
//       Override IPv6 bind address.
//
//    .. c:member:: char[4] BIND_V4
//
//       Override IPv4 bind address.
//
//    .. c:member:: unsigned char[16] IDENTITY
//
//       Override the IDENTITY. By default all chars are 0, which means chirp
//       will generate a IDENTITY.
//
//
// .. code-block:: cpp

typedef struct ch_config_s {
    float           REUSE_TIME;
    float           TIMEOUT;
    uint16_t        PORT;
    uint8_t         BACKLOG;
    uint8_t         RETRIES;
    uint8_t         MAX_HANDLERS;
    char            ACKNOWLEDGE;
    char            FLOW_CONTROL;
    char            CLOSE_ON_SIGINT;
    uint32_t        BUFFER_SIZE;
    uint8_t         BIND_V6[16];
    uint8_t         BIND_V4[4];
    uint8_t         IDENTITY[16];
    char*           CERT_CHAIN_PEM;
    char*           DH_PARAMS_PEM;
} ch_config_t;


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
//    Chirp object. It has no public members. And uses an opaque pointer to its
//    internal data structures.
//
// .. code-block:: cpp

typedef struct ch_chirp_s {
    ch_chirp_int_t*    _;
    ch_log_cb_t        _log;
    int                _init;
    char               _color_field;
    struct ch_chirp_s* _left;
    struct ch_chirp_s* _right;
} ch_chirp_t;

// .. c:type:: ch_identity_t
//
//    Struct containing the chirp identity.
//
// .. code-block:: cpp
typedef struct ch_identity_s {
    unsigned char data[16];
} ch_identity_t;

// .. c:function::
extern
void
ch_chirp_config_init(ch_config_t* config);
//
//    Initialize chirp config with defaults
//
//    :param ch_config_t config: Chirp config

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
//    :return: A chirp error. see: :c:type:`ch_error_t`
//    :rtype: ch_error_t
//

// .. c:function::
extern
ch_identity_t
ch_chirp_get_identity(ch_chirp_t* chirp);
//
//    Get the identity of this chirp instance.
//
//    :param   ch_chirp_t chirp: Chirp object
//
//    :return: A chirp identity. see: :c:type:`ch_identity_t`
//    :rtype:  ch_identity_t
//

// .. c:function::
extern
uv_loop_t*
ch_chirp_get_loop(ch_chirp_t* chirp);
//
//    Get the loop of the chirp object.
//
//    :param ch_chirp_t chirp: Chirp object
//
//    TODO: Document return value
//

// .. c:function::
extern
ch_error_t
ch_chirp_init(
        ch_chirp_t* chirp,
        const ch_config_t* config,
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
//    TODO: Document return value
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
ch_chirp_run(const ch_config_t* config, ch_chirp_t** chirp);
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
//     chirp in a user defined thread. Use ch_chirp_close_ts to close chirp
//     in any other thread.
//
//    :param ch_config_t* config: Chirp config
//    :param ch_chirp_t** chirp: Out: Pointer to chirp object pointer. Ca be
//                               NULL
//
// .. c:function::
extern
void
ch_chirp_send(ch_chirp_t* chirp, ch_message_t* msg, ch_send_cb_t send_cb);
//
//    Send a message. Messages can be sent in parallel to different nodes.
//    Messages to the same node will block, if you don't wait for the callback.
//
//    If you don't want to allocate messages on sending, we recommend to use a
//    pool of messages.
//
//    :param ch_chirp_t* chirp: Chirp instance
//    :param ch_message_t msg: The message to send. The memory of the message
//                             must stay valid till the callback is called.
//    :param ch_send_cb_t send_cb: Will be call
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
