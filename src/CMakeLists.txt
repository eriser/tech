include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TARGET ${PROJECT_NAME})

find_package(Cairo REQUIRED)
find_package(Pango REQUIRED)

include_directories(
	${CAIRO_INCLUDE_DIRS}
	${PANGO_INCLUDE_DIRS}
)

# Common sources
set(SOURCES
    bytearray.cpp
    calendartime.cpp
    char.cpp
    duration.cpp
    format.cpp
    logger.cpp
    string.cpp
    thread.cpp
    timezone.cpp
    ui/button.cpp
    ui/color.cpp
    ui/font.cpp
    ui/fontmetrics.cpp
	ui/gradient.cpp
	ui/image.cpp
    ui/layout.cpp
    ui/painter.cpp
	ui/rowlayout.cpp
    ui/timer.cpp
    ui/widget.cpp
    ui/windowsystem.cpp
	)

set(HEADERS
    ../include/tech/bytearray.h
    ../include/tech/calendartime.h
    ../include/tech/char.h
    ../include/tech/delegate.h
    ../include/tech/duration.h
    ../include/tech/flags.h
    ../include/tech/format.h
    ../include/tech/logger.h
    ../include/tech/passkey.h
    ../include/tech/pimpl.h
    ../include/tech/scopeexit.h
    ../include/tech/semaphore.h
    ../include/tech/signal.h
    ../include/tech/string.h
    ../include/tech/thread.h
    ../include/tech/timecounter.h
    ../include/tech/timezone.h
    ../include/tech/traits.h
    ../include/tech/types.h
    ../include/tech/utils.h
    ../include/tech/ui/button.h
    ../include/tech/ui/color.h
    ../include/tech/ui/events.h
    ../include/tech/ui/font.h
    ../include/tech/ui/fontmetrics.h
	../include/tech/ui/gradient.h
	../include/tech/ui/image.h
    ../include/tech/ui/keys.h
    ../include/tech/ui/layout.h
    ../include/tech/ui/line.h
    ../include/tech/ui/painter.h
	../include/tech/ui/point.h
    ../include/tech/ui/rect.h
	../include/tech/ui/rowlayout.h
    ../include/tech/ui/size.h
    ../include/tech/ui/timer.h
    ../include/tech/ui/widget.h
	)

# Libraries to link with
set(LIBRARIES
	${CAIRO_LIBRARIES}
	${PANGO_LIBRARIES}
)

if(PLATFORM_LINUX)
	find_package(Xcb COMPONENTS XCB ICCCM REQUIRED)
	include_directories(
		${XCB_XCB_INCLUDE_DIR}
		${XCB_ICCCM_INCLUDE_DIR}
		)

	list(APPEND SOURCES
		 timezone_linux.cpp
		 ui/windowsystem_linux.cpp
		)

	list(APPEND LIBRARIES
		 ${XCB_XCB_LIBRARY}
		 ${XCB_ICCCM_LIBRARY}
		 )
elseif(PLATFORM_WINDOWS)
	list(APPEND SOURCES
		 timezone_windows.cpp
		 ui/windowsystem_windows.cpp
		 )
elseif(PLATFORM_OSX)
    list(APPEND SOURCES
		 timezone_osx.cpp
		 ui/windowsystem_osx.cpp
		 )
endif()


add_library(${TARGET} STATIC ${SOURCES} ${HEADERS})
#add_library(${TARGET} SHARED ${SOURCES} ${HEADERS})

# Link with libraries
target_link_libraries(${TARGET} ${LIBRARIES})

# Add target for automatic project version extraction
add_custom_target(extract-version
	COMMAND ${CMAKE_COMMAND}
	-DPROJECT_NAME=${PROJECT_NAME}
	-DROOT_DIR=${PROJECT_SOURCE_DIR}
	-DVERSION_CONFIG=${PROJECT_SOURCE_DIR}/version.h.in
	-DVERSION_HEADER=${PROJECT_BINARY_DIR}/include/tech/version.h
	-P ${PROJECT_SOURCE_DIR}/cmake/version.cmake
)

add_dependencies(${TARGET} extract-version)

set(TECH_LIBRARY ${TARGET} CACHE STRING "Tech static library" FORCE)
