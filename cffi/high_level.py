from cffi import FFI
ffi = FFI()

ffi.set_source(
    "_high_level",
    """
    #include <c4irp.h>
    """,
    libraries=["c4irp", "uv"],
    library_dirs=["."],
    include_dirs=["../include"],
)


ffi.cdef("""
//error.h
typedef enum {
    CH_SUCCESS     = 0,
    CH_VALUE_ERROR = 1,
    CH_UV_ERROR    = 2
} ch_error_t;

//message.h
typedef struct {
    char     address[16];
    int32_t  port;
    char     identity[16];
    char     serial[16];
    int8_t   host_order;
    int16_t  actor_len;
    int32_t  data_len;
    char*    actor;
    char*    data;
} ch_message_t;

extern
ch_error_t
ch_msg_init(ch_message_t* message);

//c4irp.h
struct ch_chirp_s;
typedef struct ch_chirp_s* ch_chirp_t;

extern
ch_error_t
ch_chirp_init(ch_chirp_t* chirp);

extern
ch_error_t
ch_chirp_free(ch_chirp_t chirp);
""")

if __name__ == "__main__":
    ffi.compile()
