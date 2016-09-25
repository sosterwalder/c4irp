"""Chirp bindings."""
import concurrent.futures as fut
import os
import sys
import threading

from _chirp_cffi import ffi, lib

from . import common, const


class ChirpPool(object):
    """TODO Documentation -> async to pool, ccchirp to chirp.

    Chirp is message passing with fully automatic connection setup and
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
        self._config    = common.complete_config(config, const.Config())
        self._c_config  = ffi.new("ch_config_t*")
        self._chirp     = ffi.new("ch_chirp_t*")
        self._loop      = ffi.new("uv_loop_t*")
        self._thread    = None
        self._pool      = None
        self._uv_ret    = 0
        self._chirp_ret = 0

    # TODO properties

    def start(self):
        """Start servers and cleanup routines."""
        self._fill_c_config()
        lib.ch_loop_init(self._loop)
        err = lib.ch_chirp_init(
            self._chirp,
            self._c_config,
            self._loop,
            lib.python_log_cb
        )
        if err == lib.CH_EADDRINUSE:
            raise RuntimeError("Port %d already in use." % self._config.PORT)
        assert(err == lib.CH_SUCCESS)
        lib.ch_chirp_set_auto_stop(self._chirp)

        def run():
            """Run chirp in a thread."""
            self._uv_ret = lib.ch_run(self._loop)
            self._chirp_ret = lib.ch_loop_close(self._loop)

        self._pool   = fut.ThreadPoolExecutor(
            max_workers=self._config.MAX_HANDLERS
        )
        self._thread = threading.Thread(target=run)
        self._thread.start()

    def close(self):
        """Closing everything."""
        lib.ch_chirp_close_ts(self._chirp)
        self._thread.join()
        assert(self._uv_ret == lib.CH_SUCCESS)
        assert(self._chirp_ret == lib.CH_SUCCESS)
        self._pool.shutdown()

    def _fill_c_config(self):
        """Fill in the c_config from the config."""
        c_conf = self._c_config
        conf   = self._config
        folder = __file__.split(os.path.sep)[:-1]
        folder.append("cert.pem")
        conf.CERT_CHAIN_PEM = "%s%s" % (
            os.path.sep,
            os.path.join(*folder)
        )
        for std_attr in [
                'REUSE_TIME',
                'TIMEOUT',
                'PORT',
                'BACKLOG',
        ]:
            setattr(
                c_conf,
                std_attr,
                getattr(
                    conf,
                    std_attr,
                )
            )
        c_conf.CERT_CHAIN_PEM = ffi.new(
            "char[]", conf.CERT_CHAIN_PEM.encode("UTF-8")
        )

if sys.version_info > (3, 4):

    class ChirpAsync(object):  # pragma: no cover TODO
        """TODO.

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
