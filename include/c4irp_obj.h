// ================
// Chirp Obj Header
// ================
//
// .. code-block:: cpp

#ifndef ch_inc_c4irp_obj_h
#define ch_inc_c4irp_obj_h

#include "error.h"

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
