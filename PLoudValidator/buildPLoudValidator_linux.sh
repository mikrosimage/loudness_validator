#!/bin/sh
export BOOST_ROOT=/datas/mrn/apps/boost_1_48_0/
export QTDIR=/usr/lib64/qt4
$BOOST_ROOT/bjam --toolset=gcc cxxflags=-fPIC address-model=64 -j2 variant=release $*
