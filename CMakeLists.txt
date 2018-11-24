cmake_minimum_required(VERSION 3.8)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)

project(cubosphere
DESCRIPTION "3D puzzle, open source remake of Kula world"
LANGUAGES CXX)

set(PROJECT_HOMEPAGE_URL "https://github.com/cubosphere")

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type specified, defaulting to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(BINDIR "${CMAKE_INSTALL_FULL_BINDIR}" CACHE STRING "Where install binary")

set(DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/cubosphere" CACHE STRING "Data location (install-time)")
set(RUNTIME_DATADIR "${DATADIR}" CACHE STRING "Data location (run-time)")

add_subdirectory(src)
add_subdirectory(data)

if(INCLUDE_CPACK) # TODO: extend support for other distros, Windows™ and OS X.
	set(CPACK_PACKAGE_NAME "cubosphere")
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
	set(CPACK_DEB_COMPONENT_INSTALL ON)
	set(CPACK_DEBIAN_BIN_PACKAGE_SHLIBDEPS ON)
	set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")
	set(CPACK_DEBIAN_PACKAGE_SECTION "games")
	set(CPACK_DEBIAN_BIN_PACKAGE_NAME "cubosphere") # Avoid -bin suffix
	set(CPACK_DEBIAN_DATA_PACKAGE_NAME "cubosphere-data")
	set(CPACK_DEBIAN_DATA_PACKAGE_ARCHITECTURE "all") # One package for all arches
	include(CPack)
endif()
