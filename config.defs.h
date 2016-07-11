#ifndef ch_global_config_h
#define ch_global_config_h

/* Minimal buffersize we require when allocating for libuv */
#define LIB_UV_MIN_BUFFER 1024
//#define LOG_TO_STDERR

#ifndef NDEBUG
#include <stdio.h>
#ifdef LOG_TO_STDERR
#define L(chirp, ...) fprintf(stderr, ##__VA_ARGS__)
#else  //LOG_TO_STDERR
#include "include/chirp_obj.h"
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
#define L(...)
#define A(...)
#endif

#endif //ch_global_config_h
