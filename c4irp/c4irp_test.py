from contextlib import contextmanager
import threading
import time

import c4irp
from _c4irp_cffi import ffi, lib


def test_init_free():
    """Test if we can initialize and free c4irp"""
    with basic_uv() as error:
        assert error == lib.CH_SUCCESS


def test_init_bind_err():
    """Test if we create a bind error"""
    with basic_uv() as error:
        assert error == lib.CH_SUCCESS
        with basic_uv() as error2:
            assert error2 == lib.CH_EADDRINUSE


@contextmanager
def basic_uv():
    chirp = ffi.new("ch_chirp_t*")
    loop = ffi.new("uv_loop_t*")
    assert lib.ch_loop_init(loop) == lib.CH_SUCCESS
    error = lib.ch_chirp_init(
        chirp, lib.ch_config_defaults, loop
    )
    lib.ch_chirp_register_log_cb(chirp, lib.python_log_cb)
    yield error
    if error == lib.CH_SUCCESS:
        assert lib.ch_chirp_close_ts(chirp) == lib.CH_SUCCESS
        assert lib.ch_run(loop, lib.UV_RUN_ONCE) == lib.CH_SUCCESS
        assert lib.ch_loop_close(loop) == lib.CH_SUCCESS


def test_init_bad_port_low():
    """Test if init with bad port throws CH_VALUE_ERROR"""
    init_bad_port(-123)


def test_init_bad_port_high():
    """Test if init with bad port throws CH_VALUE_ERROR"""
    init_bad_port(65536)


def init_bad_port(port):
    """Test if init with bad port throws CH_VALUE_ERROR"""
    chirp = ffi.new("ch_chirp_t*")
    loop = ffi.new("uv_loop_t*")
    config = ffi.new("ch_config_t*")
    config[0] = lib.ch_config_defaults
    assert config.PORT == 2998
    config.PORT = port
    assert lib.ch_loop_init(loop) == lib.CH_SUCCESS
    assert lib.ch_chirp_init(
        chirp, config[0], loop
    ) == lib.CH_VALUE_ERROR


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


def test_chirp_object_basic():
    """Test if initializing and closing the ChirpPool object works"""
    chirp = c4irp.ChirpPool()
    chirp._chirp.loop = ffi.NULL
    assert chirp._chirp.loop == ffi.NULL
    chirp.start()
    assert chirp._chirp.loop != ffi.NULL
    chirp.close()

if __name__ == "__main__":  # pragma: no cover
    test_init_free()
    print("Minimal test: ok")
