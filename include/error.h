// ======
// Errors
// ======
//
// .. code-block:: cpp

#ifndef ch_inc_error_h
#define ch_inc_error_h

// .. c:type:: ch_error_t
//
//    Represents a chirp error.
//
//    .. c:member:: CH_SUCCESS
//
//       No error
//
//    .. c:member:: CH_VALUE_ERROR
//
//       Supplied value is not allowed
//
//    .. c:member:: CH_UV_ERROR
//
//       General libuv error
//
//    .. c:member:: CH_PROTOCOL_ERROR
//
//       Happens when bad values are received or remote dies unexpectedly
//
//    .. c:member:: CH_EADDRINUSE
//
//       The chirp port was already in use
//
// .. code-block:: cpp
//
typedef enum {
    CH_SUCCESS        = 0,
    CH_VALUE_ERROR    = 1,
    CH_UV_ERROR       = 2,
    CH_PROTOCOL_ERROR = 3,
    CH_EADDRINUSE     = 4,
} ch_error_t;

#endif //ch_inc_error_h
