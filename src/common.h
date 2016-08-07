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
#include <stddef.h>
#include <stdio.h>
#ifdef _WIN32
#if defined(_MSC_VER) && _MSC_VER < 1600
#include "stdint-msvc2008.h"
#define inline __inline
#else // _MSCVER
#include <stdint.h>
#endif // _WIN32
#endif

// Logging and assert macros
// =========================
//
// The logging macro L(chirp, message, ...) behaves like printf and allows to
// log to a custom callback. Usually used to log into pythons logging facility.
//
// The assert macro A(condition, message, ...) behaves like printf and allows to
// print a message with the assertion
//
// .. code-block:: cpp

#ifndef NDEBUG
#ifdef LOG_TO_STDERR
#define L(chirp, ...) fprintf(stderr, ##__VA_ARGS__)
#else  //LOG_TO_STDERR
#include "../include/chirp_obj.h"
#define L(chirp, message, ...) do { \
    if(chirp->_log != NULL) { \
        char buf[1024]; \
        snprintf( \
            buf, \
            1024, \
            "%s:%d " message, \
            __FILE__, \
            __LINE__, \
            ##__VA_ARGS__ \
        ); \
        chirp->_log(buf); \
    } \
} while(0)
#endif
#define A(condition, ...) do { \
    if(!(condition)) { \
        fprintf(stderr, ##__VA_ARGS__); \
        fprintf(stderr, "\n"); \
        assert(condition); \
    } \
} while(0)
#else //NDEBUG
#define L(chrip, message, ...) (void)(chirp); (void)(message)
#define A(condition, ...) (void)(condition)
#endif

#ifndef A
#error Assert macro not defined
#endif
#ifndef L
#error Log macro not defined
#endif

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
//    Fill in random ints efficiently len MUST be multiple of four.
//
//    Thank you windows for making this really complicated.
//
//    :param char* bytes: The buffer to fill the bytes into
//    :param size_t  len: The length of the buffer
//
// .. code-block:: cpp
//
{
    A(len % 4 == 0, "len must be multiple of four");
#ifdef _WIN32
#if RAND_MAX < 16384 || INT_MAX < 16384 // 2**14
#error Seriously broken compiler or platform
#else // RAND_MAX < 16384 || INT_MAX < 16384
    int tmp_rand;
    for(size_t i = 0; i < len; i += 2) {
        tmp_rand = rand();
        memcpy(bytes + i, &tmp_rand, 2);
    }
#endif // RAND_MAX < 16384 || INT_MAX < 16384
#else // _WIN32
#if RAND_MAX < 1073741824 || INT_MAX < 1073741824 // 2**30
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
#endif // _WIN32
}
//
// .. code-block:: cpp

#define CH_CHIRP_MAGIC 42429

#define CH_GET_CHIRP(handle) \
ch_chirp_t* chirp = (ch_chirp_t*) handle->data; \
A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*")
/* Fail fast, best I know :( */

#endif //ch_common_h
