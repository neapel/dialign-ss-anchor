cmake_minimum_required(VERSION 2.8)
project("align" CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(".")

macro(require_libraries LIBS)
	set(CMAKE_REQUIRED_LIBRARIES  ${CMAKE_REQUIRED_LIBRARIES} ${LIBS})
endmacro()


# Tests
include(CTest)
enable_testing()


# Boost libraries
find_package(Boost
	COMPONENTS program_options
	REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	require_libraries(${Boost_LIBRARIES})
endif()

# Cairo
include(FindPkgConfig)
pkg_check_modules(CAIRO cairo)
if(CAIRO_FOUND)
	include_directories(${CAIRO_INCLUDE_DIRS})
	require_libraries(${CAIRO_LIBRARIES})
endif()


# General compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra --std=c++0x")
	add_definitions(-Wall -Wextra --std=c++0x)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3")
	add_definitions(-g3)
endif()


# Targets
add_executable(align align.cc)
target_link_libraries(align ${CMAKE_REQUIRED_LIBRARIES})
