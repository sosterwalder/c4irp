=============
API Reference
=============

Public C API
============

.. note::

   The C API is not thread-safe except where stated: Functions have \*_ts suffix.
   uv_async_send() can be used.

.. toctree::
   :maxdepth: 2

   inc/callbacks.h.rst
   inc/chirp.h.rst
   inc/chirp_obj.h.rst
   inc/const.h.rst
   inc/common.h.rst
   inc/encryption.h.rst
   inc/error.h.rst
   inc/message.h.rst
   inc/wrappers.h.rst

Internal C API
==============

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/array.h.rst
   src/chirp.h.rst
   src/chirp.c.rst
   src/connection.h.rst
   src/connection.c.rst
   src/encryption.h.rst
   src/encryption.c.rst
   src/message.h.rst
   src/message.c.rst
   src/protocol.h.rst
   src/protocol.c.rst
   src/util.h.rst

Public Python API
=================

.. toctree::
   :maxdepth: 3

   chirp.rst

Tests in C
==========

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/connection_test.h.rst
   src/connection_test.c.rst

Test binaries
=============

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/alloc_etest.c.rst
   src/array_etest.c.rst
   src/chirp_etest.c.rst

External Libs
=============

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/sglib.h.rst
