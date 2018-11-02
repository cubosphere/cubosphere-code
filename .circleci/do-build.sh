#!/bin/bash
set -e

cmake -H. -Bbuild \
-DINCLUDE_CPACK=TRUE -DCPACK_GENERATOR=DEB -DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_CXX_COMPILER=clang++ -DCPACK_SET_DESTDIR=TRUE \
-DCPACK_PACKAGE_VERSION="`git log --pretty=format:'%at+git-%H' -n 1`-`lsb_release -cs`" \
-DBINDIR=/usr/games -DDATADIR=/usr/share/games/cubosphere # Override for standard games location

cmake --build build -- -j4

cmake --build build --target install
cmake --build build --target package -- -j4
