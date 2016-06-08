"""Common functions for chirp and depending modules"""

import itertools


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
