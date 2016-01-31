find_package(Git QUIET)
if(NOT GIT_FOUND)
	message(STATUS "git executable not found")
	return()
endif()

execute_process(
	COMMAND "${GIT_EXECUTABLE}" describe --tags --dirty=* --always
	WORKING_DIRECTORY "${ROOT_DIR}"
	RESULT_VARIABLE RESULT
	OUTPUT_VARIABLE VERSION
	ERROR_QUIET
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT RESULT EQUAL 0)
	# Change VERSION here for out-of-repository sources distribution
	set(VERSION "0.0.0-unknown")
	message(STATUS "Unable to get project version from git, using hardcoded value")
endif()

# Filter useless crap from the "git describe" output
string(REGEX REPLACE
	"(^[0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9]+)?)-[0-9]+-g([a-fA-F0-9]+(-dirty)?)"
	"\\1-\\3" VERSION "${VERSION}")

# Extract version variables
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.[0-9]+-(.*)?" "\\1" VERSION_EXTRA "${VERSION}")

if(${VERSION_EXTRA} MATCHES ${VERSION})
	set(VERSION_EXTRA "")
endif()

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPERCASE)
string(REGEX REPLACE "-" "_" PROJECT_NAME_UPPERCASE "${PROJECT_NAME_UPPERCASE}")

configure_file(${VERSION_CONFIG} ${VERSION_HEADER})
message(STATUS "${PROJECT_NAME} version: ${VERSION}")
