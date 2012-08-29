
include_directories(.)

set(CXX_FLAGS)
set(CXX_FLAGS_DEBUG)
set(CXX_FLAGS_RELEASE)
if(CMAKE_COMPILER_IS_GNUCXX)
	exec_program("${CMAKE_CXX_COMPILER}"
		ARGS --version OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION
	)
	if(CMAKE_CXX_COMPILER_VERSION MATCHES ".*4\\.6.*")
		set(CXX_STANDARD "c++0x")
	else()
		set(CXX_STANDARD "c++11")
	endif()
	message(STATUS "Using ${CXX_STANDARD} standard.")
	list(APPEND CXX_FLAGS
		-std=${CXX_STANDARD}
		-Wall
		-Wextra
		-Winvalid-pch
		-fPIC
	)
	list(APPEND CXX_FLAGS_DEBUG
		-g3
		-DDEBUG
		-UNDEBUG
	)
	list(APPEND CXX_FLAGS_RELEASE
		-O2
		-UDEBUG
		-DNDEBUG
	)
endif()

foreach(build_type "" "_DEBUG" "_RELEASE")
	set(CMAKE_CXX_FLAGS${build_type} "")
	foreach(flag ${CXX_FLAGS${build_type}})
		set(CMAKE_CXX_FLAGS${build_type} "${CMAKE_CXX_FLAGS${build_type}} ${flag}")
	endforeach()
	message(STATUS "CXX_FLAGS${build_type}: ${CMAKE_CXX_FLAGS${build_type}}")
endforeach()

#set(CMAKE_SHARED_LINKER_FLAGS "-rdynamic")
#set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")
#set(CMAKE_MODULE_LINKER_FLAGS "-rdynamic")

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
