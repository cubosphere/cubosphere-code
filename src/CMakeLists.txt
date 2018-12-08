# Please, keep order of find_package/target_include_directories/target_link_libraries/target_compile_definitions
# same to make future editing easier.

# Various libs
find_package(OpenGL     REQUIRED COMPONENTS OpenGL)
find_package(Lua51      REQUIRED)
find_package(GLEW       REQUIRED)
#find_package(GLUT      REQUIRED)
find_package(JPEG       REQUIRED)
find_package(PNG++      REQUIRED)
find_package(Poco       REQUIRED COMPONENTS Zip Foundation)

# SDL
find_package(SDL2       REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf   REQUIRED)

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

# SDL
	${SDL2_INCLUDE_DIR}
	${SDL2_MIXER_INCLUDE_DIRS}
	${SDL2_TTF_INCLUDE_DIRS}
)

target_link_libraries(cubosphere
# Various libs
	${OPENGL_LIBRARIES}
	${LUA_LIBRARIES}
	${GLEW_LIBRARIES}
#	${GLUT_LIBRARIES}
	${JPEG_LIBRARIES}
	${PNG++_LIBRARIES}
	${Poco_LIBRARIES}

# SDL
	${SDL2_LIBRARY}
	${SDL2_MIXER_LIBRARIES}
	${SDL2_TTF_LIBRARIES}
)

target_compile_definitions(cubosphere PRIVATE
	# Various libs
	${PNG++_DEFINITIONS}
	-DPNGPP_HAS_STD_MOVE # Should always work in C++17
)

target_compile_options(cubosphere PRIVATE "$<$<CONFIG:DEBUG>:-fstandalone-debug>")

install(TARGETS cubosphere DESTINATION "${BINDIR}" COMPONENT BIN)
