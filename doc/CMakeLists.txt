
# Get current date (output is formatted as YYYY-MM-DD)
exec_program( "cmd.exe" ARGS "/c date /t" OUTPUT_VARIABLE CURRENT_YEAR )
# leave year only
string( REGEX MATCH "[0-9]+" CURRENT_YEAR "${CURRENT_YEAR}" )

# pcregrep.exe and pcre3.dll are in package PCRE for Windows
# http://gnuwin32.sourceforge.net/packages/pcre.htm
# Normal grep won't handle lookarounds in the regex used here
find_program( GREP "pcregrep"
	PATHS "${CMAKE_CURRENT_SOURCE_DIR}/tool" )

if( NOT GREP )
	message( "Cannot find 'pcregrep' executable." )
endif( )

find_program( XSLT "xsltproc"
	PATHS "${CMAKE_CURRENT_SOURCE_DIR}/tool" )

if( NOT XSLT )
	message( "Cannot find 'xsltproc' executable." )
endif( )

# Command that creates services.xml by grepping the source code file.
add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/services.xml" 
	COMMAND
		"${GREP}"
			"--file=\"${CMAKE_CURRENT_SOURCE_DIR}/pattern.txt\""
			"--only-matching"
			"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c"
			" > \"${CMAKE_CURRENT_BINARY_DIR}/services.xml\""
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/pattern.txt" )

add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
	COMMAND
		"${XSLT}"
			"--output \"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml\""
			"--novalid"
			"--stringparam \"servicesPath\" \"${CMAKE_CURRENT_BINARY_DIR}/services.xml\""
			"--stringparam \"lastCopyrightYear\" \"${CURRENT_YEAR}\""
			"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xsl"
			"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xml"
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xsl"
		"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xml"
		"${CMAKE_CURRENT_BINARY_DIR}/services.xml" )

if( XSLT AND GREP )

	add_custom_target( "documentation"
		DEPENDS
			"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml" )

else( )

	message( "The tools necessary to build documentation are not present." )
	message( "Documentation target will not be created." )

endif( )
