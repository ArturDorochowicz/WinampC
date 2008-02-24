message( STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}" )

if( MSVC60 )
	if( NOT CMAKE_BUILD_TYPE STREQUAL "Distribution" )
		set( CMAKE_BUILD_TYPE "Distribution" )
		message( STATUS "Changed CMAKE_BUILD_TYPE to: ${CMAKE_BUILD_TYPE}" )
	endif( )		
	set( CMAKE_C_FLAGS_DISTRIBUTION "/MD /O1 /Ob1 /Og /GD /GR- /GX- /Gi-" )
	set( LINK_FLAGS_DISTRIBUTION "/INCREMENTAL:NO /OPT:NOWIN98 /OPT:REF /OPT:ICF" )
endif( )

if( MSVC80 OR MSVC90 )
	add_definitions( "-D_CRT_SECURE_NO_DEPRECATE" )
endif( )

configure_file( "version.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/version.h" )
include_directories( BEFORE "${CMAKE_CURRENT_BINARY_DIR}" )

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/Winamp SDK" )

set( WinampSdkFiles
	"${CMAKE_CURRENT_SOURCE_DIR}/include/Winamp SDK/Winamp/wa_ipc.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/Winamp SDK/lang_b/resource.h" )

# Add source group (filter in Visual Studio) that contains Winamp SDK files.
SOURCE_GROUP( "Winamp SDK" FILES ${WinampSdkFiles} )

set( SOURCES
	"WinampC.c"
	"Resources.rc"
	"version.h.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/version.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/powerpro.h"
	${WinampSdkFiles} )


add_library( "WinampC" SHARED ${SOURCES} )
set_target_properties( "WinampC" PROPERTIES LINK_FLAGS_DISTRIBUTION "${LINK_FLAGS_DISTRIBUTION}" )
