include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(TARGET ${PROJECT_NAME})

# Common sources
set(SOURCES
    bytearray.cpp
    char.cpp
    duration.cpp
    format.cpp
	item.cpp
    logger.cpp
    string.cpp
    thread.cpp
	)

set(HEADERS
	../include/tech/bytearray.h
	../include/tech/char.h
	../include/tech/circbuffer.h
	../include/tech/delegate.h
	../include/tech/duration.h
	../include/tech/flags.h
	../include/tech/format.h
	../include/tech/item.h
	../include/tech/logger.h
	../include/tech/passkey.h
	../include/tech/pimpl.h
	../include/tech/scopedrollback.h
	../include/tech/scopeexit.h
	../include/tech/semaphore.h
	../include/tech/signal.h
	../include/tech/string.h
	../include/tech/thread.h
	../include/tech/timecounter.h
	../include/tech/traits.h
	../include/tech/types.h
	../include/tech/utils.h
	)

if(TECH_ENABLE_HASH)
	list(APPEND SOURCES
		hash/crc.cpp
		)

	list(APPEND HEADERS
		../include/tech/hash/crc.h
		)
endif()


if(TECH_ENABLE_SYSTEM)
	list(APPEND SOURCES
		system/calendartime.cpp
		system/dispatcher.cpp
		system/timer.cpp
		system/timezone.cpp
	)

	list(APPEND HEADERS
		../include/tech/system/calendartime.h
		../include/tech/system/dispatcher.h
		../include/tech/system/timer.h
		../include/tech/system/timezone.h
	)
endif()


if(TECH_ENABLE_UI)
	find_package(Cairo REQUIRED)
	find_package(Pango REQUIRED)

	include_directories(
		${CAIRO_INCLUDE_DIRS}
		${PANGO_INCLUDE_DIRS}
	)

	list(APPEND SOURCES
		ui/button.cpp
	    ui/color.cpp
	    ui/font.cpp
	    ui/fontmetrics.cpp
		ui/gradient.cpp
		ui/image.cpp
	    ui/layout.cpp
	    ui/painter.cpp
		ui/palette.cpp
		ui/rowlayout.cpp
		ui/style.cpp
	    ui/timer.cpp
	    ui/widget.cpp
	    ui/windowsystem.cpp
	)

	list(APPEND HEADERS
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
		../include/tech/ui/palette.h
		../include/tech/ui/point.h
		../include/tech/ui/rect.h
		../include/tech/ui/rowlayout.h
		../include/tech/ui/size.h
		../include/tech/ui/style.h
		../include/tech/ui/timer.h
		../include/tech/ui/widget.h
	)

	# Libraries to link with
	set(LIBRARIES
		${CAIRO_LIBRARIES}
		${PANGO_LIBRARIES}
	)
endif()


if(PLATFORM_LINUX)
	list(APPEND SOURCES
		)

	if(TECH_ENABLE_SYSTEM)
		list(APPEND SOURCES
			system/dispatcher_linux.cpp
			system/timer_linux.cpp
			system/timezone_linux.cpp
			)

		list(APPEND HEADERS
			system/dispatcher_linux.h
			system/timer_linux.h
			system/timezone_linux.h
			)
	endif()

	if(TECH_ENABLE_UI)
		find_package(Xcb COMPONENTS XCB ICCCM REQUIRED)

		include_directories(
			${XCB_XCB_INCLUDE_DIR}
			${XCB_ICCCM_INCLUDE_DIR}
		)

		list(APPEND SOURCES
			ui/windowsystem_linux.cpp
		)

		list(APPEND LIBRARIES
			${XCB_XCB_LIBRARY}
			${XCB_ICCCM_LIBRARY}
		)
	endif()
elseif(PLATFORM_WINDOWS)
	list(APPEND SOURCES
		system/timezone_windows.cpp
		ui/windowsystem_windows.cpp
	)
elseif(PLATFORM_OSX)
    list(APPEND SOURCES
		system/timezone_osx.cpp
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
