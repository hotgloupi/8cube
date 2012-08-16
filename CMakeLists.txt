project(8cube)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${CMAKE_SOURCE_DIR}/cmake/Modules"
)
set(CMAKE_BUILD_TYPE Debug)

set(RELEASE_DIRECTORY
	"${CMAKE_CURRENT_BINARY_DIR}/release"
)

set(EXECUTABLE_OUTPUT_PATH
	"${RELEASE_DIRECTORY}/bin"
)

set(LIBRARY_OUTPUT_PATH
	"${RELEASE_DIRECTORY}/lib"
)

add_subdirectory(src)
