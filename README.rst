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
   :target: https://docs.adfinis-sygroup.ch/public/chirp/
.. |coverage| image:: https://img.shields.io/badge/coverage-100%25-brightgreen.svg

`Read the Docs`_

.. _`Read the Docs`: https://docs.adfinis-sygroup.ch/public/chirp/

.. [1] Coverage enforced by tests (on travis)

WORK IN PROGRESS
================

FAQ
===

* There is problem with c4irp, how can I get more logging and information?

.. code-block:: bash

   pip uninstall c4irp
   git clone https://github.com/concretecloud/c4irp.git
   cd c4irp
   MODE=debug python setup.py install

This will enable a lot of logging in the c4irp library. See also Development.


Development
===========

Known environment variables:

* CC must be the compiler used to build python.

  * Defaults to the compiler python was built with and is ignored on Windows
  * Use *CC=clang pyenv install 3.5.2* to get a clang version of python, which is
    nicer for development

* MODE=debug|release defaults to release

* CFLAGS ignored on Windows

* LDFAGS ignored on Windows, only used by distutils

* The python version currently in path as python(.exe) will be used. Use
  virtualenv, pyenv or pyenv+virtualenv to change the python version

Requirements:

.. code-block:: bash

   apt-get install build-essential automake autoconf cmake clang llvm libssl-dev libssl1.0.0-dbg

   ./make.py test

Note: You may want to run clean before running tests. This makes sure, that the
config.h file gets cleaned and then copied again (which is essential if there
are new definitions).

.. code-block:: bash

   ./make.py clean

   ./make.py test


Windows
-------

Use the testbuilds:

* https://ci.appveyor.com/project/ganwell/c4irp

* Navigate to the artifacts
Or build ourself: * Install Python 2 (gyp needs this)

* Install Visual Studio 2015 if you want to build for python 3

  * Select Git for Windows in Visual Studio Installer or install it

* Install Visual Studio_ 2008 Express

* Install Windows SDK_ for Windows Server 2008 and .NET Framework 3.5 (python 2)

.. _Studio: http://download.microsoft.com/download/E/8/E/E8EEB394-7F42-4963-A2D8-29559B738298/VS2008ExpressWithSP1ENUX1504728.iso

.. _SDK: http://www.microsoft.com/en-us/download/details.aspx?id=24826

.. NOTE::

   To support *python 2.7 == Visual Studio 2008*, we use c99conv to convert to c89
   and we support *python 3.5+ == Visual Studio 2015* without c99conv. Any future
   version of python that uses a newer version of Visual Studio has to added to
   the build-system explicitly.

   Note also, its no real development setup, it always builds everything.

.. code-block:: bash

   build\stdpython2.cmd
   python make.py

   # or

   make.py test

Contributors
============

* Sven Osterwalder (@sosterwalder)

License
=======

GNU AFFERO GENERAL PUBLIC LICENSE

Please contact me for alternative licenses.

Copyright
=========

* © 2016-∞ Jean-Louis Fuchs

* © 2016-∞ Adfinis-SyGroup AG

source used in this reposoitory:

* libuv see https://github.com/libuv/libuv
