@echo off

set BOOST_ROOT="C:\Documents and Settings\root\TuttleOFX\3rdParty\boost"
set QTDIR="C:\QtSDK\mingw"
set QTDIR="C:\QtSDK\Desktop\Qt\4.7.4\mingw"
@echo on
"C:\Documents and Settings\root\TuttleOFX\3rdParty\boost\bjam.exe" --toolset=gcc variant=release address-model=32
@echo off
copy ..\rlm\win\lib32\rlm.dll dist\release
@echo on
