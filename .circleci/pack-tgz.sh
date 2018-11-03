#!/bin/bash

# Create archive for launching from anywhere

cmake -H. -Bbuild \
-DINCLUDE_CPACK=TRUE -DCPACK_GENERATOR=TGZ -DCMAKE_INSTALL_PREFIX=/ \
-DCMAKE_CXX_COMPILER=clang++ -DCPACK_SET_DESTDIR=TRUE \
-DCPACK_PACKAGE_VERSION="`git log --pretty=format:'%at+git-%H' -n 1`-x64" \
-DDATADIR=/data -DRUNTIME_DATADIR='./data' -DBINDIR=/ -DINSTALL_DESKTOP=FALSE -DCPACK_ARCHIVE_COMPONENT_INSTALL=FALSE

cmake --build build --target package -- -j4
