"""C4irp bindings"""
import concurrent.futures as fut
import sys
import threading

from . import common, const
from _c4irp_cffi import ffi, lib


class ChirpPool(object):
    """Chirp is message passing with fully automatic connection setup and
    cleanup. Just create a Chirp() object, await obj.start(), create a Message
    using port and address of a peer and then await obj.send(msg).

    .. code-block:: python

       def setup_chirp():
           client = chirp.Chirp()
           client.start()
           msg = chirp.Message(
               "10.10.10.10",
               2998,
               "yo dawg",
           )
           load = client.send(msg).result()
           client.close()

    The connection will be removed if it wasn't used for REUSE_TIME or when you
    close chirp.

    The config argument can either be None, a object of a class like
    :class:`ccchirp.Config` or a dictionary containing the same keys as
    :class:`ccchirp.Config`. If config is None the default will be used. When
    config is set, the defined config variables will be taken and default
    copied from :class:`ccchirp.Config`.

    :param config: Config as either a object or a dictionary.
    :type  config: :py:class:`object` or :py:class:`dict`
    """
    def __init__(
            self,
            config = None,
    ):
        self._config = common.complete_config(config, const.Config())
        self._chirp  = ffi.new("ch_chirp_t*")
        self._loop   = ffi.new("uv_loop_t*")
        self._thread = None
        self._pool   = None

    # TODO properties

    def start(self):
        """Start servers and cleanup routines."""
        # TODO error to excetion method
        lib.ch_loop_init(self._loop)
        # TODO config
        lib.ch_chirp_init(
            self._chirp, lib.ch_config_defaults, self._loop
        )
        lib.ch_chirp_register_log_cb(self._chirp, lib.python_log_cb)

        def run():
            lib.ch_run(self._loop, lib.UV_RUN_DEFAULT)
            lib.ch_loop_close(self._loop)

        self._pool   = fut.ThreadPoolExecutor(
            max_workers=self._config.MAX_HANDLERS
        )
        self._thread = threading.Thread(target=run)
        self._thread.start()

    def close(self):
        """Closing everything."""
        lib.ch_chirp_close_ts(self._chirp)
        self._thread.join()
        self._pool.shutdown()

if sys.version_info > (3, 4):

    class ChirpAsync(object):
        """TODO

        Chirp is message passing with fully automatic connection setup and
        cleanup. Just create a Chirp() object, await obj.start(), create a
        Message using port and address of a peer and then await obj.send(msg).

        .. code-block:: python

           async def setup_chirp():
               client = chirp.Chirp()
               await client.start()
               msg = chirp.Message(
                   "10.10.10.10",
                   2998,
                   "yo dawg",
               )
               load = await client.send(msg)
               await client.close()

        The connection will be removed if it wasn't used for REUSE_TIME or when
        you close chirp.

        The config argument can either be None, a object of a class like
        :class:`ccchirp.Config` or a dictionary containing the same keys as
        :class:`ccchirp.Config`. If config is None the default will be used.
        When config is set, the defined config variables will be taken and
        default copied from :class:`ccchirp.Config`.

        :param config: Config as either a object or a dictionary.
        :type  config: :py:class:`object` or :py:class:`dict`
        """
        def __init__(
                self,
                config = None,
        ):
            self._config       = common.complete_config(
                config, const.Config()
            )
