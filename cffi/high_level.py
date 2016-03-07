from cffi import FFI
ffi = FFI()

ffi.set_source(
    "_high_level",
    """
    #include <c4irp.h>
    """,
    libraries=["c4irp"],
    library_dirs=["."],
    include_dirs=["../include"],
)


ffi.cdef("""
typedef struct {
    char     address[16];
    int32_t  port;
    char     identity[16];
    char     serial[16];
    int8_t   host_order;
    char*    actor;
    char*    data;
}
ch_message_t;

extern
void
ch_msg_init(ch_message_t* message);
""")

if __name__ == "__main__":
    ffi.compile()
