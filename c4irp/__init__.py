import logging
from _c4irp_cffi import ffi

log = logging.getLogger("c4irp")


@ffi.def_extern()
def python_log_cb(msg):
    log.debug(ffi.string(msg).decode("UTF-8"))
