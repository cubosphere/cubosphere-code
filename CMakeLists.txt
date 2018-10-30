cmake_minimum_required(VERSION 3.8)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
include(GNUInstallDirs)

project(cubosphere CXX)

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
