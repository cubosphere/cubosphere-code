#!/bin/bash
set -e

env DEBIAN_FRONTEND="noninteractive" apt-get install -y clang \
extra-cmake-modules cmake \
lsb-release dpkg-dev \
libglvnd-dev liblua5.1-0-dev libglew-dev libpoco-dev \
libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev libsdl2-image-dev
