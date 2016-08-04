=====
C4irp
=====

Message-passing and actor-based programming for everyone

It is part of Concreteclouds_ and the C99 implementation of Chirp_.

.. _Concreteclouds: https://concretecloud.github.io/

.. _Chirp: https://github.com/concretecloud/chirp

|travis| |appveyor| |rtd| |coverage| [1]_

.. |travis|  image:: https://travis-ci.org/concretecloud/c4irp.svg?branch=master
   :target: https://travis-ci.org/concretecloud/c4irp
.. |appveyor| image:: https://ci.appveyor.com/api/projects/status/l8rw8oiv64ledar6?svg=true
   :target: https://ci.appveyor.com/project/ganwell/c4irp
.. |rtd| image:: https://img.shields.io/badge/docs-master-brightgreen.svg
   :target: http://checkmemaster.ignorelist.com/c4irp
.. |coverage| image:: https://img.shields.io/badge/coverage-100%25-brightgreen.svg

`Read the Docs`_

.. _`Read the Docs`: https://docs.adfinis-sygroup.ch/public/chirp/

.. [1] Coverage enforced by tests (on travis)

WORK IN PROGRESS
================

Development
===========

.. code-block:: bash

   apt-get install build-essential automake autoconf cmake clang llvm libssl-dev libssl1.0.0-dbg
   make test-all

Windows
-------

* Install Python 2 (gyp needs this)

* Install Visual Studio

* Select Git for Windows in Visual Studio Installer or install it

.. code-block:: bash

   makefile.cmd

   # or

   makefile.cmd release

License
=======

GNU AFFERO GENERAL PUBLIC LICENSE

Please contact me for alternative licenses.

Copyright
=========

* © 2016-∞ Jean-Louis Fuchs

* © 2016-∞ Adfinis-SyGroup AG

source as a reference:

* evt-tls © Devchandra M. Leishangthem (dlmeetei at gmail dot com)
  https://github.com/deleisha/evt-tls

  - Based on revision: b69164203b481b11dbeb9153fc4eee8a7c3586d3

source used in this reposoitory

* libuv see https://github.com/libuv/libuv
