// ===============
// Chirp Callbacks
// ===============
// 
// .. code-block:: cpp

#ifndef ch_inc_callbacks_h
#define ch_inc_callbacks_h

// .. c:type:: ch_alloc_cb_t
//
//    Callback used by chirp to request memory. It can be set in ch_config_t.
//
//    .. c:member:: size_t suggested_size
//
//       The size the user should allocate if possible
//
//    .. c:member:: size_t required_size
//
//       The size the user has to allocate
//
//    .. c:member:: size_t provided_size
//
//       Out: The size the user has allocated
//
//    Libuv wants a buffer of 65536 bytes per stream. It will work with less
//    though.  So we decided that 1k is the absolute minimum see
//    CH_LIB_UV_MIN_BUFFER in config.h. Please provide the suggested_size, except
//    if you're on very restricted embedded system.
//
//    ATTENTION: Since OpenSSL is initialized with the first chirp instance and
//    uninitialized with the last chirp instance. OpenSSL will always use the
//    callbacks of the first chirp instance.
//
// .. code-block:: cpp
//
typedef void* (*ch_alloc_cb_t)(
        size_t suggested_size,
        size_t required_size,
        size_t* provided_size
);

// .. c:type:: ch_free_cb_t
//
//    Callback used by chirp to free memory. It can be set in ch_config_t.
//
//    ATTENTION: Since OpenSSL is initialized with the first chirp instance and
//    uninitialized with the last chirp instance. OpenSSL will always use the
//    callbacks of the first chirp instance.
//
// .. code-block:: cpp
//
typedef void (*ch_free_cb_t)(void* buf);

// .. c:type:: ch_log_cb_t
//
//    Logging callback
//
// .. code-block:: cpp

typedef void (*ch_log_cb_t)(char msg[]);

// .. c:type:: ch_realloc_cb_t
//
//    Callback used by chirp to request memory reallocation. It can be set in ch_config_t.
//
//    .. c:member:: void* buf
//
//       The Buffer to reallocate
//
//    .. c:member:: size_t new_size
//
//       The requested new size
//
//    This function is requested by openssl. Chirp itself won't use it.
//
//    ATTENTION: Since OpenSSL is initialized with the first chirp instance and
//    uninitialized with the last chirp instance. OpenSSL will always use the
//    callbacks of the first chirp instance.
//
// .. code-block:: cpp
//
typedef void* (*ch_realloc_cb_t)(void* buf, size_t new_size);


#endif //ch_inc_callbacks_h
