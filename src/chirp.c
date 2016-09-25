// =====
// Chirp
// =====
//
// .. code-block:: cpp

#include "chirp.h"
#include "util.h"

#include <time.h>

//
// .. c:var:: ch_config_defaults
//
//    Default config of chirp.
//
// .. code-block:: cpp
//
ch_config_t ch_config_defaults = {
    .REUSE_TIME     = 30,
    .TIMEOUT        = 5,
    .PORT           = 2998,
    .BACKLOG        = 100,
    .BIND_V6        = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .BIND_V4        = {0, 0, 0, 0},
    .CERT_CHAIN_PEM = NULL,
    .ALLOC_CB       = NULL,
    .FREE_CB        = NULL,
};

// .. c:function::
static void
_ch_close_async_cb(uv_async_t* handle)
//    :noindex:
//
//    see: :c:func:`_ch_close_async_cb`
//
// .. code-block:: cpp
//
{
    int tmp_err;
    CH_GET_CHIRP(handle);
    if(chirp->flags & CH_CHIRP_CLOSED) {
        L(chirp, "Error: chirp closing callback called on closed. ch_chirp_t:%p", chirp);
        return;
    }
    L(chirp, "Chirp closing callback called. ch_chirp_t:%p", chirp);
    ch_chirp_int_t* ichirp = chirp->_;
    assert(ch_pr_stop(&ichirp->protocol) == CH_SUCCESS);
    (void)(tmp_err);
    uv_close((uv_handle_t*) &ichirp->close, ch_chirp_close_cb);
    ichirp->closing_tasks += 1;
    assert(uv_prepare_init(chirp->loop, &ichirp->close_check) == CH_SUCCESS);
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
static
void*
_ch_chirp_std_alloc(
        size_t suggested_size,
        size_t required_size,
        size_t* provided_size
)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_std_alloc`
//
// .. code-block:: cpp
//
{
    *provided_size = suggested_size;
    return malloc(suggested_size);
}
// .. c:function::
static
void
_ch_chirp_std_free(void* buf)
//    :noindex:
//
//    see: :c:func:`_ch_chirp_std_free`
//
// .. code-block:: cpp
//
{
    free(buf);
}

// .. c:function::
ch_error_t
ch_chirp_init(ch_chirp_t* chirp, ch_config_t* config, uv_loop_t* loop)
//    :noindex:
//
//    see: :c:func:`ch_chirp_init`
//
// .. code-block:: cpp
//
{
    int                       tmp_err;
    if(config->ALLOC_CB == NULL) {
        config->ALLOC_CB = _ch_chirp_std_alloc;
    }
    if(config->FREE_CB == NULL) {
        config->FREE_CB = _ch_chirp_std_free;
    }
    memset(chirp, 0, sizeof(ch_chirp_t));
    chirp->config           = config;
    ch_chirp_int_t* ichirp  = ch_chirp_alloc(chirp, sizeof(ch_chirp_int_t));
    memset(ichirp, 0, sizeof(ch_chirp_int_t));
    ch_protocol_t* protocol = &ichirp->protocol;
    chirp->_                = ichirp;
    chirp->loop             = loop;

    // rand
    srand((unsigned int) time(NULL));
    _ch_random_ints_to_bytes(chirp->identity, 16);

    if(uv_async_init(chirp->loop, &ichirp->close, _ch_close_async_cb) < 0) {
        ch_chirp_free(chirp, ichirp);
        return CH_UV_ERROR; // NOCOV
    }

    protocol->chirp    = chirp;
    tmp_err            = ch_pr_start(protocol);
    if(tmp_err != CH_SUCCESS) {
        ch_chirp_free(chirp, ichirp);
        return tmp_err;
    }
    chirp->_init = CH_CHIRP_MAGIC;
    L(chirp, "Chirp initialized. ch_chirp_t:%p, uv_loop_t:%p", &chirp, &loop);
    return CH_SUCCESS;
}

// .. c:function::
ch_error_t
ch_chirp_run(ch_config_t* config, ch_chirp_t** chirp_out)
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
        return tmp_err;  // NOCOV this can only fail with access error
    }
    tmp_err = ch_chirp_init(&chirp, config, &loop);
    if(tmp_err != CH_SUCCESS) {
        return tmp_err;  // NOCOV covered in ch_chirp_init tests
    }
    chirp.flags |= CH_CHIRP_AUTO_STOP;
    L((&chirp), "UV-Loop run by chirp. ch_chirp_t:%p, uv_loop_t:%p", &chirp, &loop);
    /* This works and is not TOO bad because the function blocks. */
    // cppcheck-suppress unmatchedSuppression
    // cppcheck-suppress autoVariables
    *chirp_out = &chirp;
    tmp_err = ch_run(&loop);
    *chirp_out = NULL;
    if(tmp_err != 0) {
        return tmp_err; // NOCOV only breaking things will trigger this
    }
    if(ch_loop_close(chirp.loop)) {
        return CH_UV_ERROR; // NOCOV only breaking things will trigger this
    }
    return CH_SUCCESS;
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
        return CH_UNINIT; // NOCOV  TODO can be tested
    }
    ch_chirp_int_t* ichirp = chirp->_;
    if(chirp->flags & CH_CHIRP_CLOSED) {
        return CH_FATAL;
    }
    if(chirp->flags & CH_CHIRP_CLOSING) {
        return CH_IN_PRORESS;
    }
    chirp->flags |= CH_CHIRP_CLOSING;
    ichirp->close.data = chirp;
    if(uv_async_send(&ichirp->close) < 0) {
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
    chirp->flags |= CH_CHIRP_AUTO_STOP;
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
    L(chirp, "Check closing semaphore (%d). ch_chirp_t:%p", ichirp->closing_tasks, chirp);
    if(ichirp->closing_tasks == 0) {
        assert(uv_prepare_stop(handle) == CH_SUCCESS);
        uv_close((uv_handle_t*) handle, _ch_chirp_closing_down_cb);
    }
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
    if(chirp->flags & CH_CHIRP_AUTO_STOP) {
        uv_stop(chirp->loop);
        L(chirp, "UV-Loop stopped by chirp. ch_chirp_t:%p", chirp->loop);
    }
    ch_chirp_free(chirp, ichirp);
    chirp->flags |= CH_CHIRP_CLOSED;
    L(chirp, "Closed. ch_chirp_t:%p", chirp);
}
