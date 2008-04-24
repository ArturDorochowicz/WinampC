project( "WinampC" )

set( WINAMPC_VERSION_MAJOR        "1" )
set( WINAMPC_VERSION_MINOR        "2" )
set( WINAMPC_VERSION_BUILD        "1" )
set( WINAMPC_VERSION_REVISION     "0" )
set( WINAMPC_COPYRIGHT_LAST_YEAR  "2008" )


# Allow ENDIF() and ELSE() without expressions in parentheses
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

message( STATUS "Generator: ${CMAKE_GENERATOR}" )


add_subdirectory( "doc" )
add_subdirectory( "src" )



# Distribution target

set( DISTRIBUTION_DIR
	"${CMAKE_CURRENT_BINARY_DIR}/WinampC_v${WINAMPC_VERSION_MAJOR}.${WINAMPC_VERSION_MINOR}.${WINAMPC_VERSION_BUILD}" )

get_target_property( WINAMPC_LOCATION "WinampC" "LOCATION" )

add_custom_target( "Distribution"
	COMMAND
		"${CMAKE_COMMAND}"
			-E "make_directory" "${DISTRIBUTION_DIR}"
	COMMAND
		"${CMAKE_COMMAND}"
			-E "copy" "${CMAKE_CURRENT_BINARY_DIR}/doc/WinampC.html" "${DISTRIBUTION_DIR}"
	COMMAND
		"${CMAKE_COMMAND}"
			-E "copy" "${WINAMPC_LOCATION}" "${DISTRIBUTION_DIR}" )

add_dependencies( "Distribution"
	"WinampC"
	"Documentation" )
