// =============
// Buffer Header
// =============
//
// .. code-block:: cpp

#ifndef ch_buffer_h
#define ch_buffer_h

#include "util.h"

// .. c:type:: ch_bf_handler_t
//
//    Preallocated buffer for a chirp handler
//
//    .. c:member:: void* header
//
//    Preallocated buffer for the chirp header
//
//    .. c:member:: char* actor
//
//    Preallocated buffer for the actor
//
//    .. c:member:: void* data
//
//    Preallocated buffer for the data
//
// .. code-block:: cpp

typedef struct ch_bf_handler_s {
    ch_buf* header[CH_BF_PREALLOC_HEADER];
    char*   actor[CH_BF_PREALLOC_ACTOR];
    ch_buf* data[CH_BF_PREALLOC_DATA];
} ch_bf_handler_t;


// .. c:type:: ch_buffer_t
//
//    Contains the preallocated buffers for the chirp handlers
//
//    .. c:member:: unsinged char state
//
// .. code-block:: cpp

typedef struct ch_buffer_s {
    uint8_t  max_buffers;
    uint32_t free_buffers;
    ch_bf_handler_t* handlers;
} ch_buffer_t;

// .. c:function::
static
ch_inline
void
ch_bf_free(ch_buffer_t* buffer)
//
//    Free the buffer structure
//
//    :param ch_buffer_t* buffer: The buffer
//    :param max_buffers: Buffers to allocate
//
// .. code-block:: cpp
//
{
    ch_free(buffer->handlers);
}

// .. c:function::
static
ch_inline
void
ch_bf_init(ch_buffer_t* buffer, uint8_t max_buffers)
//
//    Initialize the buffer structure
//
//    :param ch_buffer_t* buffer: The buffer
//    :param max_buffers: Buffers to allocate
//
// .. code-block:: cpp
//
{
    A(max_buffers <= 32, "buffer.c can't handle more than 32 handlers");
    buffer->max_buffers = max_buffers;
    buffer->handlers = ch_alloc(max_buffers * sizeof(ch_bf_handler_t));
    buffer->free_buffers = 0xFFFFFFFFU;
    buffer->free_buffers <<= (32 - max_buffers);
}

#endif //ch_buffer_h
