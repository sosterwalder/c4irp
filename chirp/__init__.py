"""Message-passing and actor-based programming for everyone."""
import logging
import sys
import ssl  # noqa

from _chirp_cffi import lib, ffi

from .chirp import ChirpPool  # noqa
from .const import Config  # noqa

LG = logging.getLogger("chirp")

if sys.version_info > (3, 4):  # noqa
    from .chirp import ChirpAsync  # noqa # pragma: no cover

lib.ch_en_set_manual_openssl_init()


@ffi.def_extern()
def python_log_cb(msg, error):
    """Used as a callback from libchirp to log into python."""
    if error:
        LG.error(ffi.string(msg).decode("UTF-8"))
    else:
        LG.debug(ffi.string(msg).decode("UTF-8"))

del python_log_cb
