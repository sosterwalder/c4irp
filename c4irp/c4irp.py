"""C4irp bindings"""
import sys

from . import common, const


class ChirpPool(object):
    """Chirp is message passing with fully automatic connection setup and
    cleanup. Just create a Chirp() object, await obj.start(), create a Message
    using port and address of a peer and then await obj.send(msg).

    .. code-block:: python

       async def setup_chirp():
           client = chirp.Chirp()
           await client.start()
           msg = chirp.Message(
               "10.10.10.10",
               2998,
               "yo dawg",
           )
           await client.send(msg)
           await client.close()

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
        self._config       = common.complete_config(config, const.Config())

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
               await client.send(msg)
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
