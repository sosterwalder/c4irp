from _c4irp_cffi import ffi, lib


def test_init_free():
    """Test if we can initialize and free c4irp"""
    chirp = ffi.new("ch_chirp_t*")
    lib.ch_chirp_init(chirp)
    lib.ch_chirp_free(chirp[0])

if __name__ == "__main__":
    test_init_free()
    print("Minimal test: ok")
