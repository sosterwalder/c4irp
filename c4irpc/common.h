// =============
// Common header
// =============
// .. code-block:: cpp
//
#ifndef ch_common_h
#define ch_common_h

#include "../include/error.h"

#include <uv.h>

#include <assert.h>
//
// .. code-block:: cpp
//
// .. c:function::
static
inline
ch_error_t
_ch_uv_error_map(int error)
//
//    Map common libuv errors to c4irp errors.
//
//    :param int error: Libuv error
//    :rtype: ch_error_t
//
// .. code-block:: cpp
//
{
    switch(error) {
        case(0):
            return CH_SUCCESS;
        case(UV_EADDRINUSE):
            return CH_EADDRINUSE;
        case(UV_EINVAL):
            return CH_VALUE_ERROR;
        default:
            return CH_UV_ERROR;
    }
}
//
// .. code-block:: cpp

#define CH_CHIRP_MAGIC 42429

#endif //ch_common_h
