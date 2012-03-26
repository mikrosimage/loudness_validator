PLoud Validator version 1.0
==================

PLoudValidator is a software to validate audio files with correct loudness.
It can validate various specifications:

* EBU R128

* ATSC A85

* CST-Ficam specification for France.

License
-------

    Mikros Image private project.


Compilation
-----------

* Getting the source

> git clone git://github.com/mikrosimage/loudness_validator.git  
cd loudness_validator

* Linux compilation

> export BOOST_ROOT=path/to/boost
cd PLoudValidator
buildPLoudValidator_linux.sh

* Windows compilation

> set BOOST_ROOT=path/to/boost
cd PLoudValidator
buildPLoudValidator_windows.bat

Tested compilers
----------------

* GCC 4.4.1 on OpenSUSE 11.2
* GCC 4.5.2 on Windows XP 


More information 
----------------

* EBU R-128
[EBU R-128 link](http://tech.ebu.ch/loudness)

* ATSC A85
[ATSC A85 pdf](www.atsc.org/cms/standards/a_85-2011a.pdf)

* France CST-Ficam specification
[CST-Ficam pdf](http://www.arpp-pub.org/IMG/pdf/140911_-_Pub_TV_et_intensite_sonore_v-def-2.pdf)

* EBU test material
[zip archive](http://tech.ebu.ch/webdav/site/tech/shared/testmaterial/ebu-loudness-test-setv03.zip)


