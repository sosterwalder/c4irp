"""Common functions for chirp and depending modules"""

import logging
import itertools
import subprocess

lg = logging.getLogger("c4irp")


class TransparentStrRepr(str):
    """A string implementation with a transparent repr"""

    def __repr__(self):
        return self


def collect_processes(procs):
    TR = TransparentStrRepr
    for proc in procs:
        assert isinstance(proc, subprocess.Popen)
        if proc.returncode is None:
            try:
                proc.kill()
            except ProcessLookupError:  # pragma: no cover
                pass
        lg.debug(
            "Proc %s returncode: %s",
            proc.args,
            proc.returncode,
        )
        lg.debug(
            "Proc %s stdout: %s",
            proc.args,
            TR(proc.stdout.read().decode("UTF-8")),
        )
        lg.debug(
            "Proc %s stderr: %s",
            proc.args,
            TR(proc.stderr.read().decode("UTF-8")),
        )


def complete_config(config, base):
    """Complete a given config with values from a object like
    :class:`.const.Config`.

    :param config: The supplied config as either a object or a dictionary.
    :type  config: :py:class:`object` or :py:class:`dict`
    :param   base: The base object which contains the defaults.
    :type    base: object
    :rtype: :class:`.const.Config`"""
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
