
include_directories(.)

if(CMAKE_COMPILER_IS_GNUCXX)
	exec_program("${CMAKE_CXX_COMPILER}" ARGS --version OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
	if(CMAKE_CXX_COMPILER_VERSION MATCHES ".*4\\.6.*")
		set(CXX_STANDARD "c++0x")
	else()
		set(CXX_STANDARD "c++11")
	endif()
	message(STATUS "Using ${CXX_STANDARD} standard.")
    add_definitions("-std=${CXX_STANDARD}")
endif()

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost
	COMPONENTS
		filesystem
		python3
		signals
		system
	REQUIRED
)
find_package(Python3Libs REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL REQUIRED)

link_libraries(
	${PYTHON3_LIBRARIES}
	${Boost_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${SDL_LIBRARY}
)
include_directories(
	${PYTHON3_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${SDL_INCLUDE_DIR}
)

add_subdirectory(etc)
add_subdirectory(app)
add_subdirectory(cube)
