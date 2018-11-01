cmake_minimum_required(VERSION 3.8)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
include(GNUInstallDirs)

project(cubosphere
DESCRIPTION "3D puzzle, open source remake of Kula world"
LANGUAGES CXX)

set(PROJECT_HOMEPAGE_URL "https://github.com/cubosphere")

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type specified, defaulting to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT BINDIR)
	set(BINDIR "${CMAKE_INSTALL_FULL_BINDIR}")
endif()

if(NOT DATADIR)
	set(DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/cubosphere")
	if((NOT RUNTIME_DATADIR) AND (NOT NO_DATADIR))
		set(RUNTIME_DATADIR "${DATADIR}") # Nobody need it I guess, but let's keep…
	endif()
endif()

add_subdirectory(src)
add_subdirectory(data)

if(INCLUDE_CPACK) # TODO: extend support for other distros, Windows™ and OS X.
	set(CPACK_PACKAGE_NAME "Cubosphere")
	set(CPACK_PACKAGE_VENDOR "Cubosphere team")
	set(CPACK_PACKAGE_DESCRIPTION "Goal of this project is a freeware game similar to the PSX game Kula World. It is designed platform independent, written in C++ and using the following libraries: OpenGL, SDL (Simple DirectMedia Layer), libjpeg, Lua, GLSL-Shader.
Features:

* Game similar to Kula World
* Over 450 levels in 34 different designs!
* Starting from beta 0.3 — a lot of new stuff like magnets, gravity changers and so on
* Multiball feature and two-player-mode
* Internal level editor!
* 3d-Engine via OpenGL
* GLSL-Shaders
* Joystick/Gamepad support")
	set(CPACK_PACKAGE_CONTACT "Valeri Ochinski vyo@vallua.ru")
	if(NOT CPACK_PACKAGE_VERSION) # Auto git builds use hashes
		set(CPACK_PACKAGE_VERSION "0.3b")
	endif()
	set(CPACK_PACKAGE_VERSION_MAJOR "0")
	set(CPACK_PACKAGE_VERSION_MINOR "3")
	set(CPACK_PACKAGE_VERSION_PATCH "3")
	set(CPACK_STRIP_FILES TRUE)

	set(CPACK_PACKAGE_EXECUTABLES "cubosphere;Cubosphere")

	# DEB section
	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")
	set(CPACK_DEBIAN_PACKAGE_SECTION "games")
	include(CPack)
endif()
