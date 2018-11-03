#!/bin/bash
set -e

REL="`lsb_release -cs`"
BIN_VER="`git log --pretty=format:'%at+git-%H' -n 1 .`-$REL"
DATA_VER="`git log --pretty=format:'%at+git-%H' -n 1 data`-$REL"

cmake -H. -Bbuild \
-DINCLUDE_CPACK=TRUE -DCPACK_GENERATOR=DEB -DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_CXX_COMPILER=clang++ -DCPACK_SET_DESTDIR=TRUE \
-DCPACK_PACKAGE_VERSION="$BIN_VER" \
-DBINDIR=/usr/games -DDATADIR=/usr/share/games/cubosphere \
-DCPACK_COMPONENTS_ALL=BIN -DCPACK_DEBIAN_BIN_PACKAGE_DEPENDS="cubosphere-data (= $DATA_VER)"

cmake --build build --target package -- -j4

cmake -H. -Bbuild \
-DINCLUDE_CPACK=TRUE -DCPACK_GENERATOR=DEB -DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_CXX_COMPILER=clang++ -DCPACK_SET_DESTDIR=TRUE \
-DCPACK_PACKAGE_VERSION="$DATA_VER" \
-DBINDIR=/usr/games -DDATADIR=/usr/share/games/cubosphere \
-DCPACK_COMPONENTS_ALL=DATA

cmake --build build --target package -- -j4
