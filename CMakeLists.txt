project( "WinampC" )

# Allow ENDIF() and ELSE() without expressions in parentheses
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

message( STATUS "Generator: ${CMAKE_GENERATOR}" )


# Get current date (output is formatted as YYYY-MM-DD)
# Need to do this explicitly through shell, because neither EXEC_PROGRAM
# nor EXECUTE_PROCESS seem to use it.
exec_program( "$ENV{SystemRoot}/system32/cmd.exe" ARGS "/c date /t" OUTPUT_VARIABLE CURRENT_YEAR )
# leave year only
string( REGEX MATCH "[0-9]+" CURRENT_YEAR "${CURRENT_YEAR}" )


set( WINAMPC_VERSION_MAJOR     "1" )
set( WINAMPC_VERSION_MINOR     "2" )
set( WINAMPC_VERSION_BUILD     "0" )
set( WINAMPC_VERSION_REVISION  "0" )


add_subdirectory( "doc" )
add_subdirectory( "src" )



# Distribution target

set( DISTRIBUTION_DIR
	"${CMAKE_CURRENT_BINARY_DIR}/WinampC_v${WINAMPC_VERSION_MAJOR}.${WINAMPC_VERSION_MINOR}.${WINAMPC_VERSION_BUILD}" )

get_target_property( WINAMPC_LOCATION "WinampC" "LOCATION" )

add_custom_target( "Distribution"
	COMMAND
		"${CMAKE_COMMAND}"
			"-E make_directory \"${DISTRIBUTION_DIR}\""
	COMMAND
		"${CMAKE_COMMAND}"
			"-E copy \"${CMAKE_CURRENT_BINARY_DIR}/doc/WinampC.xhtml\" \"${DISTRIBUTION_DIR}\"" 
	COMMAND
		"${CMAKE_COMMAND}"
			"-E copy \"${WINAMPC_LOCATION}\" \"${DISTRIBUTION_DIR}\""
	COMMAND
		"${CMAKE_COMMAND}"
			"-E copy \"${CMAKE_CURRENT_SOURCE_DIR}/licence.txt\" \"${DISTRIBUTION_DIR}\"" )

add_dependencies( "Distribution"
	"WinampC"
	"Documentation" )
