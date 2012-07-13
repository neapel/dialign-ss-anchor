cmake_minimum_required(VERSION 2.8)
project("align" CXX)
set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include_directories(".")

macro(require_libraries LIBS)
	set(CMAKE_REQUIRED_LIBRARIES  ${CMAKE_REQUIRED_LIBRARIES} ${LIBS})
endmacro()


# Tests
include(CTest)
enable_testing()


# Boost libraries
if(${CMAKE_BUILD_TYPE} STREQUAL "RELEASE")
	#set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost
	COMPONENTS program_options
	REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	require_libraries(${Boost_LIBRARIES})
endif()

# Cairo for debugging.
if(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
	include(FindPkgConfig)
	pkg_check_modules(CAIRO cairo)
	if(CAIRO_FOUND)
		include_directories(${CAIRO_INCLUDE_DIRS})
		require_libraries(${CAIRO_LIBRARIES})
	endif()
else()
	set(CAIRO_FOUND 0)
endif()


# General compiler flags
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror --std=c++0x")

	set(CMAKE_CXX_FLAGS_DEBUG "-g3")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 --static")
	set(CMAKE_CXX_FLAGS_PROFILE "-O0 -g3 -pg --coverage")
endif()


# Targets
add_executable(align align.cc)
target_link_libraries(align ${CMAKE_REQUIRED_LIBRARIES})
