// ===========
// Util Header
// ===========
//
// Common utility functions.
//
// .. code-block:: cpp
//
#ifndef ch_util_h
#define ch_util_h

#include "../include/callbacks.h"
#include "../include/common.h"

ch_alloc_cb_t _ch_alloc_cb;
ch_free_cb_t _ch_free_cb;
ch_realloc_cb_t _ch_realloc_cb;

// .. c:function::
static
ch_inline
void*
ch_alloc(size_t size)
//
//    Allocate fixed amount of memory.
//
//    :param size_t size: The amount of memory in bytes to allocate
//
// .. code-block:: cpp
//
{
    return _ch_alloc_cb(size);
}
// .. c:function::
static
ch_inline
void
ch_bytes_to_hex(uint8_t* bytes, size_t bytes_size, char* str, size_t str_size)
//
//    Convert a bytes array to a hex string
//
//    :param uint8_t* bytes: Bytes to convert
//    :param size_t bytes_size: Length of the bytes to convert
//    :param char* str: Destination string
//    :param size_t str_size: Length of the buffer to write the string to
//
// .. code-block:: cpp
//
{
    A(bytes_size * 2 + 1 <= str_size, "Not enough space for string");
    for(size_t i = 0; i < bytes_size; i++)
    {
            str += snprintf(str, 2, "%02X", bytes[i]);
    }
    *str = 0;
}

// .. c:function::
static
ch_inline
int
ch_msb32(uint32_t x)
//
//    Get the most significant bit set
//
//    :param uint32_t x: The bit set
//
// .. code-block:: cpp
//
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return(x & ~(x >> 1));
}

// .. c:function::
static
ch_inline
void
ch_free(
        void* buf
)
//
//    Free a memory handle.
//
//    :param void* buf: The handle to free
//
// .. code-block:: cpp
//
{
    _ch_free_cb(buf);
}

// .. c:function::
static
ch_inline
void
ch_random_ints_as_bytes(uint8_t* bytes, size_t len)
//
//    Fill in random ints efficiently len MUST be multiple of four.
//
//    Thank you windows for making this really complicated.
//
//    :param uint8_t* bytes: The buffer to fill the bytes into
//    :param size_t  len: The length of the buffer
//
// .. code-block:: cpp
//
{
    A(len % 4 == 0, "len must be multiple of four");
#ifdef _WIN32
#   if RAND_MAX < 16384 || INT_MAX < 16384 // 2**14
#       error Seriously broken compiler or platform
#   else // RAND_MAX < 16384 || INT_MAX < 16384
        int tmp_rand;
        for(size_t i = 0; i < len; i += 2) {
            tmp_rand = rand();
            memcpy(bytes + i, &tmp_rand, 2);
        }
#   endif // RAND_MAX < 16384 || INT_MAX < 16384
#else // _WIN32
#   if RAND_MAX < 1073741824 || INT_MAX < 1073741824 // 2**30
#       ifdef CH_ACCEPT_STRANGE_PLATFORM
            /* WTF, fallback platform */
            for(size_t i = 0; i < len; i++) {
                bytes[i] = ((unsigned int) rand()) % 256;
            }
#       else // ACCEPT_STRANGE_PLATFORM
            // cppcheck-suppress preprocessorErrorDirective
#           error Unexpected RAND_MAX / INT_MAX, define \
                CH_ACCEPT_STRANGE_PLATFORM
#       endif // ACCEPT_STRANGE_PLATFORM
#   else // RAND_MAX < 1073741824 || INT_MAX < 1073741824
        /* Tested: this is 4 times faster*/
        int tmp_rand;
        for(size_t i = 0; i < len; i += 4) {
            tmp_rand = rand();
            memcpy(bytes + i, &tmp_rand, 4);
        }
#   endif // RAND_MAX < 1073741824 || INT_MAX < 1073741824
#endif // _WIN32
}
//
// .. c:function::
static
ch_inline
void*
ch_realloc(
        void*  buf,
        size_t size
)
//
//    Resize allocated memory.
//
//    :param void* buf: The handle to resize
//    :param size_t size: The new size of the memory in bytes
//
// .. code-block:: cpp
//
{
    return _ch_realloc_cb(buf, size);
}

// .. c:function::
static
ch_inline
ch_error_t
ch_uv_error_map(int error)
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
        case(UV_ENOTCONN):
        case(UV_EINVAL):
            return CH_VALUE_ERROR;
        default:
            return CH_UV_ERROR;
    }
}
//

#endif // ch_util_h
