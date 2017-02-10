"""Common functions for chirp and depending modules."""

import itertools
import logging
import os
import subprocess

import six

LG = logging.getLogger("c4irp")

# Pyhton2 doesn't have ProcessLookupError, we just use some not to generic
# exception instead
if six.PY2:
    MyProcessLookupError = subprocess.CalledProcessError
else:
    MyProcessLookupError = ProcessLookupError  # noqa


class TransparentStrRepr(str):
    """A string implementation with a transparent repr."""

    def __repr__(self):
        """Just return the string: make repr transparent."""
        return self


def collect_processes(procs):
    """Terminate and collect output of subprocesses."""
    TR = TransparentStrRepr
    for proc in procs:
        assert isinstance(proc, subprocess.Popen)
        if proc.returncode is None:
            try:
                proc.kill()
            except MyProcessLookupError:  # pragma: no cover
                pass
        LG.debug(
            "Proc %s returncode: %s",
            proc.args,
            proc.returncode,
        )
        LG.debug(
            "Proc %s stdout: %s",
            proc.args,
            TR(proc.stdout.read().decode("UTF-8")),
        )
        LG.debug(
            "Proc %s stderr: %s",
            proc.args,
            TR(proc.stderr.read().decode("UTF-8")),
        )


def complete_config(config, base):
    """Complete a given config with values from a object.

    like: :class:`.const.Config`.

    :param config: The supplied config as either a object or a dictionary.
    :type  config: :py:class:`object` or :py:class:`dict`
    :param   base: The base object which contains the defaults.
    :type    base: object
    :rtype: :class:`.const.Config`
    """
    if config is None:
        return base
    if isinstance(config, dict):
        config_dict = config
    else:
        config_dict = {
            key: value for (key, value) in itertools.chain(
                vars(config.__class__).items(),
                vars(config).items(),
            ) if not key.startswith("_")
        }
    for (key, value) in config_dict.items():
        setattr(base, key, value)

    return base


def get_crypto_files():
    """Get default cert.pem and dh.pem.

    :rtype: list
    """
    folder = os.path.abspath(__file__).split(
        os.path.sep
    )[:-1]
    cert   = list(folder)
    dh     = list(folder)
    cert.append("cert.pem")
    dh.append("dh.pem")
    return (
        os.path.sep.join(cert),
        os.path.sep.join(dh),
    )
