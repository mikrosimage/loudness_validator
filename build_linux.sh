#!/bin/sh
$BOOST_ROOT/bjam --toolset=gcc cxxflags=-fPIC address-model=64 -j2 variant=release $*
