// ==========
// Encryption
// ==========
//
// .. code-block:: cpp
//
#include "encryption.h"
#include "chirp.h"

#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/conf.h>

// .. c:var:: _ch_en_manual_openssl
//
//    The user will call ch_en_openssl_init() and ch_en_openssl_uninit().
//    Defaults to 0.
//
// .. code-block:: cpp
//
static char _ch_en_manual_openssl = 0;

// .. c:var:: _ch_en_openssl_ref_count
//
//    Counts how many chirp instances are using openssl.
//
// .. code-block:: cpp
//
static int _ch_en_openssl_ref_count = 0;

// .. c:function::
ch_error_t
ch_en_openssl_init(const ch_config_t* config)
//    :noindex:
//
//    see: :c:func:`ch_en_openssl_init`
//
// .. code-block:: cpp
//
{
    SSL_library_init();
    SSL_load_error_strings();
    OPENSSL_config("chirp");

    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_en_openssl_uninit(void)
//    :noindex:
//
//    see: :c:func:`ch_en_openssl_uninit`
//
// .. code-block:: cpp
//
{
    return CH_SUCCESS;
    FIPS_mode_set(0);
    ENGINE_cleanup();
    CONF_modules_unload(1);
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
    return CH_SUCCESS;
}

// .. c:function::
void
ch_en_set_manual_openssl_init(void)
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
    ch_chirp_t* chirp = enc->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    if(!_ch_en_manual_openssl) {
        _ch_en_openssl_ref_count += 1;
        L(
            chirp,
            "Initializing the OpenSSL library. ch_chirp_t:%p",
            chirp
        );
        return ch_en_openssl_init(&ichirp->config);
    }
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
    ch_chirp_t* chirp = enc->chirp;
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    if(!_ch_en_manual_openssl) {
        _ch_en_openssl_ref_count -= 1;
        if(_ch_en_openssl_ref_count == 0) {
            L(
                chirp,
                "Uninitializing the OpenSSL library. ch_chirp_t:%p",
                chirp
            );
            return ch_en_openssl_uninit();
        }
    }
    return CH_SUCCESS;
}
