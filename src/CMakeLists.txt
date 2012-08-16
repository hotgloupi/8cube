
include_directories(.)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost
	COMPONENTS
		filesystem
		python3
		system
	REQUIRED
)

include_directories(${Boost_INCLUDE_DIRS})

find_package(PythonLibs)

include_directories(${PYTHON_INCLUDE_DIRS})

add_subdirectory(etc)
add_subdirectory(app)
add_subdirectory(cube)
