cmake_minimum_required(VERSION 3.0.0)

# Project name
set(PROJECT_NAME tech)
project(${PROJECT_NAME})

# Options -------------------------------------------------------------------------------
option(TECH_GENERATE_DOCS  "Enable documentation generation"       ON)
option(TECH_ENABLE_TESTS   "Enable automatic unit testing"         ON)

# Set CMake modules path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Set include directories ---------------------------------------------------------------
set(TECH_INCLUDE_DIRS
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}/include
	CACHE STRING "Tech include directories" FORCE
)

include_directories(SYSTEM ${TECH_INCLUDE_DIRS})

# Warn user if the build type is not set
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE RelWithDebInfo)
   message(WARNING "CMAKE_BUILD_TYPE is not set, forcing to RelWithDebInfo")
endif()

# Set compiler flags
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "-std=c++14 -Wall -Wextra -Wpedantic -fPIC")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")
endif()

# Generate platform header --------------------------------------------------------------
include(TestBigEndian)
test_big_endian(WORDS_BIGENDIAN)

if(WORDS_BIGENDIAN)
	set(PLATFORM_BIG_ENDIAN true CACHE BOOL "Big-endian platform marker")
else()
	set(PLATFORM_LITTLE_ENDIAN true CACHE BOOL "Little-endian platform marker")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(PLATFORM_NAME "windows")
	set(PLATFORM_WINDOWS true CACHE BOOL "Windows platform marker")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(PLATFORM_NAME "linux")
	set(PLATFORM_LINUX true CACHE BOOL "Linux platform marker")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(PLATFORM_NAME "osx")
	set(PLATFORM_OSX true CACHE BOOL "OSX platform marker")
endif()

configure_file(
	${PROJECT_SOURCE_DIR}/platform.h.in
	${PROJECT_BINARY_DIR}/include/tech/platform.h
)

add_subdirectory(src)

# Run unit tests ------------------------------------------------------------------------
if(TECH_ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
