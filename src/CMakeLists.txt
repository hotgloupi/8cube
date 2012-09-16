
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
	)
	list(APPEND CXX_FLAGS_DEBUG
		-g3
		-DDEBUG
		-UNDEBUG
	)
	list(APPEND CXX_FLAGS_RELEASE
		-O3
		-UDEBUG
		-DNDEBUG
		-fvisibility=hidden
	)
endif()

foreach(build_type "" "_DEBUG" "_RELEASE")
	set(CMAKE_CXX_FLAGS${build_type} "")
	foreach(flag ${CXX_FLAGS${build_type}})
		set(CMAKE_CXX_FLAGS${build_type} "${CMAKE_CXX_FLAGS${build_type}} ${flag}")
	endforeach()
	message(STATUS "CXX_FLAGS${build_type}: ${CMAKE_CXX_FLAGS${build_type}}")
endforeach()

set(CMAKE_SHARED_LINKER_FLAGS "-rdynamic")
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")
#set(CMAKE_MODULE_LINKER_FLAGS "")

# Workaround to remove '-rdynamic' flag
# fix scheduled for 2.8.1
# http://www.itk.org/Bug/view.php?id=9985
# http://www.itk.org/Bug/view.php?id=5833
#set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
#set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

if(${CMAKE_BUILD_TYPE} MATCHES "[rR]elease|RELEASE")
	set(USE_STATIC_LIBS ON)
	set(LINK_TYPE STATIC)
else()
	set(USE_STATIC_LIBS OFF)# CACHE BOOL "use static libraries for linking")
	set(LINK_TYPE SHARED)
endif()


if(BUILD_TYPE_CHANGED)
	if(USE_STATIC_LIBS)
		message(STATUS "Linking against static libraries.")
	else()
		message(STATUS "Linking against dynamic libraries.")
	endif()
	set(Boost_FOUND FALSE)
	set(Boost_system_FOUND FALSE)
	set(Boost_filesystem_FOUND FALSE)
	set(Boost_python3_FOUND FALSE)
	set(Boost_signals_FOUND FALSE)
	unset(Boost_INCLUDE_DIR CACHE)
	unset(Boost_LIBRARY_DIRS CACHE)
endif()

set(Boost_USE_STATIC_LIBS       ${USE_STATIC_LIBS})
set(Boost_USE_MULTITHREADED     OFF)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost
	COMPONENTS
		system
		filesystem
		python3
		signals
	REQUIRED
)

foreach(boost_lib ${Boost_LIBRARIES})
	message(STATUS "Found boost library: ${boost_lib}")
endforeach()

set(PYTHON3_USE_STATIC_LIBS ${USE_STATIC_LIBS})
find_package(Python3Libs REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(Freetype REQUIRED)

include_directories(
	${PYTHON3_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${SDL_INCLUDE_DIR}
	${SDLIMAGE_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
)

add_subdirectory(etc)
add_subdirectory(app)
add_subdirectory(cube)

