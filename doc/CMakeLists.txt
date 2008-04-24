
# Microsoft PowerShell. If installed, should be in PATH.
find_program( MSH "powershell" )

if( NOT MSH )
	message( "Cannot find 'powershell' executable." )
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


add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/services.xml"
	COMMAND
		"${MSH}" 
			"${CMAKE_CURRENT_SOURCE_DIR}/extract-services-descriptions.ps1"
			"${CMAKE_CURRENT_BINARY_DIR}/services.xml"			
			"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c"
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/extract-services-descriptions.ps1"
		"${CMAKE_CURRENT_SOURCE_DIR}/../src/WinampC.c"
	COMMENT	
		"Extracting services' descriptions from source code files into an XML file" )


add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
	COMMAND
		"${XSLT}"
			--output "${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
			--novalid
			--nonet
			--stringparam "servicesPath" "${CMAKE_CURRENT_BINARY_DIR}/services.xml"
			--stringparam "pluginVersion" "${WINAMPC_VERSION_MAJOR}.${WINAMPC_VERSION_MINOR}.${WINAMPC_VERSION_BUILD}"
			--stringparam "lastCopyrightYear" "${WINAMPC_COPYRIGHT_LAST_YEAR}"
			"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xsl"
			"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xml"
	DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xsl"
		"${CMAKE_CURRENT_SOURCE_DIR}/WinampC.xml"
		"${CMAKE_CURRENT_BINARY_DIR}/services.xml"
	COMMENT
		"Applying XSL transform to produce documentation in XHTML" )


# Due to some problems with doctype emitted by Tidy (no system indentifier and
# no way to specify one), we provide it ourselves.
add_custom_command( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/WinampC.html"
	COMMAND
		"${CMAKE_COMMAND}"
			"-E echo \"<!DOCTYPE HTML PUBLIC \\\"-//W3C//DTD HTML 4.01//EN\\\" \\\"http://www.w3.org/TR/html4/strict.dtd\\\">\""
			" > \"${CMAKE_CURRENT_BINARY_DIR}/WinampC.html\""
	COMMAND
		"${TIDY}"
			-quiet
			-utf8
			-indent
			-wrap "100"
			--doctype "omit"
			--break-before-br "no"
			-ashtml
			"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
			>> "${CMAKE_CURRENT_BINARY_DIR}/WinampC.html"
	DEPENDS
		"${CMAKE_CURRENT_BINARY_DIR}/WinampC.xhtml"
	COMMENT
		"Running Tidy to produce final HTML documentation from XHTML" )


if( MSH AND XSLT AND TIDY )
	add_custom_target( "Documentation"
		DEPENDS
			"${CMAKE_CURRENT_BINARY_DIR}/WinampC.html" )
else( )
	message( "The tools necessary to build documentation are not present." )
	message( "Documentation target will not be created." )
endif( )
