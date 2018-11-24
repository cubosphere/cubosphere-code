# - Try to find PNG++
#
# The following variables are optionally searched for defaults
#  PNG++_ROOT_DIR:            Base directory where all GLOG components are found
#
# The following are set after configuration is done: 
#  PNG++_FOUND
#  PNG++_INCLUDE_DIRS
#  PNG++_LIBRARIES
#  PNG++_DEFINITIONS

include(FindPackageHandleStandardArgs)

set(PNG++_ROOT_DIR "" CACHE PATH "Folder contains PNG++")

find_path(PNG++_INCLUDE_DIR
    NAMES
        png.hpp
    PATHS
		/usr/include/png++
		/usr/local/include/png++
        ${PNG++_ROOT_DIR}
    PATH_SUFFIXES
        src)

find_package_handle_standard_args(PNG++ DEFAULT_MSG
    PNG++_INCLUDE_DIR)

if(PNG++_FOUND)
	find_package(PNG REQUIRED)
	set(PNG++_INCLUDE_DIRS ${PNG++_INCLUDE_DIR} ${PNG_INCLUDE_DIRS})
	set(PNG++_LIBRARIES ${PNG_LIBRARIES})
	set(PNG++_DEFINITIONS ${PNG_DEFINITIONS})
endif()
