=============
API Reference
=============

Public C API
============

.. note::

   The C API is not thread-safe except where stated. Functions have \*_ts suffix.
   uv_async_send() can be used.

.. toctree::
   :maxdepth: 2

   inc/c4irp.h.rst
   inc/message.h.rst
   inc/error.h.rst
   inc/c4irp_obj.h.rst
   inc/const.h.rst

Internal C API
==============

.. toctree::
   :maxdepth: 2
   :includehidden:

   src/array.h.rst
   src/array_test.c.rst
   src/common.h.rst
   src/message.h.rst
   src/message.c.rst
   src/c4irp.h.rst
   src/c4irp.c.rst
   src/protocol.h.rst
   src/protocol.c.rst
   src/connection.h.rst
   src/connection.c.rst

Public Python API
=================

.. toctree::
   :maxdepth: 3

   c4irp.rst

Tests in C
==========

.. toctree::
   :maxdepth: 2
   :includehidden:

   c4irpc/connection_test.h.rst
   c4irpc/connection_test.c.rst

External Libs
==============

.. toctree::
   :maxdepth: 2
   :includehidden:

   c4irpc/sglib.h.rst
