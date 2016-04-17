import threading
import time

from _c4irp_cffi import ffi, lib

from . import log


def test_init_free():
    """Test if we can initialize and free c4irp"""
    log.debug("Running test_init_free")
    chirp = ffi.new("ch_chirp_t*")
    loop = ffi.new("uv_loop_t*")
    assert lib.ch_loop_init(loop) == lib.CH_SUCCESS
    assert lib.ch_chirp_init(
        chirp, lib.ch_config_defaults, loop
    ) == lib.CH_SUCCESS
    lib.ch_chirp_register_log_cb(chirp, lib.python_log_cb)
    assert lib.ch_chirp_close_ts(chirp) == lib.CH_SUCCESS
    assert lib.ch_run(loop, lib.UV_RUN_ONCE) == lib.CH_SUCCESS
    assert lib.ch_loop_close(loop) == lib.CH_SUCCESS
    assert False


def test_chirp_run():
    """Test if the do everything ch_chirp_run method works"""

    res = []
    chirp_p = ffi.new("ch_chirp_t**")

    def run():
        res.append(lib.ch_chirp_run(
            lib.ch_config_defaults, chirp_p
        ))

    thread = threading.Thread(target=run)
    thread.start()
    time.sleep(0.1)
    lib.ch_chirp_close_ts(chirp_p[0])
    thread.join()
    assert res[0] == lib.CH_SUCCESS

if __name__ == "__main__":  # pragma: no cover
    test_init_free()
    print("Minimal test: ok")
