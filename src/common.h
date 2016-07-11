// =============
// Common header
// =============
// .. code-block:: cpp
//
#ifndef ch_common_h
#define ch_common_h

#include "../include/error.h"
#include "../config.h"

#include <uv.h>

#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
//
// .. c:function::
static
inline
ch_error_t
_ch_uv_error_map(int error)
//
//    Map common libuv errors to chirp errors.
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
// .. c:function::
static
void
_ch_random_ints_to_bytes(unsigned char* bytes, size_t len)
//
//    Fill in random ints efficiently len MUST be multiple of four
//
//    :param char* bytes: The buffer to fill the bytes into
//    :param size_t  len: The length of the buffer
//
// .. code-block:: cpp
//
{
    A(len % 4 == 0, "len must be multiple of four");
#if RAND_MAX < 1073741824 || INT_MAX < 1073741824
#ifdef CH_ACCEPT_STRANGE_PLATFORM
    /* WTF, fallback platform */
    for(size_t i = 0; i < len; i++) {
        bytes[i] = ((unsigned int) rand()) % 256;
    }
#else // ACCEPT_STRANGE_PLATFORM
#error Unexpected RAND_MAX / INT_MAX, define CH_ACCEPT_STRANGE_PLATFORM
#endif // ACCEPT_STRANGE_PLATFORM
#else // RAND_MAX < 1073741824 || INT_MAX < 1073741824
    /* Tested: this is 4 times faster*/
    int tmp_rand;
    for(size_t i = 0; i < len; i += 4) {
        tmp_rand = rand();
        memcpy(bytes + i, &tmp_rand, 4);
    }
#endif // RAND_MAX < 1073741824 || INT_MAX < 1073741824
}
//
// .. code-block:: cpp

#define CH_CHIRP_MAGIC 42429

#define CH_GET_CHIRP(handle) \
ch_chirp_t* chirp = (ch_chirp_t*) handle->data; \
A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*")
// Fail fast, best I know :(


#endif //ch_common_h
