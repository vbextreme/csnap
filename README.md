libcsnap v0.0 && snap v0.0
==========================
gestione protocollo snap tramite libreria scritta per il c esportabile anche sui sitemi embedded aggiunto comando bash per usare il protocollo snap.

http://www.hth.com/snap/

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
$ mkdir oobj<br/>
$ make lib<br/>
$ sudo make libinstall

To install snap
======================
$ mkdir obj<br/>
$ make app<br/>
$ sudo make appinstall

To uninstall it:
==============
$sudo rm /usr/bin/snap<br/>
$sudo rm /usr/lib/libcsnap.so

