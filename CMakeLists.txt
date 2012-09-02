project(8cube)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${CMAKE_SOURCE_DIR}/cmake/Modules"
)

set(CMAKE_BUILD_TYPE Debug
	CACHE STRING "Build type can be Release or Debug"
)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(RELEASE_DIRECTORY
	"${CMAKE_CURRENT_BINARY_DIR}/release"
)

set(EXECUTABLE_OUTPUT_PATH
	"${RELEASE_DIRECTORY}/bin"
)

set(LIBRARY_OUTPUT_PATH
	"${RELEASE_DIRECTORY}/lib"
)

set(PYTHON_LIBRARY_OUTPUT_PATH
	"${LIBRARY_OUTPUT_PATH}/python"
)

set(APP_OUTPUT_PATH
	"${PYTHON_LIBRARY_OUTPUT_PATH}/cubeapp"
)

add_subdirectory(src)

