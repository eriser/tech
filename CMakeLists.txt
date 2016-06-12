cmake_minimum_required(VERSION 3.0.0)

# Set a default build type if none was specified ----------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified")
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build" FORCE)

	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel"
		"RelWithDebInfo")
endif()

# Project name --------------------------------------------------------------------------
set(PROJECT_NAME tech CACHE INTERNAL "")
project(${PROJECT_NAME})

# Options -------------------------------------------------------------------------------
option(TECH_GENERATE_DOCS  "Enable documentation generation"           ON)
option(TECH_ENABLE_TESTS   "Enable automatic unit testing"             ON)
option(TECH_ENABLE_SYSTEM  "Enable 'system' module"                    OFF)
option(TECH_ENABLE_SOCKETS "Enable 'sockets' module (requires system)" OFF)
option(TECH_ENABLE_UI      "Enable 'UI' module (requires system)"      OFF)

if(TECH_ENABLE_SOCKETS AND NOT TECH_ENABLE_SYSTEM)
	message(WARNING "Enabling the 'system' module as a dependency for the 'sockets'")
endif()

if(TECH_ENABLE_UI AND NOT TECH_ENABLE_SYSTEM)
	message(WARNING "Enabling the 'system' module as a dependency for the 'ui'")
endif()

message(STATUS "${PROJECT_NAME} modules summary:")
message(STATUS "system  : ${TECH_ENABLE_SYSTEM}")
message(STATUS "sockets : ${TECH_ENABLE_SOCKETS}")
message(STATUS "ui      : ${TECH_ENABLE_UI}")

# Set CMake modules path ----------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

# Set include directories ---------------------------------------------------------------
set(TECH_INCLUDE_DIRS
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_BINARY_DIR}/include
	CACHE STRING "Tech include directories" FORCE
)

include_directories(SYSTEM ${TECH_INCLUDE_DIRS})

# Set compiler flags --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD "14")

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-vla")
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic --stdlib=libstdc++")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-statement-expression")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-vla-extension")
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
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
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
find_package(GTest)

if(GTEST_FOUND AND TECH_ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
