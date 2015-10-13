libcsnap v0.0 && snap v0.0
==========================

Stato:
======
* libreria manca cmd

Bug:
====

Require libcsnap.so:
====================
nothing

Require snap:
=============
libcsnap.so

To install libcsnap.so
======================
$ mkdir oobj
$ make lib
$ sudo make libinstall

To install snap
======================
$ mkdir obj
$ make app
$ sudo make appinstall

To uninstall it:
==============
$sudo rm /usr/bin/snap
$sudo rm /usr/lib/libcsnap.so

