// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "chirp.h"
#include "util.h"

#include <time.h>
#include <signal.h>

SGLIB_DEFINE_RBTREE_FUNCTIONS( // NOCOV
    ch_chirp_t,
    _left,
    _right,
    _color_field,
    SGLIB_NUMERIC_COMPARATOR
);

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
    .CLOSE_ON_SIGINT = 1,
    .BUFFER_SIZE     = 0,
    .BIND_V6         = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4         = {0, 0, 0, 0},
    .IDENTITY        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .CERT_CHAIN_PEM  = NULL,
};

// .. c:var:: int _ch_chirp_ref_count
//
//    Chirp reference counter
//
// .. code-block:: cpp

static int _ch_chirp_ref_count = 0;

// .. c:var:: int _ch_chirp_sig_init
//
//    Signal handler was initialized
//
// .. code-block:: cpp

static char _ch_chirp_sig_init = 0;

// .. c:var:: ch_chirp_t** _ch_chirp_instances
//
//    Signal handler was initialized
//
// .. code-block:: cpp
//
static ch_chirp_t* _ch_chirp_instances = NULL;

// .. c:function::
static void
_ch_chirp_check_closing_cb(uv_prepare_t* handle);
//
//    Close chirp when the closing semaphore reaches zero.
//
//    TODO params
//
// .. c:function::
static void
_ch_chirp_close_async_cb(uv_async_t* handle);
//
//    Internal callback to close chirp. Makes ch_chirp_close_ts thread-safe
//
// .. c:function::
static void
_ch_chirp_closing_down_cb(uv_handle_t* handle);
//
//    Closing chirp after the check callback has been closed.
//
//    TODO params
//
// .. c:function::
static
void
_ch_chirp_sig_handler(int);
//
//    Closes all chirp instances on sig int.
//

// .. c:function::
static void
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
        chirp
    );
    if(ichirp->closing_tasks == 0) {
        assert(uv_prepare_stop(handle) == CH_SUCCESS);
        uv_close((uv_handle_t*) handle, _ch_chirp_closing_down_cb);
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
static void
_ch_chirp_close_async_cb(uv_async_t* handle)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_close_async_cb`
//
// .. code-block:: cpp
//
{
    CH_GET_CHIRP(handle);
    ch_chirp_int_t* ichirp = chirp->_;
    if(ichirp->flags & CH_CHIRP_CLOSED) {
        L(
            chirp,
            "Error: chirp closing callback called on closed. ch_chirp_t:%p",
            chirp
        );
        return;
    }
    L(chirp, "Chirp closing callback called. ch_chirp_t:%p", chirp);
    assert(ch_pr_stop(&ichirp->protocol) == CH_SUCCESS);
    assert(ch_en_stop(&ichirp->encryption) == CH_SUCCESS);
    uv_close((uv_handle_t*) &ichirp->close, ch_chirp_close_cb);
    ichirp->closing_tasks += 1;
    assert(uv_prepare_init(ichirp->loop, &ichirp->close_check) == CH_SUCCESS);
    ichirp->close_check.data = chirp;
    // We use a semaphore to wait until all callbacks are done:
    // 1. Every time a new callback is scheduled we do ichirp->closing_tasks += 1
    // 2. Every time a callback is called we do ichirp->closing_tasks -= 1
    // 3. Every uv_loop iteration before it blocks we check ichirp->closing_tasks == 0
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
        "Decr closing semaphore to (%d). uv_handle_t:%p, ch_chirp_t:%p",
        chirp->_->closing_tasks,
        handle,
        chirp
    );
}

// .. c:function::
ch_error_t
ch_chirp_close_ts(ch_chirp_t* chirp)
//    :noindex:
//
//    see: :c:func:`ch_chirp_close_ts`
//
//    This function is thread-safe
//
// .. code-block:: cpp
//
{
    L(chirp, "Closing chirp via callback. ch_chirp_t:%p", chirp);
    if(chirp == NULL || chirp->_init != CH_CHIRP_MAGIC) {
        fprintf(
            stderr,
            "%s:%d Fatal: chirp is not initialzed. ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            chirp
        );
        return CH_UNINIT; // NOCOV  TODO can be tested
    }
    A(chirp->_init == CH_CHIRP_MAGIC, "Not a ch_chirp_t*");
    ch_chirp_int_t* ichirp = chirp->_;
    if(ichirp->flags & CH_CHIRP_CLOSED) {
        fprintf(
            stderr,
            "%s:%d Fatal: chirp is already closed. ch_chirp_t:%p\n",
            __FILE__,
            __LINE__,
            chirp
        );
        return CH_FATAL;
    }
    if(ichirp->flags & CH_CHIRP_CLOSING) {
        L(chirp, "Error: close already in progress. ch_chirp_t:%p", chirp);
        return CH_IN_PRORESS;
    }
    ichirp->flags |= CH_CHIRP_CLOSING;
    ichirp->close.data = chirp;
    if(uv_async_send(&ichirp->close) < 0) {
        L(chirp, "Error: could not call close callback. ch_chirp_t:%p", chirp);
        return CH_UV_ERROR; // NOCOV only breaking things will trigger this
    }
    return CH_SUCCESS;
}

// .. c:function::
static void
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
            ichirp->loop,
            chirp
        );
    }
    ch_free(ichirp);
    ichirp->flags |= CH_CHIRP_CLOSED;
    L(chirp, "Closed. ch_chirp_t:%p", chirp);
    if(sglib_ch_chirp_t_is_member(_ch_chirp_instances, chirp))
        sglib_ch_chirp_t_delete(&_ch_chirp_instances, chirp);
    else {
        L(
            chirp,
            "Error: closing unknown chirp instance. ch_chirp_t:%p",
            chirp
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
    memcpy(id.data, chirp->_->config.IDENTITY, sizeof(id.data));
    return id;
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
    ichirp->loop            = loop;
    ch_config_t* tmp_conf   = &ichirp->config;
    ch_protocol_t* protocol = &ichirp->protocol;
    ch_encryption_t* enc    = &ichirp->encryption;
    chirp->_                = ichirp;
    if(log_cb != NULL)
        ch_chirp_register_log_cb(chirp, log_cb);

    // rand
    srand((unsigned int) time(NULL));
    int i = 0;
    while(
            i < sizeof(tmp_conf->IDENTITY) &&
            tmp_conf->IDENTITY[i] == 0
    ) i += 1;
    if(tmp_conf->IDENTITY[i] == 0)
        _ch_random_ints_to_bytes(ichirp->identity, sizeof(tmp_conf->IDENTITY));
    else
        *ichirp->identity = *tmp_conf->IDENTITY;


    if(uv_async_init(loop, &ichirp->close, _ch_chirp_close_async_cb) < 0) {
        L(
            chirp,
            "Error: Could not initialize close callback. ch_chirp_t:%p",
            chirp
        );
        ch_free(ichirp);
        return CH_UV_ERROR; // NOCOV
    }

    ch_pr_init(chirp, protocol);
    tmp_err = ch_pr_start(protocol);
    if(tmp_err != CH_SUCCESS) {
        L(
            chirp,
            "Error: Could not start protocol: %d. ch_chirp_t:%p",
            tmp_err,
            chirp
        );
        ch_free(ichirp);
        return tmp_err;
    }
    ch_en_init(chirp, enc);
    tmp_err = ch_en_start(enc);
    if(tmp_err != CH_SUCCESS) {
        L(
            chirp,
            "Error: Could not start protocol: %d. ch_chirp_t:%p",
            tmp_err,
            chirp
        );
        ch_free(ichirp);
        return tmp_err;
    }

    L(
        chirp,
        "Chirp initialized id: %x %x %x %x. ch_chirp_t:%p, uv_loop_t:%p",
        (uint32_t) ichirp->identity[0],
        (uint32_t) ichirp->identity[3],
        (uint32_t) ichirp->identity[7],
        (uint32_t) ichirp->identity[11],
        chirp,
        loop
    );
    if(!_ch_chirp_sig_init) {
       if(signal(SIGINT, _ch_chirp_sig_handler) == SIG_ERR) {
            L(
                chirp,
                "Error: Unable to set SIGINT handler. ch_chirp_t:%p",
                chirp
            );
       }
       else
           _ch_chirp_sig_init = 1;
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

    tmp_err = _ch_uv_error_map(ch_loop_init(&loop));
    if(tmp_err != CH_SUCCESS) {
        L(
            (&chirp),
            "Error: Could not init loop: %d. uv_loop_t:%p",
            tmp_err,
            &loop
        );
        return tmp_err;  // NOCOV this can only fail with access error
    }
    tmp_err = ch_chirp_init(&chirp, config, &loop, NULL);
    if(tmp_err != CH_SUCCESS) {
        L(
            (&chirp),
            "Error: Could not init chirp: %d ch_chirp_t:%p",
            tmp_err,
            &chirp
        );
        return tmp_err;  // NOCOV covered in ch_chirp_init tests
    }
    chirp._->flags |= CH_CHIRP_AUTO_STOP;
    L(
        (&chirp),
        "UV-Loop run by chirp. ch_chirp_t:%p, uv_loop_t:%p",
        &chirp,
        &loop
    );
    /* This works and is not TOO bad because the function blocks. */
    // cppcheck-suppress unmatchedSuppression
    // cppcheck-suppress autoVariables
    *chirp_out = &chirp;
    tmp_err = ch_run(&loop);
    *chirp_out = NULL;
    if(tmp_err != 0) {
        L(
            (&chirp),
            "Error: uv_run returned with error: %d, uv_loop_t:%p",
            tmp_err,
            &loop
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
    if(signo != SIGINT)
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
