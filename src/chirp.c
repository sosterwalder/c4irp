// =====
// Chirp
// =====
//
// .. code-block:: cpp
//
#include "chirp.h"
#include "util.h"

#include <openssl/err.h>

#include <time.h>
#include <signal.h>
#ifdef _WIN32
#   define ch_access _access
#   include <io.h>
#else
#   define ch_access access
#   include <unistd.h>
#endif

// Sglib Prototypes
// ================

// .. code-block:: cpp
//
SGLIB_DEFINE_RBTREE_FUNCTIONS( // NOCOV
    ch_chirp_t,
    _left,
    _right,
    _color_field,
    SGLIB_NUMERIC_COMPARATOR
)

// Declarations
// ============

// .. c:var:: ch_config_t ch_config_defaults
//
//    Default config of chirp.
//
// .. code-block:: cpp
//
static ch_config_t _ch_config_defaults = {
    .REUSE_TIME      = 30,
    .TIMEOUT         = 5,
    .PORT            = 2998,
    .BACKLOG         = 100,
    .RETRIES         = 1,
    .MAX_HANDLERS    = 16,
    .FLOW_CONTROL    = 1,
    .ACKNOWLEDGE     = 1,
    .CLOSE_ON_SIGINT = 1,
    .BUFFER_SIZE     = 0,
    .BIND_V6         = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4         = {0, 0, 0, 0},
    .IDENTITY        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .CERT_CHAIN_PEM  = NULL,
    .DH_PARAMS_PEM   = NULL,
};

// .. c:var:: int _ch_chirp_ref_count
//
//    Chirp reference counter
//
// .. code-block:: cpp
//
static int _ch_chirp_ref_count = 0;

// .. c:var:: int _ch_chirp_sig_init
//
//    Signal handler was initialized
//
// .. code-block:: cpp
//
static char _ch_chirp_sig_init = 0;

// .. c:var:: ch_chirp_t** _ch_chirp_instances
//
//    Signal handler was initialized
//
// .. code-block:: cpp
//
static ch_chirp_t* _ch_chirp_instances = NULL;

// .. c:function::
static
void
_ch_chirp_check_closing_cb(uv_prepare_t* handle);
//
//    Close chirp when the closing semaphore reaches zero.
//
//    :param uv_prepare_t* handle: Prepare handle which will be stopped (and
//                                 thus its callback)
//

// .. c:function::
static
void
_ch_chirp_close_async_cb(uv_async_t* handle);
//
//    Internal callback to close chirp. Makes ch_chirp_close_ts thread-safe
//
//    :param uv_async_t* handle: Async handle which is used to closed chirp
//

// .. c:function::
static
void
_ch_chirp_closing_down_cb(uv_handle_t* handle);
//
//    Close chirp after the check callback has been closed and stops the libuv
//    loop.
//
//    :param uv_handle_t* handle: Base libuv handle which contains chirp (as
//                                data)
//

// .. c:function::
static
void
_ch_chirp_sig_handler(int);
//
//    Closes all chirp instances on sig int.
//

// .. c:function::
static
ch_error_t
_ch_chirp_verify_cfg(const ch_chirp_t* chirp);
//
//   Verifies the configuration.
//
//   :param   ch_chirp_t* chrip: Instance of a chirp object
//
//   :return: A chirp error. see: :c:type:`ch_error_t`
//   :rtype:  ch_error_t
//

// Definitions
// ===========

// .. c:function::
static
void
_ch_chirp_check_closing_cb(uv_prepare_t* handle)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close_cb`
//
// .. code-block:: cpp
//
{
    CH_GET_CHIRP(handle);
    ch_chirp_int_t* ichirp = chirp->_;
    A(ichirp->closing_tasks > -1, "Closing semaphore dropped below zero");
    L(
        chirp,
        "Check closing semaphore (%d). ch_chirp_t:%p",
        ichirp->closing_tasks,
        (void*) chirp
    );
    // In production we allow the semaphore to drop below zero but log it as an
    // error
    if(ichirp->closing_tasks < 1) {
        assert(uv_prepare_stop(handle) == CH_SUCCESS);
        assert(ch_en_stop(&ichirp->encryption) == CH_SUCCESS);
        uv_close((uv_handle_t*) handle, _ch_chirp_closing_down_cb);
    }
    if(ichirp->closing_tasks < 0) {
        E(
            chirp,
            "Check closing semaphore dropped blow 0. ch_chirp_t:%p",
            (void*) chirp
        );
    }
}

// .. c:function::
void
ch_chirp_config_init(ch_config_t* config)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close_cb`
//
// .. code-block:: cpp
//
{
    *config = _ch_config_defaults;
}

// .. c:function::
static
void
_ch_chirp_close_async_cb(uv_async_t* handle)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_close_async_cb`
//
// .. code-block:: cpp
//
{
    CH_GET_CHIRP(handle);
    if(chirp->_ == NULL) {
        E(
            chirp,
            "Chirp closing callback called on closed. ch_chirp_t:%p",
            (void*) chirp
        );
        return;
    }
    ch_chirp_int_t* ichirp = chirp->_;
    if(ichirp->flags & CH_CHIRP_CLOSED) {
        E(
            chirp,
            "Chirp closing callback called on closed. ch_chirp_t:%p",
            (void*) chirp
        );
        return;
    }
    L(
        chirp,
        "Chirp closing callback called. ch_chirp_t:%p",
        (void*) chirp
    );
    assert(ch_pr_stop(&ichirp->protocol) == CH_SUCCESS);
    uv_close((uv_handle_t*) &ichirp->close, ch_chirp_close_cb);
    ichirp->closing_tasks += 1;
    assert(uv_prepare_init(ichirp->loop, &ichirp->close_check) == CH_SUCCESS);
    ichirp->close_check.data = chirp;
    // We use a semaphore to wait until all callbacks are done:
    // 1. Every time a new callback is scheduled we do
    //    ichirp->closing_tasks += 1
    // 2. Every time a callback is called we do ichirp->closing_tasks -= 1
    // 3. Every uv_loop iteration before it blocks we check
    //    ichirp->closing_tasks == 0
    // -> if we reach 0 all callbacks are done and we continue freeing memory
    // etc.
    assert(uv_prepare_start(
        &ichirp->close_check,
        _ch_chirp_check_closing_cb
    ) == CH_SUCCESS);
}

// .. c:function::
void
ch_chirp_close_cb(uv_handle_t* handle)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close_cb`
//
// .. code-block:: cpp
//
{
    CH_GET_CHIRP(handle);
    chirp->_->closing_tasks -= 1;
    L(
        chirp,
        "Closing semaphore (%d). uv_handle_t:%p, ch_chirp_t:%p",
        chirp->_->closing_tasks,
        (void*) handle,
        (void*) chirp
    );
}

// .. c:function::
ch_error_t
ch_chirp_close_ts(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close_ts`
//
//    This function is thread-safe.
//
// .. code-block:: cpp
//
{
    char chirp_closed = 0;
    ch_chirp_int_t* ichirp;
    if(chirp == NULL || chirp->_init != CH_CHIRP_MAGIC) {
        fprintf(
            stderr,
            "%s:%d Fatal: chirp is not initialzed. ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            (void*) chirp
        );
        return CH_UNINIT; // NOCOV  TODO can be tested
    }
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    if(chirp->_ != NULL) {
        ichirp = chirp->_;
        if(ichirp->flags & CH_CHIRP_CLOSED)
            chirp_closed = 1;
    } else
        chirp_closed = 1;
    if(chirp_closed) {
        fprintf(
            stderr,
            "%s:%d Fatal: chirp is already closed. ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            (void*) chirp
        );
        return CH_FATAL;
    }
    if(ichirp->flags & CH_CHIRP_CLOSING) {
        E(
            chirp,
            "Close already in progress. ch_chirp_t:%p",
            (void*) chirp
        );
        return CH_IN_PRORESS;
    }
    ichirp->flags |= CH_CHIRP_CLOSING;
    ichirp->close.data = chirp;
    L(chirp, "Closing chirp via callback. ch_chirp_t:%p", (void*) chirp);
    if(uv_async_send(&ichirp->close) < 0) {
        E(
            chirp,
            "Could not call close callback. ch_chirp_t:%p",
            (void*) chirp
        );
        return CH_UV_ERROR; // NOCOV only breaking things will trigger this
    }
    return CH_SUCCESS;
}

// .. c:function::
static
void
_ch_chirp_closing_down_cb(uv_handle_t* handle)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_closing_down_cb`
//
// .. code-block:: cpp
//
{
    CH_GET_CHIRP(handle);
    ch_chirp_int_t* ichirp = chirp->_;
    if(ichirp->flags & CH_CHIRP_AUTO_STOP) {
        uv_stop(ichirp->loop);
        L(
            chirp,
            "UV-Loop stopped by chirp. uv_loop_t:%p ch_chirp_t:%p",
            (void*) ichirp->loop,
            (void*) chirp
        );
    }
    chirp->_ = NULL;
    ch_free(ichirp);
    L(chirp, "Closed. ch_chirp_t:%p", (void*) chirp);
    if(sglib_ch_chirp_t_is_member(_ch_chirp_instances, chirp))
        sglib_ch_chirp_t_delete(&_ch_chirp_instances, chirp);
    else {
        E(
            chirp,
            "Closing unknown chirp instance. ch_chirp_t:%p",
            (void*) chirp
        );
    }
    _ch_chirp_ref_count -= 1;
    A(chirp, "Chirp reference count dropped below 0");
}

// .. c:function::
ch_identity_t
ch_chirp_get_identity(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_closing_down_cb`
//
// .. code-block:: cpp
//
{
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_identity_t id;
    memcpy(id.data, chirp->_->identity, sizeof(id.data));
    return id;
}

// .. c:function::
uv_loop_t*
ch_chirp_get_loop(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_closing_down_cb`
//
// .. code-block:: cpp
//
{
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    return chirp->_->loop;
}

// .. c:function::
ch_error_t
ch_chirp_init(
        ch_chirp_t* chirp,
        const ch_config_t* config,
        uv_loop_t* loop,
        ch_log_cb_t log_cb
)
//    :noindex:
//
//    see: :c:func:`ch_chirp_init`
//
// .. code-block:: cpp
//
{
    int tmp_err;
    memset(chirp, 0, sizeof(ch_chirp_t));
    chirp->_init            = CH_CHIRP_MAGIC;
    ch_chirp_int_t* ichirp  = ch_alloc(sizeof(ch_chirp_int_t));
    memset(ichirp, 0, sizeof(ch_chirp_int_t));
    ichirp->config          = *config;
    ichirp->public_port     = config->PORT;
    ichirp->loop            = loop;
    ch_config_t* tmp_conf   = &ichirp->config;
    ch_protocol_t* protocol = &ichirp->protocol;
    ch_encryption_t* enc    = &ichirp->encryption;
    chirp->_                = ichirp;
    if(log_cb != NULL)
        ch_chirp_register_log_cb(chirp, log_cb);
    tmp_err = _ch_chirp_verify_cfg(chirp);
    if(tmp_err != CH_SUCCESS) {
        chirp->_init = 0;
        return tmp_err;
    }

    // rand
    srand((unsigned int) time(NULL));
    unsigned int i = 0;
    while(
            i < (sizeof(tmp_conf->IDENTITY) - 1) &&
            tmp_conf->IDENTITY[i] == 0
    ) i += 1;
    if(tmp_conf->IDENTITY[i] == 0)
        ch_random_ints_as_bytes(ichirp->identity, sizeof(ichirp->identity));
    else
        *ichirp->identity = *tmp_conf->IDENTITY;


    if(uv_async_init(loop, &ichirp->close, _ch_chirp_close_async_cb) < 0) {
        E(
            chirp,
            "Could not initialize close callback. ch_chirp_t:%p",
            (void*) chirp
        );
        ch_free(ichirp);
        chirp->_init = 0;
        return CH_UV_ERROR; // NOCOV
    }

    ch_pr_init(chirp, protocol);
    tmp_err = ch_pr_start(protocol);
    if(tmp_err != CH_SUCCESS) {
        E(
            chirp,
            "Could not start protocol: %d. ch_chirp_t:%p",
            tmp_err,
            (void*) chirp
        );
        ch_free(ichirp);
        chirp->_init = 0;
        return tmp_err;
    }
    ch_en_init(chirp, enc);
    tmp_err = ch_en_start(enc);
    if(tmp_err != CH_SUCCESS) {
#       ifndef NDEBUG
            ERR_print_errors_fp(stderr);
#       endif
        E(
            chirp,
            "Could not start encryption: %d. ch_chirp_t:%p",
            tmp_err,
            (void*) chirp
        );
        ch_free(ichirp);
        chirp->_init = 0;
        return tmp_err;
    }
#   ifndef NDEBUG
    char id_str[33];
    ch_bytes_to_hex(
        ichirp->identity,
        sizeof(ichirp->identity),
        id_str,
        sizeof(id_str)
    );
    L(
        chirp,
        "Chirp initialized id: %s. ch_chirp_t:%p, uv_loop_t:%p",
        id_str,
        (void*) chirp,
        (void*) loop
    );
#   endif
    if(!_ch_chirp_sig_init) {
       if(signal(SIGINT, _ch_chirp_sig_handler) == SIG_ERR) {
            E(
                chirp,
                "Unable to set SIGINT handler. ch_chirp_t:%p",
                (void*) chirp
            );
       }
       else
           _ch_chirp_sig_init = 1; // We need at least sigint
       if(signal(SIGTERM, _ch_chirp_sig_handler) == SIG_ERR) {
            E(
                chirp,
                "Unable to set SIGTERM handler. ch_chirp_t:%p",
                (void*) chirp
            );
       }
    }
    _ch_chirp_ref_count += 1;
    sglib_ch_chirp_t_add(&_ch_chirp_instances, chirp);
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_run(const ch_config_t* config, ch_chirp_t** chirp_out)
//    :noindex:
//
//    see: :c:func:`ch_chirp_run`
//
// .. code-block:: cpp
//
{
    ch_chirp_t chirp;
    uv_loop_t  loop;
    ch_error_t tmp_err;
    if(chirp_out == NULL) {
        return CH_UNINIT; // NOCOV  TODO can be tested
    }
    *chirp_out = NULL;

    tmp_err = ch_uv_error_map(ch_loop_init(&loop));
    if(tmp_err != CH_SUCCESS) {
        E(
            (&chirp),
            "Could not init loop: %d. uv_loop_t:%p",
            tmp_err,
            (void*) &loop
        );
        return tmp_err;  // NOCOV this can only fail with access error
    }
    tmp_err = ch_chirp_init(&chirp, config, &loop, NULL);
    if(tmp_err != CH_SUCCESS) {
        E(
            (&chirp),
            "Could not init chirp: %d ch_chirp_t:%p",
            tmp_err,
            (void*) &chirp
        );
        return tmp_err;  // NOCOV covered in ch_chirp_init tests
    }
    chirp._->flags |= CH_CHIRP_AUTO_STOP;
    L(
        (&chirp),
        "UV-Loop run by chirp. ch_chirp_t:%p, uv_loop_t:%p",
        (void*) &chirp,
        (void*) &loop
    );
    /* This works and is not TOO bad because the function blocks. */
    // cppcheck-suppress unmatchedSuppression
    // cppcheck-suppress autoVariables
    *chirp_out = &chirp;
    tmp_err = ch_run(&loop);
    *chirp_out = NULL;
    if(tmp_err != 0) {
        E(
            (&chirp),
            "uv_run returned with error: %d, uv_loop_t:%p",
            tmp_err,
            (void*) &loop
        );
        return tmp_err; // NOCOV only breaking things will trigger this
    }
    if(ch_loop_close(&loop)) {
        return CH_UV_ERROR; // NOCOV only breaking things will trigger this
    }
    return CH_SUCCESS;
}

// .. c:function::
void
ch_chirp_set_auto_stop(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_set_auto_stop`
//
//    This function is thread-safe
//
// .. code-block:: cpp
//
{
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    chirp->_->flags |= CH_CHIRP_AUTO_STOP;
}

// .. c:function::
static
void
_ch_chirp_sig_handler(int signo)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_sig_handler`
//
// .. code-block:: cpp
//
{
    ch_chirp_t* t;
    if(signo != SIGINT && signo != SIGTERM)
        return;
    struct sglib_ch_chirp_t_iterator it;
    for(
            t = sglib_ch_chirp_t_it_init(
                &it,
                _ch_chirp_instances
            );
            t != NULL;
            t = sglib_ch_chirp_t_it_next(&it)
    ) {
        if(t->_->config.CLOSE_ON_SIGINT)
            ch_chirp_close_ts(t);
    }
}

// .. c:function::
static
ch_error_t
_ch_chirp_verify_cfg(const ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_sig_handler`
//
// .. code-block:: cpp
//
{
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_config_t* conf = &chirp->_->config;
    VE(
        chirp,
        conf->DH_PARAMS_PEM != NULL,
        "Config: DH_PARAMS_PEM must be set."
    );
    VE(
        chirp,
        conf->CERT_CHAIN_PEM != NULL,
        "Config: CERT_CHAIN_PEM must be set."
    );
    V(
        chirp,
        ch_access(conf->CERT_CHAIN_PEM, F_OK ) != -1,
        "Config: cert %s does not exist.",
        conf->CERT_CHAIN_PEM
    );
    V(
        chirp,
        conf->PORT > 1024,
        "Config: port must be > 1042. (%d)",
        conf->PORT
    );
    V(
        chirp,
        conf->BACKLOG < 128,
        "Config: backlog must be < 128. (%d)",
        conf->BACKLOG
    );
    V(
        chirp,
        conf->TIMEOUT <= 60,
        "Config: timeout must be <= 60. (%f)",
        conf->TIMEOUT
    );
    V(
        chirp,
        conf->TIMEOUT >= 0.1,
        "Config: timeout must be >= 0.1. (%f)",
        conf->TIMEOUT
    );
    V(
        chirp,
        conf->REUSE_TIME >= 2,
        "Config: resuse time must be => 2. (%f)",
        conf->REUSE_TIME
    );
    V(
        chirp,
        conf->REUSE_TIME <= 3600,
        "Config: resuse time must be <= 3600. (%f)",
        conf->REUSE_TIME
    );
    V(
        chirp,
        conf->TIMEOUT <= conf->REUSE_TIME,
        "Config: timeout must be <= reuse time. (%f, %f)",
        conf->TIMEOUT,
        conf->REUSE_TIME
    );
    if(conf->FLOW_CONTROL) {
        VE(
            chirp,
            conf->MAX_HANDLERS >= 16,
            "Config: if flow control is on max_handlers must be >= 16."
        );
    } else {
        VE(
            chirp,
            conf->MAX_HANDLERS >= 1,
            "Config: max_handlers must be >= 1."
        );
    }
    if(conf->ACKNOWLEDGE == 0) {
        VE(
            chirp,
            conf->RETRIES != 0,
            "Config: if acknowledge is disabled retries has to be 0."
        );
        VE(
            chirp,
            conf->FLOW_CONTROL != 0,
            "Config: if acknowledge is disabled flow-control has to be 0."
        );
    }
    VE(
        chirp,
        conf->MAX_HANDLERS <= 32,
        "Config: max_handlers must be <= 1."
    );
    V(
        chirp,
        conf->BUFFER_SIZE >= CH_LIB_UV_MIN_BUFFER || conf->BUFFER_SIZE == 0,
        "Config: buffer size must be > %d (%d)",
        CH_LIB_UV_MIN_BUFFER,
        conf->BUFFER_SIZE
    );
    V(
        chirp,
        conf->BUFFER_SIZE >= sizeof(ch_message_t) || conf->BUFFER_SIZE == 0,
        "Config: buffer size must be > %d (%d)",
        (int) sizeof(ch_message_t),
        conf->BUFFER_SIZE
    );
    V(
        chirp,
        conf->BUFFER_SIZE >= sizeof(
            ch_rd_handshake_t
        ) || conf->BUFFER_SIZE == 0,
        "Config: buffer size must be > %d (%d)",
        (int) sizeof(ch_rd_handshake_t),
        conf->BUFFER_SIZE
    );
    return CH_SUCCESS;
}
