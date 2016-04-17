import logging
import sys
from _c4irp_cffi import ffi

log = logging.getLogger("c4irp")

from .c4irp import ChirpPool  # noqa
if sys.version_info > (3, 4):  # noqa
    from .c4irp import ChirpAsync  # noqa

from .const import Config  # noqa


@ffi.def_extern()
def python_log_cb(msg):
    log.debug(ffi.string(msg).decode("UTF-8"))

del python_log_cb
