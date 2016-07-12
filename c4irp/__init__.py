"""Message-passing and actor-based programming for everyone."""
import logging
import sys

from _chirp_cffi import ffi

from .chirp import ChirpPool  # noqa
from .const import Config  # noqa

LG = logging.getLogger("c4irp")

if sys.version_info > (3, 4):  # noqa
    from .chirp import ChirpAsync  # noqa # pragma: no cover


@ffi.def_extern()
def python_log_cb(msg):
    """Used as a callback from libchirp to log into python."""
    LG.debug(ffi.string(msg).decode("UTF-8"))

del python_log_cb
