#!/bin/bash
set -e

apt-get install -y clang \
extra-cmake-modules cmake \
lsb-release dpkg-dev \
libglvnd-dev liblua5.1-0-dev libglew-dev libjpeg-turbo8-dev libpng++-dev libpoco-dev \
libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev
