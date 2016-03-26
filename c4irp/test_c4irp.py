from _c4irp_cffi import ffi, lib


def test_init_free():
    """Test if we can initialize and free c4irp"""
    chirp = ffi.new("ch_chirp_t*")
    assert lib.ch_chirp_init(chirp, lib.ch_config_defaults) == lib.CH_SUCCESS
    assert lib.ch_chirp_close(chirp) == lib.CH_SUCCESS

if __name__ == "__main__":  # pragma: no cover
    test_init_free()
    print("Minimal test: ok")
