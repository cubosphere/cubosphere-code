# Cubosphere [reborn] :basketball: :video_game:
[![CircleCI (all branches)](https://img.shields.io/circleci/project/github/cubosphere/cubosphere-code/master.svg)](https://circleci.com/gh/cubosphere/cubosphere-code) [![#cubosphere on the Freenode IRC network](https://img.shields.io/badge/FreeNode-%23cubosphere-brightgreen.svg)](https://kiwiirc.com/client/irc.freenode.net#cubosphere) [![linux downloads](https://img.shields.io/badge/downloads-linux-brightgreen.svg)](http://cubosphere.vallua.ru/artifacts)
## A little story behind it
One upon a time on planet called Earth one man found a very very cool game called "cubosphere", but it was abandoned in 2011-12. He enjoyed it and dreamed that some day it could become great.

After a while, he decided to pick it up and at least make it look like a modern project, so others will be able to contribute easily.

This is why this organisation exists now.

# What is this game about?
This project is a freeware game similar to the PSX game "Kula World" / "Roll Away". It is designed to be platform independent, written in C++ and using the following libraries: OpenGL, SDL2 (Simple DirectMedia Layer), Lua, GLSL-Shader and POCO.

## Features
* Game similar to Kula World
* Over 450 levels in 34 different designs!
* Starting from beta 0.3 — a lot of new stuff like magnets, gravity changers and so on
* Multiball feature and two-player-mode
* Internal level editor!
* 3D engine via OpenGL
* GLSL-Shaders
* Joystick/Gamepad support

# What have I already done to improve the game?
* **Moved to SDL2 from SDL1.2**
* Migrated to CMake
* Separated data and code repos with auto ZIP
* Fixed some warnings
* Removed `using namespace std;`
* Made code C++17 compatible (no warnings or errors)
* Configured Cricle CI with auto .deb build for Ubuntu LTS + last release (read next section), cross-linux .tar.gz
* Created icon and .desktop file
* Moved editor manual to GitHub wiki which can be opened from editor menu
* Added `.png` support and converted all textures to it, previews are saved in it too
* Replaced self-patched minizip with POCO Zip

... and a lot of technical or smaller internal changes!

# How to download the latest git build?
1. Go to http://cubosphere.vallua.ru/artifacts (warning: only the last build artifacts are kept, some of them may be missing if a build is uploading now)
2. Download the appropriate package (the naming scheme is `Cubosphere-timestamp+git-hash-...`)
3. Install as normal (if you don't know how, just open the file and click the "Install" button or similar)

## Ubuntu packages
Some notes on the Ubuntu packages:

1. You need both `BIN` and `DATA` packages, they are `cubosphere` and `cubosphere-data`, respectively.
2. It is OK that their versions differ, data is stored in another repo and updates with it
3. When updating, download only `BIN` package: download `DATA` only if deps can't be resolved 
    1. You'll probably be reqested to remove `cubosphere` when updating `cubosphere-data`, do so and install new version later
    2. Use `dpkg -i` from the command line to avoid this issues (google it for more info)

# What will/should be done?
[Check out the projects page!](https://github.com/cubosphere/cubosphere-code/projects)

# How can I take part in this fun and make cubosphere great again?
You can:

* Fork repo and make a pull request, or
* If you have made a lot of contributions and I trust you then you can ask for direct write access
* Test the game, report issues, and tell your friends about it
* If you have experience with CMake on Windows or macOS, any help with it will be appreciated!
* Join IRC (click the badge at the top of this page) and talk to me, other players and (if they are here!) team members
* Support development on [![Beerpay](https://beerpay.io/cubosphere/cubosphere-code/badge.svg?style=flat)](https://beerpay.io/cubosphere/cubosphere-code), motivate using [![Beerpay](https://beerpay.io/cubosphere/cubosphere-code/make-wish.svg?style=flat)](https://beerpay.io/cubosphere/cubosphere-code)

# OK, some credits
You can find original homepage, codebase and developers at https://sourceforge.net/projects/cubosphere.
