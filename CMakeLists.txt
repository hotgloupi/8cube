project(8cube)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_MODULE_PATH
	"${CMAKE_MODULE_PATH}"
	"${CMAKE_SOURCE_DIR}/cmake/Modules"
)
set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(src)
