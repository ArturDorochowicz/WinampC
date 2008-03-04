
# pcregrep.exe and pcre3.dll are in package PCRE for Windows
# http://gnuwin32.sourceforge.net/packages/pcre.htm
# Normal grep won't handle lookarounds in the regex used here
find_program( GREP "pcregrep"
	PATHS "${CMAKE_CURRENT_SOURCE_DIR}/tool" )

if( NOT GREP )
	message( "Cannot find 'pcregrep' executable." )
endif( )

# xsltproc.exe is part of libxml2 library
# http://www.zlatkovic.com/pub/libxml/
find_program( XSLT "xsltproc"
	PATHS "${CMAKE_CURRENT_SOURCE_DIR}/tool" )

if( NOT XSLT )
	message( "Cannot find 'xsltproc' executable." )
endif( )

# tidy.exe http://tidy.sourceforge.com
find_program( TIDY "tidy"
	PATHS "${CMAKE_CURRENT_SOURCE_DIR}/tool" )

if( NOT TIDY )
	message( "Cannot find 'tidy' executable." )
endif( )


# Command that creates services.xml by grepping the source code file.
add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/services.xml" 
	COMMAND
		"${CMAKE_COMMAND}"
			"-E echo \"<services>\""
			" > \"${CMAKE_CURRENT_BINARY_DIR}/services.xml\""
	COMMAND
		"${GREP}"
			--file="${CMAKE_CURRENT_SOURCE_DIR}/pattern.txt"
			--only-matching
			"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c"
			>> "${CMAKE_CURRENT_BINARY_DIR}/services.xml"
	COMMAND
		"${CMAKE_COMMAND}"
			"-E echo \"</services>\""
			" >> \"${CMAKE_CURRENT_BINARY_DIR}/services.xml\""
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/pattern.txt" )


# Command that applies XSL transform
add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
	COMMAND
		"${XSLT}"
			--output "${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
			--novalid
			--stringparam "servicesPath" "${CMAKE_CURRENT_BINARY_DIR}/services.xml"
			--stringparam "pluginVersion" "${WINAMPC_VERSION_MAJOR}.${WINAMPC_VERSION_MINOR}.${WINAMPC_VERSION_BUILD}"
			--stringparam "lastCopyrightYear" "${CURRENT_YEAR}"
			"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xsl"
			"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xml"
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xsl"
		"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xml"
		"${CMAKE_CURRENT_BINARY_DIR}/services.xml" )


# Tidy the output from XSL transform
add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WinampC.html"
	COMMAND
		"${TIDY}"
			-quiet
			-utf8
			-indent
			-wrap "100"
			--doctype "strict"
			--break-before-br "no"
			-ashtml
			-output "${CMAKE_CURRENT_BINARY_DIR}/WinampC.html"
			"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
	DEPENDS
		"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml" )


if( XSLT AND GREP AND TIDY )
	add_custom_target( "Documentation"
		DEPENDS
			"${CMAKE_CURRENT_BINARY_DIR}/WinampC.html" )
else( )
	message( "The tools necessary to build documentation are not present." )
	message( "Documentation target will not be created." )
endif( )
