
find_program( PCREGREP "pcregrep"
	PATHS "${CMAKE_CURRENT_SOURCE_DIR}/tool" )

if( NOT PCREGREP )
	message( "Cannot find 'pcregrep' executable." )
endif( )

find_program( MSXSL "msxsl"
	PATHS "${CMAKE_CURRENT_SOURCE_DIR}/tool" )

if( NOT MSXSL )
	message( "Cannot find 'msxsl' executable." )
endif( )

# Command that creates services.xml by grepping the source code file.
add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/services.xml" 
	COMMAND
		"${PCREGREP}"
			"--file=\"${CMAKE_CURRENT_SOURCE_DIR}/pattern.txt\""
			"--only-matching"
			"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c"
			" > \"${CMAKE_CURRENT_BINARY_DIR}/services.xml\""
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c" )

# Command that creates final documentation by running XSL transform
# in WinampC.xsl on WinampC.xml. The transform uses services.xml.
add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
	COMMAND
		"${MSXSL}"
			"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xml"
			"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xsl"
			"-o \"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml\""
	DEPENDS
		"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xsl"
		"${CMAKE_CURRENT_BINARY_DIR}/services.xml" )

if( MSXSL AND PCREGREP )

	# Prepare XSL file
	# CONFIGURE_FILE is done at configuration time!
	set( CMAKE_PATH_SERVICES_XML "${CMAKE_CURRENT_BINARY_DIR}/services.xml" )
	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xsl.cmake"
		"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xsl"
		@ONLY )


	add_custom_target( "documentation"
		DEPENDS
			"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml" )

else( )

	message( "The tools necessary to build documentation are not present." )
	message( "Documentation target will not be created." )

endif( )
