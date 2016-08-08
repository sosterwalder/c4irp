=====
C4irp
=====

Message-passing and actor-based programming for everyone

It is part of Concreteclouds_ and the C99 implementation of Chirp_.

.. _Concreteclouds: https://concretecloud.github.io/

.. _Chirp: https://github.com/concretecloud/chirp

|floobits| |travis| |appveyor| |rtd| |coverage| [1]_

.. |floobits|  image:: https://floobits.com/ganwell/c4irp.svg
   :target: https://floobits.com/ganwell/c4irp/redirect
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

* Install Visual Studio latest

   * Select Git for Windows in Visual Studio Installer or install it

* Install Visual Studio_ 2008 Express

* Install Windows SDK_ for Windows Server 2008 and .NET Framework 3.5

.. _Studio: http://download.microsoft.com/download/E/8/E/E8EEB394-7F42-4963-A2D8-29559B738298/VS2008ExpressWithSP1ENUX1504728.iso

.. _SDK: http://www.microsoft.com/en-us/download/details.aspx?id=24826

.. NOTE::

   Python 2 needs all this, Python 3 is easier, but we need it anyways.
   Python 2 also recommends to use this exact versions, since Windows is too much
   work anyways. I just followed the instructions as close as possible.

   Note also, its no real development setup, it always builds everything.

.. code-block:: bash

   cmd /C "makefile.cmd debug test"

   # or

   cmd /C "makefile.cmd release"

License
=======

GNU AFFERO GENERAL PUBLIC LICENSE

Please contact me for alternative licenses.

Copyright
=========

* © 2016-∞ Jean-Louis Fuchs

* © 2016-∞ Adfinis-SyGroup AG

source used in this reposoitory

* libuv see https://github.com/libuv/libuv
