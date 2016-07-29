"""Testing common functions."""
# pylint: disable=E1101

import unittest

import hypothesis.strategies as st
from hypothesis import given

from . import common, const

_any = st.floats(-1, 1) | st.booleans() | st.text() | st.none()


class ConfigTest(unittest.TestCase):
    """Testing complete_config."""

    def test_none(self):
        """Test if right config is returned when we supply none."""
        config = common.complete_config(None, const.Config)
        assert config.REUSE_TIME == const.Config.REUSE_TIME
        assert config.TIMEOUT    == const.Config.TIMEOUT

    def test_object(self):
        """Test if complete_config can handle a object."""
        class ConfigNoOverride(object):
            """Helper to test override."""

            BLA = "LAP"

        # Although we don't advertise this we can even handle a class
        config_a = common.complete_config(ConfigNoOverride, const.Config)
        assert config_a.BLA        == "LAP"
        assert config_a.REUSE_TIME == const.Config.REUSE_TIME

        class ConfigOverride(object):
            """Helper to test override."""

            TIMEOUT = -1

        config_b = common.complete_config(ConfigOverride(), const.Config())
        assert config_b.REUSE_TIME == const.Config.REUSE_TIME
        assert config_b.TIMEOUT    == ConfigOverride.TIMEOUT

        # Check if object override is correct
        obj = const.Config()
        obj.TIMEOUT = -2
        config_c = common.complete_config(obj, const.Config())
        assert config_c.REUSE_TIME == const.Config.REUSE_TIME
        assert config_c.TIMEOUT    == -2

    def test_dict(self):
        """Test if complete_config can handle a dict."""
        config = common.complete_config({'BLA': 'LAP'}, const.Config())
        assert config.BLA        == "LAP"
        assert config.REUSE_TIME == const.Config.REUSE_TIME

        config = common.complete_config({'TIMEOUT': -1}, const.Config())
        assert config.REUSE_TIME == const.Config.REUSE_TIME
        assert config.TIMEOUT    == -1

    @given(st.dictionaries(st.text("ASCII"), _any))
    def test_generated(self, config_in):
        """Test if complete_config can handle dicts created by hypothesis."""
        config_in['TIMEOUT'] = -1
        config = common.complete_config(config_in, const.Config())
        assert config.TIMEOUT == -1
        for (key, value) in config_in.items():
            assert value == getattr(config, key)
