# Installing cubosphere from source
After picking up this great project it use CMake for better compatiblity and easier builds. This is manual for manual build, not very different from any other CMake build.
**IMPORTANT: make sure that `data` directory contain files. If it don't, clone repo with submodules this time, consult web about it.**

# Requiments
You will need (with headers, which often stored in `-dev` packages):

* OpenGL (use hardware accelerated driver for better results)
* Lua 5.1 (library)
* GLEW
* SDL 2.0
* SDL_ttf 2.0
* SDL_mixer 2.0
* libjpeg (libturbojpeg is ok too)
* libpng (and libpng++ for build)
* POCO libraries (`Zip` one is used)

Also, you will need C++ compiler (`gcc/g++`, `clang` or another one), CMake and `make`/`ninja`.

Optimal setpup on Ubuntu 18.04/18.10 (CI build use it):

```
sudo apt-get install -y clang \
extra-cmake-modules cmake \
lsb-release dpkg-dev \
libglvnd-dev liblua5.1-0-dev libglew-dev libjpeg-turbo8-dev libpng++-dev libpoco-dev \
libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev
```

# Build process
Go to directory where this file leans and run in terminal:
```
mkdir build && cd build
cmake ..
make -j4
sudo make install
```

## Some useful CMake arguments:
* `-DCMAKE_INSTALL_PREFIX=` — set intallation prefix.
* `-DCMAKE_CXX_COMPILER=` — set C++ compiler to use (`g++`/`clang++`/etc.).
* `-G Ninja` — use `ninja` generator instead of `Unix makefiles` (generally better). Replace `make` with `ninja` in 3rd and 4th commands.
* `-DDATADIR=` — custom place to install all game data (overrides `-DCMAKE_INSTALL_PREFIX` for data).
* `-DRUNTIME_DATADIR=` — where game will look up for data (overrides both `-DCMAKE_INSTALL_PREFIX` and `-DDATADIR` for runtime).
* `-DINSTALL_DESKTOP=` — install linux menu entery or not (defaults to `UNIX`, so on on linux and off on others)
* `-DZIP=TRUE` — zip most resources (useful for standalone builds, bad for most users) 
**IMPORTANT: POCO Zip (library used by cubosphere) before 1.9.1 (unreleased at `29.11.2018`) will have problems reading it, so don't use**
