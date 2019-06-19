cmake_minimum_required(VERSION 3.8)

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(cubosphere
DESCRIPTION "3D puzzle game, open source remake of Kula world"
LANGUAGES C CXX)

set(PROJECT_VERSION "0.4-alpha1")
set(PROJECT_HOMEPAGE_URL "https://github.com/cubosphere")

include(GNUInstallDirs)

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type specified, defaulting to RelWithDebInfo")
	set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

option(USE_SYSTEM_GLEW "Use system GLEW instead of the built-in version, when available." ON)

set(BINDIR "${CMAKE_INSTALL_FULL_BINDIR}" CACHE STRING "Where to install binary")
set(DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/cubosphere" CACHE STRING "Data location (install-time)")
set(RUNTIME_DATADIR "${DATADIR}" CACHE STRING "Data location (run-time)")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive") # Allow some errors, otherwise compilation fails on Windows

# Define dependencies path
if(MSVC)
    set(DEPENDENCIES "dependencies-vs")
elseif(MINGW)
    set(DEPENDENCIES "dependencies-mingw")
else()
    set(DEPENDENCIES "dependencies")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DEPENDENCIES "${DEPENDENCIES}-64bit")
endif()

if(WIN32)
    if(NOT IS_DIRECTORY "${PROJECT_SOURCE_DIR}/${DEPENDENCIES}")
        set(DEPENDENCIES "dependencies")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(DEPENDENCIES "${DEPENDENCIES}-64bit")
        endif()
    endif()

    if(NOT IS_DIRECTORY "${PROJECT_SOURCE_DIR}/${DEPENDENCIES}")
        message(FATAL_ERROR "Dependencies directory not found.")
    endif()
endif()

# These variables enable MSVC to find libraries located in "dependencies"
if(WIN32)
    set(ENV{PATH} "$ENV{PATH};${PROJECT_SOURCE_DIR}/${DEPENDENCIES}/include")
    set(ENV{LIB} ${PROJECT_SOURCE_DIR}/${DEPENDENCIES}/lib)
    include_directories(${PROJECT_SOURCE_DIR}/${DEPENDENCIES}/include)
endif()

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)

    if(NOT PKGCONFIG_FOUND)
        message(FATAL_ERROR "Pkg-config not found.")
    endif()
endif()

# Find system GLEW library or build it if missing
if (APPLE)
    find_package(glew)

    if(GLEW_FOUND)
        include_directories(${GLEW_INCLUDE_DIRS})
    endif()
else()
    if((UNIX AND NOT APPLE) AND USE_SYSTEM_GLEW)
        pkg_check_modules(GLEW glew>=2.1)
    endif()

    if(GLEW_FOUND)
        include_directories(${GLEW_INCLUDE_DIRS})
    else()
        # Fallback to built-in version silently
        add_subdirectory("${PROJECT_SOURCE_DIR}/lib/glew")
        include_directories("${PROJECT_SOURCE_DIR}/lib/glew/include")
        set(GLEW_LIBRARIES "glew")
    endif()
endif()

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB HEADER_FILES "src/*.hpp")

add_executable(cubosphere
	${SRC_FILES}
	${HEADER_FILES}
)

target_compile_definitions(cubosphere PRIVATE "DATADIR=\"${RUNTIME_DATADIR}\"")

# Find POCO library and enable it if present
if(NOT WIN32)
    find_package(Poco COMPONENTS Zip Foundation)
endif()
if(Poco_FOUND)
    include_directories(${Poco_INCLUDE_DIRS})
    add_definitions(-DUSE_POCO)
    target_link_libraries(cubosphere ${Poco_LIBRARIES})
endif()

# Please, keep order of find_package/target_include_directories/target_link_libraries/target_compile_definitions
# same to make future editing easier.

# Various libs
find_package(OpenGL     REQUIRED COMPONENTS OpenGL)
find_package(Lua "5.1" EXACT REQUIRED)

# SDL
find_package(SDL2       REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf   REQUIRED)
find_package(SDL2_image REQUIRED)

set_property(TARGET cubosphere PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(cubosphere PRIVATE
# Various libs
	${OPENGL_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}

# SDL
	${SDL2_INCLUDE_DIR}
	${SDL2_MIXER_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(cubosphere
# Various libs
	${OPENGL_LIBRARIES}
	${LUA_LIBRARIES}
	${GLEW_LIBRARIES}
    stdc++fs

# SDL
	${SDL2_LIBRARY}
	${SDL2_MIXER_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
	${SDL2_IMAGE_LIBRARIES}
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(cubosphere PRIVATE "$<$<CONFIG:DEBUG>:-fstandalone-debug>")
endif()

if(MINGW AND CMAKE_BUILD_TYPE MATCHES Release)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
endif()

if(MINGW AND CMAKE_BUILD_TYPE MATCHES Debug)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,console")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mconsole")
endif()

if(MSVC)
    # VS will automatically add NDEBUG for release mode, but only _DEBUG in debug mode.
    # Since STK uses DEBUG, this is added for debug compilation only:
    set_property(DIRECTORY PROPERTY COMPILE_DEFINITIONS_DEBUG DEBUG)
else()
    # All non VS generators used create only a single compile mode, so
    # compile flags can be simplye be added
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_definitions(-DDEBUG)
    else()
        add_definitions(-DNDEBUG)
    endif()
endif()

if(MSVC OR MINGW)
  add_custom_command(TARGET cubosphere POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/${DEPENDENCIES}/bin"
        $<TARGET_FILE_DIR:cubosphere>)
endif()

if(MINGW)
    find_library(LIBGCC NAMES "libgcc_s_dw2-1.dll" "libgcc_s_sjlj-1.dll" "libgcc_s_seh-1.dll" PATHS ${CMAKE_FIND_ROOT_PATH})
    if(LIBGCC)
        file(COPY ${LIBGCC} DESTINATION ${CMAKE_BINARY_DIR}/bin/)
    endif()
    find_library(LIBSTDCPP NAMES "libstdc++-6.dll" PATHS ${CMAKE_FIND_ROOT_PATH})
    if(LIBSTDCPP)
        file(COPY ${LIBSTDCPP} DESTINATION ${CMAKE_BINARY_DIR}/bin/)
    endif()
    find_library(LIBPTHREAD NAMES "winpthread-1.dll" "libwinpthread-1.dll" "pthreadGC2.dll" PATHS ${CMAKE_FIND_ROOT_PATH})
    if(LIBPTHREAD)
        file(COPY ${LIBPTHREAD} DESTINATION ${CMAKE_BINARY_DIR}/bin/)
    endif()
endif()

install(TARGETS cubosphere DESTINATION "${BINDIR}" COMPONENT BIN)

if(MINGW)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/ DESTINATION ${BINDIR}
            FILES_MATCHING PATTERN "*.dll")
endif()

add_subdirectory(data)

if(INCLUDE_CPACK) # TODO: extend support for other distros, Windows, and macOS.
	set(CPACK_PACKAGE_NAME "cubosphere")
	set(CPACK_PACKAGE_VENDOR "Cubosphere Dev Team")
	set(CPACK_PACKAGE_DESCRIPTION "This project is a free game similar to the PS1/PSX game Kula World (known as Roll Away in some regions). It is designed to be platform independent, written in C++ and using the following libraries: OpenGL, SDL2 (Simple DirectMedia Layer), Lua, and POCO.
Features:

* Game similar to Kula World
* Over 450 levels in 34 different designs!
* Starting from beta 0.3 — a lot of new stuff like magnets, gravity changers, and so on
* Multiball feature and two-player-mode
* Internal level editor!
* 3D Engine via OpenGL
* GLSL Shaders
* Joystick/Gamepad support")
	set(CPACK_PACKAGE_CONTACT "QwertyChouskie asdfghrbljzmkd@outlook.com")
	if(NOT CPACK_PACKAGE_VERSION) # Auto git builds use hashes
		set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
	endif()
	set(CPACK_PACKAGE_VERSION_MAJOR "0")
	set(CPACK_PACKAGE_VERSION_MINOR "3")
	set(CPACK_PACKAGE_VERSION_PATCH "90")
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
