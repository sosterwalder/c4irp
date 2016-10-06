// ==========
// Encryption
// ==========
//
// .. code-block:: cpp
//
#include "encryption.h"
#include "chirp.h"

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
    const SSL_METHOD* method = TLSv1_2_method();
    if(method == NULL) {
        L(
            chirp,
            "Error: Could not get the TLSv1_2_method. ch_chirp_t:%p",
            chirp
        );
        return CH_TLS_ERROR;
    }
    enc->ssl_ctx = SSL_CTX_new(method);
    if(enc->ssl_ctx == NULL) {
        L(
            chirp,
            "Error: Could create the SSL_CTX. ch_chirp_t:%p",
            chirp
        );
        return CH_TLS_ERROR;
    }
    SSL_CTX_set_verify(
            enc->ssl_ctx,
            SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
            NULL
    );
    SSL_CTX_set_verify_depth(enc->ssl_ctx, 5);
    if(SSL_CTX_load_verify_locations(
                enc->ssl_ctx,
                ichirp->config.CERT_CHAIN_PEM,
                NULL
    ) != 1) {
        L(
            chirp,
            "Error: Could not set the verification certificate "
            "%s. ch_chirp_t:%p",
            ichirp->config.CERT_CHAIN_PEM,
            chirp
        );
        return CH_TLS_ERROR;
    }
    if(SSL_CTX_use_certificate_chain_file(
                enc->ssl_ctx,
                ichirp->config.CERT_CHAIN_PEM
    ) != 1) {
        L(
            chirp,
            "Error: Could not set the certificate %s. ch_chirp_t:%p",
            ichirp->config.CERT_CHAIN_PEM,
            chirp
        );
        return CH_TLS_ERROR;
    }
    if(SSL_CTX_set_cipher_list(
            enc->ssl_ctx,
            "-ALL:"
            "TLS_RSA_WITH_AES_256_CBC_SHA256:"
            "TLS_DH_RSA_WITH_AES_256_CBC_SHA256:"
            "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384:"
            "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384:"
            "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384:"
            "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384:"
    ) != 1) {
        L(
            chirp,
            "Error: Could not set the cipher list. ch_chirp_t:%p",
            chirp
        );
        return CH_TLS_ERROR;
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
    SSL_CTX_free(enc->ssl_ctx);
    return CH_SUCCESS;
}
