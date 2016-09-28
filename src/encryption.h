// =================
// Encryption Header
// =================
//
// .. code-block:: cpp

#ifndef ch_encryption_h
#define ch_encryption_h

#include "../include/chirp_obj.h"

// .. c:type:: ch_encryption_t
//
//    Encryption object.
//
//    .. c:member:: struct ch_chirp_s*
//
//       reference back to chirp
//
// .. code-block:: cpp

typedef struct ch_encryption_s {
    ch_chirp_t* chirp;
} ch_encryption_t;

// .. c:var:: _ch_en_manual_openssl
//
//    The user will call ch_en_openssl_init() and ch_en_openssl_uninit().
//    Defaults to 0.
//
// .. code-block:: cpp
//
static char _ch_en_manual_openssl;

// .. c:function::
static
void
_cn_en_free(void* buf, const char* file, int line);
//
//    Free callback for OpenSSL.
//
//    :param void* buf: Buffer to free
//    :param const char* file: File the allocation came from
//    :param int line: The line the allocation came from
// .. c:function::
static
ch_inline
void
ch_en_init(ch_chirp_t* chirp, ch_encryption_t* enc)
//
//    Initialize the encryption struct.
//
//    :param ch_chirp_t* chirp: Chirp instance
//    :param ch_encryption_t* enc: Encryption to initialize
//
// .. code-block:: cpp
//
{
    memset(enc, 0, sizeof(ch_encryption_t));
    enc->chirp = chirp;
}

// .. c:function::
static
void*
_cn_en_malloc(size_t size, const char* file, int line);
//
//    Malloc callback for OpenSSL.
//
//    :param size_t size: Size to allocate
//    :param const char* file: File the allocation came from
//    :param int line: The line the allocation came from
//
// .. c:function::
static
void*
_cn_en_realloc(void* buf, size_t new_size, const char* file, int line);
//
//    Realloc callback for OpenSSL.
//
//    :param void* buf: Buffer to reallocate
//    :param size_t size: Size to allocate
//    :param const char* file: File the allocation came from
//    :param int line: The line the allocation came from

// .. c:function::
ch_error_t
ch_en_start(ch_encryption_t* enc);
//
//    Start the encryption
//
//    TODO params
//
// .. c:function::
ch_error_t
ch_en_stop(ch_encryption_t* enc);
//
//    Stop the encryption
//
//    TODO params
//
// .. code-block:: cpp
#endif //ch_encryption_h
