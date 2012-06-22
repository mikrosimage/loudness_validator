PLoud Validator version 1.1
==================

PLoudValidator is a software to validate audio files with correct loudness.
It can validate various specifications:

* EBU R 128

* ATSC A85

* CST-Ficam RT-017 specification for France.

It can also correct file to EBU R 128.
Comming soon for others specifications.

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
export QTDIR=/usr/lib64/qt4  
./build_linux.sh  
./build_linux.sh /library /tools /PloudValidator  

see ./build_linux.sh --help for more informations

* Windows compilation

> set BOOST_ROOT=path/to/boost  
set QTDIR="C:\QtSDK\mingw" # with default install  
set QTDIR="C:\QtSDK\Desktop\Qt\4.7.4\mingw"  
cd PLoudValidator  
build_windows.bat  
build_windows.bat  /library /tools /PloudValidator  

see ./build_windows.bat --help for more informations

Tested compilers
----------------

* GCC 4.4.1 on OpenSUSE 11.2
* GCC 4.5.2 on Windows XP

Tested libraries
----------------

* Qt 4.5.3
* libsndfile 1.0.24-22.2


Dependencies
------------

Tools from loudnessLibrary directory requires libsndfile.
And PLoudValidator require also Qt4.

[libsndfile](http://www.mega-nerd.com/libsndfile/)
[Qt4](http://qt.nokia.com/products/)

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


