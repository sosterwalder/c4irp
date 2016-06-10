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

   c4irpc/array.h.rst
   c4irpc/array_test.c.rst
   c4irpc/common.h.rst
   c4irpc/message.h.rst
   c4irpc/message.c.rst
   c4irpc/c4irp.h.rst
   c4irpc/c4irp.c.rst
   c4irpc/protocol.h.rst
   c4irpc/protocol.c.rst

Public Python API
=================

.. toctree::
   :maxdepth: 3

   c4irp.rst
