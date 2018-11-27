# Please, keep order of find_package/target_include_directories/target_link_libraries/target_compile_definitions
# same to make future editing easier.

# Various libs
find_package(OpenGL     REQUIRED COMPONENTS OpenGL)
find_package(Lua51      REQUIRED)
find_package(GLEW       REQUIRED)
#find_package(GLUT      REQUIRED)
find_package(JPEG       REQUIRED) # TODO: remove in 0.4
find_package(PNG++      REQUIRED)
find_package(LibArchive REQUIRED)

# SDL
find_package(SDL       REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf   REQUIRED)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

add_executable(cubosphere
	${SRC_FILES}
	${HEADER_FILES}
)

target_compile_definitions(cubosphere PRIVATE "DATADIR=\"${RUNTIME_DATADIR}\"")

set_property(TARGET cubosphere PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(cubosphere PRIVATE
# Various libs
	${OPENGL_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
	${GLEW_INCLUDE_DIRS}
#	${GLUT_INCLUDE_DIR}
	${JPEG_INCLUDE_DIRS}
	${PNG++_INCLUDE_DIRS}
	${LibArchive_INCLUDE_DIRS}

# SDL
	${SDL_INCLUDE_DIR}
	${SDL_MIXER_INCLUDE_DIRS}
	${SDL_TTF_INCLUDE_DIRS}
)

target_link_libraries(cubosphere
# Various libs
	${OPENGL_LIBRARIES}
	${LUA_LIBRARIES}
	${GLEW_LIBRARIES}
#	${GLUT_LIBRARIES}
	${JPEG_LIBRARIES}
	${PNG++_LIBRARIES}
	${LibArchive_LIBRARIES}

# SDL
	${SDL_LIBRARY}
	${SDL_MIXER_LIBRARIES}
	${SDL_TTF_LIBRARIES}
)

target_compile_definitions(cubosphere PRIVATE
	# Various libs
	${PNG++_DEFINITIONS}
	-DPNGPP_HAS_STD_MOVE # Should always work in C++17
)

install(TARGETS cubosphere DESTINATION "${BINDIR}" COMPONENT BIN)
