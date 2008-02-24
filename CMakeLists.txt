project( "WinampC" )

# Allow ENDIF() and ELSE() without expressions in parentheses
set( CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true )

message( STATUS "Generator: ${CMAKE_GENERATOR}" )


# Get current date (output is formatted as YYYY-MM-DD)
exec_program( "cmd.exe" ARGS "/c date /t" OUTPUT_VARIABLE CURRENT_YEAR )
# leave year only
string( REGEX MATCH "[0-9]+" CURRENT_YEAR "${CURRENT_YEAR}" )


set( WINAMPC_VERSION_MAJOR     "1" )
set( WINAMPC_VERSION_MINOR     "2" )
set( WINAMPC_VERSION_BUILD     "0" )
set( WINAMPC_VERSION_REVISION  "0" )
set( WINAMPC_VERSION_STRING    "\"${WINAMPC_VERSION_MAJOR}.${WINAMPC_VERSION_MINOR}.${WINAMPC_VERSION_BUILD}\"" )

set( WINAMPC_COPYRIGHT_YEARS_STRING "\"2003-${CURRENT_YEAR}\"" )

add_subdirectory( "doc" )
add_subdirectory( "src" )
