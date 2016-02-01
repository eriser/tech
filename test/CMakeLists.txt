set(TARGET ${PROJECT_NAME}-test)

find_package(GTest REQUIRED)

include_directories(
	SYSTEM ${TECH_INCLUDE_DIRS}
	${GTEST_INCLUDE_DIR}
)

set(SOURCES
	types_test.cpp
	typetraits_test.cpp
	utils_test.cpp
	delegate_test.cpp
	bytearray_test.cpp
	string_test.cpp
	format_test.cpp
)

set(LIBRARIES
	${GTEST_LIBRARIES}
	${GTEST_MAIN_LIBRARIES}
	${TECH_LIBRARY}
)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} ${LIBRARIES})

add_test(TechTest ${TARGET})

if(CMAKE_CONFIGURATION_TYPES)
	add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
		--force-new-ctest-process --output-on-failure --build-config "$<CONFIGURATION>")
else()
	add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
		--force-new-ctest-process --output-on-failure)
endif()

add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND make check)

#add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND}  --verbose)
