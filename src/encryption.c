// ==========
// Encryption
// ==========
//
// .. code-block:: cpp
//
#include "encryption.h"

#include <openssl/ssl.h>
#include <openssl/crypto.h>

// .. c:var:: _ch_en_manual_openssl
//
//    The user will call ch_en_openssl_init() and ch_en_openssl_uninit().
//    Defaults to 0.
//
// .. code-block:: cpp
//
static char _ch_en_manual_openssl = 0;

// .. c:function::
void
ch_en_set_manual_openssl_init()
//    :noindex:
//
//    see: :c:func:`ch_en_set_manual_openssl_init`
//
// .. code-block:: cpp
//
{
    _ch_en_manual_openssl = 0;
}

// .. c:function::
ch_error_t
ch_en_start(ch_encryption_t* enc)
//    :noindex:
//
//    see: :c:func:`ch_en_start`
//
// .. code-block:: cpp
//
{ 
    SSL_library_init(); // No useful return value
#ifndef NDEBUG
    SSL_load_error_strings();
#endif

    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_en_stop(ch_encryption_t* enc)
//    :noindex:
//
//    see: :c:func:`ch_en_stop`
//
// .. code-block:: cpp
//
{
    return CH_SUCCESS;
}
