# Installing cubosphere from source
After picking up this great project it use CMake for better compatiblity and easier builds. This is manual for manual build, not very different from any other CMake build.
**IMPORTANT: make sure that `data` directory contain files. If it don't, clone repo with submodules this time, consult web about it.**

# Requiments
You will need (with headers, which often stored in `-dev` packages):
* OpenGL (hardware accelerated driver for better results)
* Lua 5.1 (library)
* GLEW
* SDL 1.2
* SDL_ttf 2.0
* SDL_mixer 1.2
* libjpeg

Also, you will need C++ compiler (`gcc/g++`, `clang` or another one), CMake and `make`/`ninja`.

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
* `-G ninja` — use `ninja` generator instead of `Unix makefiles` (generally better). Replace `make` with `ninja` in 3rd and 4th commands.
* `-DDATADIR=` — custom place to install all game data (overrides `-DCMAKE_INSTALL_PREFIX` for data).
* `-DRUNTIME_DATADIR=` — where game will look up for data (overrides both `-DCMAKE_INSTALL_PREFIX` and `-DDATADIR` for runtime).
