from _c4irp_cffi import ffi, lib


def test_init_free():
    """Test if we can initialize and free c4irp"""
    chirp = ffi.new("ch_chirp_t*")
    loop = ffi.new("uv_loop_t*")
    assert lib.ch_loop_init(loop) == lib.CH_SUCCESS
    assert lib.ch_chirp_init(
        chirp, lib.ch_config_defaults, loop
    ) == lib.CH_SUCCESS
    assert lib.ch_chirp_close(chirp) == lib.CH_SUCCESS
    assert lib.ch_run(loop, lib.UV_RUN_ONCE) == lib.CH_SUCCESS
    assert lib.ch_loop_close(loop) == lib.CH_SUCCESS

if __name__ == "__main__":  # pragma: no cover
    test_init_free()
    print("Minimal test: ok")
