// ================
// Chirp Obj Header
// ================
//
// .. code-block:: cpp

#ifndef ch_inc_c4irp_obj_h
#define ch_inc_c4irp_obj_h

#include "error.h"

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
    int REUSE_TIME;
    int TIMEOUT;
    int PORT;
    char BIND_V6[16];
    char BIND_V4[16];
} ch_config_t;

//
// .. c:var:: ch_config_defaults
//
//    Default config of c4irp.
//
// .. code-block:: cpp

static ch_config_t ch_config_defaults = {
    .REUSE_TIME = 30,
    .TIMEOUT    = 5,
    .PORT       = 2998,
    .BIND_V6    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// .. c:type:: ch_chirp_t
//
//    Opaque pointer to chirp object.
//  
// .. code-block:: cpp

struct ch_chirp_s;
typedef struct ch_chirp_s* ch_chirp_t;

// .. c:function::
extern
ch_error_t
ch_chirp_init(ch_chirp_t* chirp);
//
//    Intialiaze a chirp object. Memory is allocated by function. You must call
//    :c:func:`ch_chirp_free` to cleanup the object.
//
//    :param ch_chirp_t* chirp: Out: Chirp object
//
// .. c:function::
extern
ch_error_t
ch_chirp_free(ch_chirp_t chirp);
//
//    Cleanup chirp object. Will remove all callbacks. Pending outs will be
//    ignored after calling free.
//
//    :param ch_chirp_t chirp: Chirp object
//
// .. code-block:: cpp

#endif //ch_inc_c4irp_obj_h
