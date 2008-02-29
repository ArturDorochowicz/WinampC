/*
/*  Winamp Control Plugin
/*
/*  Copyright Artur Dorochowicz
/*
/**/


/* PowerPro 4.5.12 is the first version to support EncodeFloat/DecodeFloat */
#define PPRO_VERSION 4512
#include <powerpro.h>

/* Winamp SDK */
#include <Winamp/wa_ipc.h>
#include <lang_b/resource.h>

#include <stdio.h>


typedef enum ResponseType
{
	ResponseTypeNone = 0,
	ResponseTypeShowErrorMsg,
	ResponseTypeSetFlag,
	ResponseTypeReturnGivenMsg
} ResponseType;


/* MSDN doesn't specify if it includes the terminating null character or not.
 * Assuming it does not.
 */
#define MAX_WND_CLASS_NAME_LENGTH 256 


static HWND FindWinampWindow( const char *window_class )
{
	static const char default_class[] = "Winamp v1.x";
	
	if( window_class != NULL && window_class[0] != '\0' )
		return FindWindow( window_class, NULL );
	else
		return FindWindow( default_class, NULL );
}


/** Safe string copy.
 *  Destination string is always null-terminated (unless its buffer size is 0 ).
 *  @param dst The destination.
 *  @param dst_size The destination buffer size in bytes.
 *  @param src The source.
**/
static void strcpys( char *dst, size_t dst_size, const char *src )
{
	if( src != 0 && dst_size != 0 && dst != NULL )
	{
		while( dst_size > 0 && *src != '\0' )
		{
			*dst = *src;
			dst++;
			src++;
			dst_size--;
		}

		*dst = '\0';
	}
}


static const char* RequiredArgsCountInfo( int required_count )
{
	switch( required_count )
	{
	case 2:
		return "The service needs two arguments when using both optional arguments.\n"
			"It needs one argument when omitting 'class type'.\n"
			"It needs no arguments when omitting 'class type' and 'response type'.";
	case 3:
		return "The service needs three arguments when using both optional arguments.\n"
			"It needs two arguments when omitting 'class type'.\n"
			"It needs one argument when omitting 'class type' and 'response type'.";
	case 4:
		return "The service needs four arguments when using both optional arguments.\n"
			"It needs three arguments when omitting 'class type'.\n"
			"It needs two arguments when omitting 'class type' and 'response type'.";
	default:
		return "";
	}
}


/**
 *  @param required_count Includes two optional arguments.
**/
static BOOL CheckArgCount( const char **args, int num_args, int required_count,
	PPROSERVICES *ppro_svcs, char *window_class, size_t window_class_size,
	ResponseType *response_type, char *response_msg, size_t response_msg_size )
{
	BOOL rv = TRUE;

	if( num_args > required_count )
	{
		const char *error_msg1 = "Too many arguments for the service.\n";
		const char *error_msg2 = RequiredArgsCountInfo( required_count );
		ppro_svcs->ErrMessage( (LPSTR) error_msg1, (LPSTR) error_msg2 );
		rv = FALSE;   /* error */
	}
	else if( num_args < required_count - 2 )
	{
		const char *error_msg1 = "Not enough arguments for the service.\n";
		const char *error_msg2 = RequiredArgsCountInfo( required_count );
		ppro_svcs->ErrMessage( (LPSTR) error_msg1, (LPSTR) error_msg2 );
		rv = FALSE;   /* error */
	}
	else if( num_args >= required_count - 1 )   /* at least one optional present */
	{
		const char *response_type_arg = args[required_count - 2];
		
		if( response_type_arg[0] == '3' )
		{
			*response_type = ResponseTypeReturnGivenMsg;
			strcpys( response_msg, response_msg_size, &response_type_arg[1] );
		}
		else if( response_type_arg[0] == '2' && response_type_arg[1] == '\0' )
		{
			*response_type = ResponseTypeSetFlag;
		}
		else if( response_type_arg[0] == '1' && response_type_arg[1] == '\0' )
		{
			*response_type = ResponseTypeShowErrorMsg;
		}

		if( num_args == required_count )   /* both optional present */
		{
			strcpys( window_class, window_class_size, args[required_count - 1] );
		}
	}
	/* else no optional arguments were specified */

	return rv;
}


static void PerformResponse( ResponseType response_type, const char *response_msg,
	HWND winamp_wnd, PPROSERVICES *ppro_svcs, DWORD *ppro_flags, char *retval,
	size_t retval_size )
{
	if( winamp_wnd != NULL )   /* window found */
	{
		/* if response_type is 2, set flag f0 to 1 */
		if( ResponseTypeSetFlag == response_type )
		{
			*ppro_flags |= 1;
		}			
	}
	else   /* window not found */
	{
		/* if response type is 1 then show window with error explanation */
		if( ResponseTypeShowErrorMsg == response_type )
		{
			ppro_svcs->ErrMessage( "Specified Winamp window does not exist.", "" );
		}
		/* if response type is 2 then set flag f0 to 0 */
		else if( ResponseTypeSetFlag == response_type )
		{
			*ppro_flags &= ~1;
		}
		/* if response type is 3abcde... then return abcde... */
		else if( ResponseTypeReturnGivenMsg == response_type )
		{
			strcpys( retval, retval_size, response_msg );
		}
	}           
}


/** Read a string from process memory.
 *  @param wnd The window handle by which the process is found.
 *  @param src_start The address of the beginning of the string.
 *  @param dst The destination buffer.
 *  @param dst_size The size of the destination buffer.
**/
void ReadStringFromProcessMemory( HWND wnd, const void *src_start, char *dst, size_t dst_size )
{
	if( dst_size > 0 )
	{
		DWORD process_id;
		HANDLE process;

		dst[0] = '\0';

		GetWindowThreadProcessId( wnd, &process_id );
		process = OpenProcess( PROCESS_VM_READ, 0, process_id );
		if( process != INVALID_HANDLE_VALUE )
		{
			SIZE_T bytes_read;

			ReadProcessMemory( process, src_start, dst, dst_size, &bytes_read );
			CloseHandle( process );

			if( bytes_read > 0 )
			{
				dst[bytes_read - 1] = '\0';
			}
		}
	}
}


static HWND Startup( PPROSERVICES *ppro_svcs, DWORD *ppro_flags, char **args,
	unsigned int num_args, unsigned int num_args_required, char *retval,
	size_t retval_size )
{
	char wnd_class[MAX_WND_CLASS_NAME_LENGTH + 1] = { 0 };
	ResponseType response_type = ResponseTypeNone;
	char response_msg[PPRO_MAX_VAR_LENGTH + 1] = { 0 };
	HWND winamp_wnd = NULL;

	if( TRUE == CheckArgCount( args, num_args, num_args_required, ppro_svcs, wnd_class,
		sizeof( wnd_class ), &response_type, response_msg, sizeof( response_msg ) ) )
	{
		winamp_wnd = FindWinampWindow( wnd_class );
		PerformResponse( response_type, response_msg, winamp_wnd, ppro_svcs,
			ppro_flags, retval, retval_size );
	}

	return winamp_wnd;
}


#define STARTUP( num_args_required ) \
	HWND winamp_wnd = Startup( ppro_svcs, ppro_flags, argv, argc, \
		(num_args_required) + 2, retval, retval_size ); \
	if( NULL == winamp_wnd ) return;


/* Argument types used in documentation are:
   string - string value, possible PowerPro arguments: integer (is converted to string), string 
   int - integer value, possible PowerPro arguments: integer, string (containing an integer), float (is truncated to integer)
   float - float value, possible PowerPro arguments: integer, string (containing a float), float
*/


/*! <service name="add_bookmark">
/*!  <description>Add specified file/URL to Winamp's bookmark list.</description>
/*!  <requirements>Winamp 2.4+</requirements>
/*!  <argument name="file" type="string">File path or URL.</argument>
/*! </service> */
BEGIN_PPRO_SVC( add_bookmark )
{
	COPYDATASTRUCT cds;

	STARTUP( 1 );

	cds.dwData = IPC_ADDBOOKMARK;
	cds.lpData = argv[0];
	cds.cbData = strlen( argv[0] ) + 1;
	SendMessage( winamp_wnd, WM_COPYDATA, 0, (LPARAM) &cds );
}
END_PPRO_SVC


/* Works but requires Unicode input */
BEGIN_PPRO_SVC( add_bookmark_w )
{
	COPYDATASTRUCT cds;

	STARTUP( 1 );

	cds.dwData = IPC_ADDBOOKMARKW;
	cds.lpData = argv[0];
	cds.cbData = ( wcslen( (wchar_t*) argv[0] ) + 1 ) * sizeof( wchar_t);
	SendMessage( winamp_wnd, WM_COPYDATA, 0, (LPARAM) &cds );
}
END_PPRO_SVC


/*! <service name="add_track_as_bookmark">
/*!  <description>Add current track as bookmark.</description>
/*! </service> */
BEGIN_PPRO_SVC( add_track_as_bookmark )
{
	HWND winamp_wnd = Startup( ppro_svcs, ppro_flags, argv, argc, 2, retval, retval_size );
	if( NULL == winamp_wnd )
		return;
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_MAKECURBOOKMARK, 0 );
}
END_PPRO_SVC


/*! <service name="autoload_preset_dialog">
/*!  <description>Open autoload presets dialog. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( autoload_preset_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, IDM_EQ_LOADMP3, 0 );
}
END_PPRO_SVC


/*! <service name="autoload_save_preset_dialog">
/*!  <description>Open autoload preset save dialog. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( autoload_save_preset_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, IDM_EQ_SAVEMP3, 0 );
}
END_PPRO_SVC


/*! <service name="block_minibrowser">
/*!  <description>Block the Minibrowser from updates.</description>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( block_minibrowser )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_WA_IPC, 1, IPC_MBBLOCK );
}
END_PPRO_SVC


/*! <service name="caption">
/*!  <description>Get full caption of the Winamp's window. This is something like: 'Number. Song title - Winamp'.
/*!   Obviously, Winamp's option 'Scroll song title in the Windows taskbar' influences the output from this service.
/*!  </description>
/*!  <return-value type="string">Winamp's window caption.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( caption )
{
	STARTUP( 0 );
	GetWindowText( winamp_wnd, retval, retval_size );
}
END_PPRO_SVC


/*! <service name="get_plist_entry_path">
/*!  <description>Get full path of playlist entry at specified position.</description>
/*!  <requirements>Winamp 2.04+</requirements>
/*!  <argument name="index" type="int">The position. First entry is at 1.</argument>
/*!  <return-value type="string">The path.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_plist_entry_path )
{
	WPARAM index;
	char *file_path;

	STARTUP( 1 );

	/* change 1-based input into Winamp's 0-based index */
	index = (WPARAM) ppro_svcs->DecodeFloat( argv[0] ) - 1;
	file_path = (char*) SendMessage( winamp_wnd, WM_WA_IPC, index, IPC_GETPLAYLISTFILE );
	if( file_path != NULL )
	{
		ReadStringFromProcessMemory( winamp_wnd, file_path, retval, retval_size );
	}
}
END_PPRO_SVC


/*! <service name="get_plist_entry_title">
/*!  <description>Get title of playlist entry at specified position.</description>
/*!  <requirements>Winamp 2.04+</requirements>
/*!  <argument name="index" type="int">The position. First entry is at 1.</argument>
/*!  <return-value type="string">The title.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_plist_entry_title )
{
	WPARAM index;
	const char *title;

	STARTUP( 1 );

	/* change 1-based input into Winamp's 0-based index */
	index = (WPARAM) ppro_svcs->DecodeFloat( argv[0] ) - 1;
	title = (const char*) SendMessage( winamp_wnd, WM_WA_IPC, index, IPC_GETPLAYLISTTITLE );
	if( title != NULL )
	{
		ReadStringFromProcessMemory( winamp_wnd, title, retval, retval_size );
	}
}
END_PPRO_SVC


/*! <service name="get_plist_selected_path">
/*!  <description> Get full path of currently selected playlist entry.</description>
/*!  <requirements>Winamp 2.04+</requirements>
/*!  <return-value type="string">The path.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_plist_selected_path )
{
	LRESULT index;
	char *file_path;
	
	STARTUP( 0 );

	index = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETLISTPOS );
	file_path = (char*) SendMessage( winamp_wnd, WM_WA_IPC, index, IPC_GETPLAYLISTFILE );
	if( file_path != NULL )
	{
		ReadStringFromProcessMemory( winamp_wnd, file_path, retval, retval_size );
	}
}
END_PPRO_SVC


/*! <service name="get_plist_selected_title">
/*!  <description> Get title of currently selected playlist entry.</description>
/*!  <requirements>Winamp 2.04+</requirements>
/*!  <return-value type="string">The title.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_plist_selected_title )
{
	LRESULT index;
	char *title;

	STARTUP( 0 );

	index = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETLISTPOS );
	title = (char*) SendMessage( winamp_wnd, WM_WA_IPC, index, IPC_GETPLAYLISTTITLE );
	if( title != NULL )
	{
		ReadStringFromProcessMemory( winamp_wnd, title, retval, retval_size );
	}
}
END_PPRO_SVC


/*! <service name="change_directory">
/*!  <description>Change Winamp's current working directory.</description>
/*!  <argument name="directory" type="string">The new working directory.</argument>
/*! </service> */
BEGIN_PPRO_SVC( change_directory )
{
	COPYDATASTRUCT cds;
	
	STARTUP( 1 );
	
	cds.dwData = IPC_CHDIR;
	cds.lpData = argv[0];
	cds.cbData = strlen( argv[0] ) + 1;
	SendMessage( winamp_wnd, WM_COPYDATA, 0, (LPARAM) &cds );
}
END_PPRO_SVC


/*! <service name="clear_plist">
/*!  <description>Clear Winamp's internal playlist.</description>
/*! </service> */
BEGIN_PPRO_SVC( clear_plist )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_DELETE );
}
END_PPRO_SVC


/*! <service name="close_winamp">
/*!  <description>Close Winamp.</description>
/*! </service> */
BEGIN_PPRO_SVC( close_winamp )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_FILE_QUIT, 0 );
}
END_PPRO_SVC


/*! <service name="configure_visual_plugin">
/*!  <description>Open configuration window of the current visualization plugin.</description>
/*! </service> */
BEGIN_PPRO_SVC( configure_visual_plugin )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_VISCONF, 0 );
}
END_PPRO_SVC


/*! <service name="delete_autoload_preset_dialog">
/*!  <description>Open delete an auto load preset dialog. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( delete_autoload_preset_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, IDM_EQ_DELMP3, 0 );
}
END_PPRO_SVC


/*! <service name="delete_preset_dialog">
/*!  <description>Open delete preset dialog. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( delete_preset_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, IDM_EQ_DELPRE, 0 );
}
END_PPRO_SVC


/*! <service name="display_elapsed_time">
/*!  <description>Set time display mode to elapsed time.</description>
/*! </service> */
BEGIN_PPRO_SVC( display_elapsed_time )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_ELAPSED, 0 );
}
END_PPRO_SVC


/*! <service name="display_remaining_time">
/*!  <description>Set time display mode to remaining time.</description>
/*! </service> */
BEGIN_PPRO_SVC( display_remaining_time )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_REMAINING, 0 );
}
END_PPRO_SVC


/*! <service name="end_of_plist">
/*!  <description>Play the last file on the playlist. 
/*!   When shuffle is on, it is the last played file.
/*!  </description>
/*! </service> */
BEGIN_PPRO_SVC( end_of_plist )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON5_CTRL, 0 );
}
END_PPRO_SVC


/*! <service name="enqueue_file">
/*!  <description>Enqueue file.</description>
/*!  <argument name="file" type="string">The file to enqueue.</argument>
/*! </service> */
BEGIN_PPRO_SVC( enqueue_file )
{
	COPYDATASTRUCT cds;

	STARTUP( 1 );

	cds.dwData = IPC_ENQUEUEFILE;
	cds.lpData = argv[0];
	cds.cbData = strlen( argv[0] ) + 1;
	SendMessage( winamp_wnd, WM_COPYDATA, 0, (LPARAM) &cds );
}
END_PPRO_SVC


/* Works but requires Unicode input */
BEGIN_PPRO_SVC( enqueue_file_w )
{
	COPYDATASTRUCT cds;

	STARTUP( 1 );

	cds.dwData = IPC_PLAYFILEW;
	cds.lpData = argv[0];
	cds.cbData = ( wcslen( (wchar_t*) argv[0] ) + 1 ) * sizeof( wchar_t);
	SendMessage( winamp_wnd, WM_COPYDATA, 0, (LPARAM) &cds );
}
END_PPRO_SVC


/*! <service name="execute_visual_plugin">
/*!  <description>Execute current visualization plugin.</description>
/*! </service> */
BEGIN_PPRO_SVC( execute_visual_plugin )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_VISPLUGIN, 0 );
}
END_PPRO_SVC


/*! <service name="file_open_dialog">
/*!  <description>Open 'Open file(s)' dialog.</description>
/*! </service> */
BEGIN_PPRO_SVC( file_open_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_FILE_PLAY, 0 );
}
END_PPRO_SVC


/*! <service name="flush_plist_cache_buffer">
/*!  <description>Flushes the playlist cache buffer.</description>
/*! </service> */
BEGIN_PPRO_SVC( flush_plist_cache_buffer )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_REFRESHPLCACHE );
}
END_PPRO_SVC


/*! <service name="forward_5sec">
/*!  <description>Fast forward 5 seconds.</description>
/*! </service> */
BEGIN_PPRO_SVC( forward_5sec )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_FFWD5S, 0 );
}
END_PPRO_SVC


/*! <service name="get_bitrate">
/*!  <description>Get momentary bitrate of current song.</description>
/*!  <requirements>Winamp 2.05+</requirements>
/*!  <return-value type="int">The bitrate in kbs.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_bitrate )
{
	LRESULT bitrate;

	STARTUP( 0 );

	bitrate = SendMessage( winamp_wnd, WM_WA_IPC, 1, IPC_GETINFO );
	PPRO_SVC_RETURN_UINT( bitrate );
}
END_PPRO_SVC


/*! <service name="get_eq_data">
/*!  <description>Query the status of equaliser.</description>
/*!  <argument name="position" type="int">Specifies the information to query. <br />
/*!    0&#8211;9 - The 10 bands of EQ data, <br />
/*!    10 - the preamp value, <br />
/*!    11 - enabled status, <br />
/*!    12 - autoload status.
/*!  </argument>
/*!  <return-value type="mixed">The value and its type depend on the specified argument. <br />
/*!    For 0&#8211;10 - return value is a float from -20 to +20 (db). <br />
/*!    For 11&#8211;12 - return value is an integer: 0 (disabled) or nonzero (enabled). <br />
/*!  </return-value>
/*!  <requirements>Winamp 2.05+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_eq_data )
{
	WPARAM position;
	LRESULT data;

	STARTUP( 1 );

	position = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	data = SendMessage( winamp_wnd, WM_WA_IPC, position, IPC_GETEQDATA );
	if( position >= 0 && position <= 10 )
	{
		PPRO_SVC_RETURN_FLOAT( - (double) data * (20 + 20 ) / 63 + 20 );
	}
	else
	{
		PPRO_SVC_RETURN_UINT( data );
	}
}
END_PPRO_SVC


/*! <service name="get_eq_data63">
/*!  <description>Query the status of equaliser. Data is returned exactly as received from Winamp.</description>
/*!  <argument name="position" type="int">Specifies the information to query. <br />
/*!    0&#8211;9 - The 10 bands of EQ data, <br />
/*!    10 - the preamp value, <br />
/*!    11 - enabled status, <br />
/*!    12 - autoload status.
/*!  </argument>
/*!  <return-value type="int">The value depends on the specified argument. <br />
/*!    For 0&#8211;10 - return value is an integer from 63 to 0 (which corresponds to -20&#8211;+20 db).<br />
/*!    For 11&#8211;12 - return value is 0 (disabled) or nonzero (enabled).<br />
/*!  </return-value>
/*!  <requirements>Winamp 2.05+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_eq_data63 )
{
	WPARAM position;
	LRESULT data;

	STARTUP( 1 );

	position = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	data = SendMessage( winamp_wnd, WM_WA_IPC, position, IPC_GETEQDATA );
	PPRO_SVC_RETURN_UINT( data );
}
END_PPRO_SVC


/*! <service name="get_length">
/*!  <description>Get length of the current song.</description>
/*!  <return-value type="int">The length in seconds.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_length )
{
	LRESULT length;

	STARTUP( 0 );

	length = SendMessage( winamp_wnd, WM_WA_IPC, 1, IPC_GETOUTPUTTIME );
	PPRO_SVC_RETURN_UINT( length );
}
END_PPRO_SVC


/*! <service name="get_net_status">
/*!  <description>Check if the Internet connection is available for Winamp.
/*!   Relates to the Internet connection type setting on the main general
/*!   preferences page in the Winamp preferences.
/*!  </description>
/*!  <requirements>Winamp 2.05+</requirements>
/*!  <return-value type="int">Returns 1 when connection is available, 0 otherwise.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_net_status )
{
	LRESULT status;

	STARTUP( 0 );

	status = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_INETAVAILABLE );
	PPRO_SVC_RETURN_UINT( status );
}
END_PPRO_SVC


/*! <service name="get_number_of_channels">
/*!  <description>Get number of channels of the current song.</description>
/*!  <requirements>Winamp 2.05+</requirements>
/*!  <return-value type="int">Number of channels.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_number_of_channels )
{
	LRESULT channels;
	
	STARTUP( 0 );
	
	channels = SendMessage( winamp_wnd, WM_WA_IPC, 2, IPC_GETINFO );
	PPRO_SVC_RETURN_UINT( channels );
}
END_PPRO_SVC


/*! <service name="get_panning">
/*!  <description>Get value describing panning setting.</description>
/*!  <requirements>Winamp 2.0+</requirements>
/*!  <return-value type="int">The panning value from -100 (all left) to +100 (all right).</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_panning )
{
	LRESULT panning;

	STARTUP( 0 );

	panning = SendMessage( winamp_wnd, WM_WA_IPC, (WPARAM) -666, IPC_SETPANNING );
	PPRO_SVC_RETURN_INT( panning * 100 / 127 );
}
END_PPRO_SVC


/*! <service name="get_panning127">
/*!  <description>Get value describing panning setting. Data is returned exactly as received from Winamp.</description>
/*!  <requirements>Winamp 2.0+</requirements>
/*!  <return-value type="int">The panning value from -127 (all left) to +127 (all right).</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_panning127 )
{
	LRESULT panning;

	STARTUP( 0 );

	panning = SendMessage( winamp_wnd, WM_WA_IPC, (WPARAM) -666, IPC_SETPANNING );
	PPRO_SVC_RETURN_INT( panning );
}
END_PPRO_SVC


/*! <service name="get_playback_status">
/*!  <description>Get status of the playback.</description>
/*!  <return-value type="int">Returns 1 when Winamp is playing, 3 when paused, 0 when it is not playing.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_playback_status )
{
	LRESULT status;

	STARTUP( 0 );

	status = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_ISPLAYING );
	PPRO_SVC_RETURN_UINT( status );
}
END_PPRO_SVC


/*! <service name="get_plist_length">
/*!  <description>Gets playlist length in tracks.</description>
/*!  <return-value type="int">Number of tracks in the playlist.</return-value>
/*!  <requirements>Winamp 2.0+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_plist_length )
{
	LRESULT length;

	STARTUP( 0 );

	length = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETLISTLENGTH );
	PPRO_SVC_RETURN_UINT( length );
}
END_PPRO_SVC


/*! <service name="get_plist_position">
/*!  <description>Get position in the current playlist, in tracks.</description>
/*!  <return-value type="int">The position of the current track in the playlist. First track is at 1.</return-value>
/*!  <requirements>Winamp 2.05+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_plist_position )
{
	LRESULT position;
	
	STARTUP( 0 );

	/* change Winamp's 0-based index into 1-based */
	position = 1 + SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETLISTPOS );
	PPRO_SVC_RETURN_UINT( position );
}
END_PPRO_SVC


/*! <service name="get_plist_position1">
/*!  <description>Write current playlist to %winamp.ini.dir%\Winamp.m3u and return position in the playlist.</description>
/*!  <return-value type="int">The position of the current track in the playlist. First track is at 1.</return-value>
/*!  <requirements>Winamp 1.666+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_plist_position1 )
{
	LRESULT position;

	STARTUP( 0 );

	/* change Winamp's 0-based index into 1-based */
	position = 1 + SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_WRITEPLAYLIST );
	PPRO_SVC_RETURN_UINT( position );
}
END_PPRO_SVC


/*! <service name="get_position">
/*!  <description>Get position in the current track.</description>
/*!  <return-value type="int">The position in miliseconds.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_position )
{
	LRESULT position;
	
	STARTUP( 0 );

	position = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETOUTPUTTIME );
	PPRO_SVC_RETURN_INT( position );
}
END_PPRO_SVC


/*! <service name="get_position">
/*!  <description>Get position in the current track.</description>
/*!  <return-value type="int">The position in seconds.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_position_in_sec )
{
	LRESULT position;
	
	STARTUP( 0 );

	position = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETOUTPUTTIME );
	PPRO_SVC_RETURN_INT( ( position != -1 ? ( position / 1000 ) : position ) );
}
END_PPRO_SVC


/*! <service name="get_rating">
/*!  <description>Get rating of the current track.</description>
/*!  <return-value type="int">Rating value from 0 (no rating) to 5.</return-value>
/*!  <requirements>Winamp 5.04+ with Media Library</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_rating )
{
	LRESULT rating;

	STARTUP( 0 );

	rating = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETRATING );
	PPRO_SVC_RETURN_UINT( rating );
}
END_PPRO_SVC


/*! <service name="get_repeat">
/*!  <description>Get status of the repeat option.</description>
/*!  <return-value type="int">1 - enabled or 0 - disabled.</return-value>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_repeat )
{
	LRESULT repeat;

	STARTUP( 0 );

	repeat = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GET_REPEAT );
	PPRO_SVC_RETURN_UINT( repeat );
}
END_PPRO_SVC


/*! <service name="get_samplerate">
/*!  <description>Get sample rate of the current track.</description>
/*!  <return-value type="int">Sample rate in kHz.</return-value>
/*!  <requirements>Winamp 2.05+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_samplerate )
{
	LRESULT samplerate;

	STARTUP( 0 );

	samplerate = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETINFO );
	PPRO_SVC_RETURN_UINT( samplerate );
}
END_PPRO_SVC


/*! <service name="get_samplerate_hz">
/*!  <description>Get sample rate of the current track.</description>
/*!  <return-value type="int">Sample rate in Hz.</return-value>
/*!  <requirements>Winamp 5.25+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_samplerate_hz )
{
	LRESULT samplerate;

	STARTUP( 0 );

	samplerate = SendMessage( winamp_wnd, WM_WA_IPC, 5, IPC_GETINFO );
	PPRO_SVC_RETURN_UINT( samplerate );
}
END_PPRO_SVC


/*! <service name="get_shuffle">
/*!  <description>Get status of the shuffle option.</description>
/*!  <return-value type="int">1 - enabled or 0 - disabled.</return-value>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_shuffle )
{
	LRESULT shuffle;

	STARTUP( 0 );
	
	shuffle = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GET_SHUFFLE );
	PPRO_SVC_RETURN_UINT( shuffle );
}
END_PPRO_SVC


/*! <service name="get_time_display_mode">
/*!  <description>Get time display mode.</description>
/*!  <return-value type="int">1 - displaying elapsed time, 0 - displaying remaining time.</return-value>
/*!  <requirements>Winamp 2.05+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_time_display_mode )
{
	LRESULT mode;

	STARTUP( 0 );

	mode = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETTIMEDISPLAYMODE );
	PPRO_SVC_RETURN_UINT( mode );
}
END_PPRO_SVC


/*! <service name="get_version">
/*!  <description>Get Winamp's version number.</description>
/*!  <return-value type="string">The version number in X.YZ format.
/*!   Because of the wealth of hexadecimal Winamp version formats (see get_version_hex) this service
/*!   may not always return correct results. It works by simply replacing second
/*!   digit in hexadecimal version number (which should be 0) with a dot.
/*!  </return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_version )
{
	LRESULT version;

	STARTUP( 0 );

	version = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETVERSION );
	sprintf( retval, "%X.%X", WINAMP_VERSION_MAJOR( version ), WINAMP_VERSION_MINOR( version ) );
}
END_PPRO_SVC


/*! <service name="get_version_hex">
/*!  <description>Get Winamp's version number.</description>
/*!  <return-value type="string">The version number as returned from Winamp.
/*!   Usual, expected format is X0YZ for version X.YZ. <br />
/*!   A quote from Winamp SDK: <br />
/*!   "The version returned will be 0x20yx for Winamp 2.yx.
/*!   Versions previous to Winamp 2.0 typically (but not always) use 0x1zyx for 1.zx.
/*!   Just a bit weird but that's the way it goes. <br />
/*!   For Winamp 5.x it uses the format 0x50yx for Winamp 5.yx, e.g. 5.01 -> 0x5001, 5.09 -> 0x5009, 5.1 -> 0x5010. <br />
/*!   Notes: <br />
/*!   For 5.02 this api will return the same value as for a 5.01 build. <br />
/*!   For 5.07 this api will return the same value as for a 5.06 build."
/*!  </return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_version_hex )
{
	LRESULT version;

	STARTUP( 0 );

	version = SendMessage( winamp_wnd, WM_WA_IPC, 0, IPC_GETVERSION );
	sprintf( retval, "%X", version );
}
END_PPRO_SVC


/*! <service name="get_video_height">
/*!  <description>Get height of the current video.</description>
/*!  <return-value type="int">The height.</return-value>
/*!  <requirements>Winamp 5+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_video_height )
{
	LRESULT size;

	STARTUP( 0 );

	size = SendMessage( winamp_wnd, WM_WA_IPC, 3, IPC_GETINFO );
	PPRO_SVC_RETURN_UINT( HIWORD( size ) );
}
END_PPRO_SVC


/*! <service name="get_video_width">
/*!  <description>Get width of the current video.</description>
/*!  <return-value type="int">The width.</return-value>
/*!  <requirements>Winamp 5+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( get_video_width )
{
	LRESULT size;

	STARTUP( 0 );

	size = SendMessage( winamp_wnd, WM_WA_IPC, 3, IPC_GETINFO );
	PPRO_SVC_RETURN_UINT( LOWORD( size ) );
}
END_PPRO_SVC


/*! <service name="get_volume">
/*!  <description>Get value of the volume setting.</description>
/*!  <return-value type="int">The volume as a value from 0 (mute) to 100.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_volume )
{
	LRESULT volume;

	STARTUP( 0 );

	volume = SendMessage( winamp_wnd, WM_WA_IPC, (WPARAM) -666, IPC_SETVOLUME );
	PPRO_SVC_RETURN_UINT( volume * 100 / 255 );
}
END_PPRO_SVC


/*! <service name="get_volume255">
/*!  <description>Get value of the volume setting. Data is returned exactly as received from Winamp.</description>
/*!  <return-value type="int">The volume as a value from 0 (mute) to 255.</return-value>
/*! </service> */
BEGIN_PPRO_SVC( get_volume255 )
{
	LRESULT volume;

	STARTUP( 0 );

	volume = SendMessage( winamp_wnd, WM_WA_IPC, (WPARAM) -666, IPC_SETVOLUME );
	PPRO_SVC_RETURN_UINT( volume );
}
END_PPRO_SVC


/*! <service name="jump_to_file_dialog">
/*!  <description>Open 'Jump to file' dialog.</description>
/*! </service> */
BEGIN_PPRO_SVC( jump_to_file_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_JUMPFILE, 0 );
}
END_PPRO_SVC


/*! <service name="jump_to_time">
/*!  <description>Jump to specified position in the current track.</description>
/*!  <argument name="position" type="int">Requested position in miliseconds.</argument>
/*!  <return-value type="int">Returns -1 if Winamp is not playing, 1 on end of file, or 0 if call was successful.</return-value>
/*!  <requirements>Winamp 1.60+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( jump_to_time )
{
	WPARAM position;
	LRESULT result;

	STARTUP( 1 );

	position = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	result = SendMessage( winamp_wnd, WM_WA_IPC, position, IPC_JUMPTOTIME );
	PPRO_SVC_RETURN_INT( result );
}
END_PPRO_SVC


/*! <service name="jump_to_time_dialog">
/*!  <description>Open 'Jump to time' dialog.</description>
/*! </service> */
BEGIN_PPRO_SVC( jump_to_time_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_JUMP, 0 );
}
END_PPRO_SVC


/*! <service name="load_default_preset">
/*!  <description>Load default equaliser preset. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( load_default_preset )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, IDM_EQ_LOADDEFAULT, 0 );
}
END_PPRO_SVC


/*! <service name="load_preset_dialog">
/*!  <description>Open load preset dialog. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( load_preset_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, IDM_EQ_LOADPRE, 0 );
}
END_PPRO_SVC


/*! <service name="load_preset_from_eqf">
/*!  <description>Load equaliser preset from EQF. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( load_preset_from_eqf )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, ID_LOAD_EQF, 0 );
}
END_PPRO_SVC


static void MinimizeWindow( HWND wnd )
{
	SendMessage( wnd, WM_SYSCOMMAND, SC_MINIMIZE, 0 );
}


static void RestoreWindow( HWND wnd, PPROSERVICES *ppro_svcs )
{
	ppro_svcs->Show( wnd );
	SetForegroundWindow( wnd);
}


/*! <service name="minimize">
/*!  <description>Minimize the Winamp's window.</description>
/*! </service> */
BEGIN_PPRO_SVC( minimize )
{
	STARTUP( 0 );

	MinimizeWindow( winamp_wnd );
}
END_PPRO_SVC


/*! <service name="min_restore">
/*!  <description>Minimize the Winamp's window if it is not minimized, restore if minimized.</description>
/*! </service> */
BEGIN_PPRO_SVC( min_restore )
{
	STARTUP( 0 );

	if( IsIconic( winamp_wnd ) )
	{		
		RestoreWindow( winamp_wnd, ppro_svcs );
	}
	else
	{
		MinimizeWindow( winamp_wnd );
	}
}
END_PPRO_SVC


/*! <service name="move_10_tracks_back">
/*!  <description>Move 10 tracks back and start playing.</description>
/*! </service> */
BEGIN_PPRO_SVC( move_10_tracks_back )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_JUMP10BACK, 0 );
}
END_PPRO_SVC


/*! <service name="move_10_tracks_forward">
/*!  <description>Move 10 tracks forward and start playing.</description>
/*! </service> */
BEGIN_PPRO_SVC( move_10_tracks_forward )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_JUMP10FWD, 0 );
}
END_PPRO_SVC


/*! <service name="next_track">
/*!  <description>Start the next track.</description>
/*! </service> */
BEGIN_PPRO_SVC( next_track )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON5, 0 );
}
END_PPRO_SVC


/*! <service name="open_about_box">
/*!  <description>Open 'About Winamp' window.</description>
/*! </service> */
BEGIN_PPRO_SVC( open_about_box )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_HELP_ABOUT, 0 );
}
END_PPRO_SVC


/*! <service name="open_file_info_box">
/*!  <description>Open file info window.</description>
/*! </service> */
BEGIN_PPRO_SVC( open_file_info_box )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_EDIT_ID3, 0 );
}
END_PPRO_SVC


/*! <service name="open_preferences">
/*!  <description>Open preferences window.</description>
/*! </service> */
BEGIN_PPRO_SVC( open_preferences )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_PREFS, 0 );
}
END_PPRO_SVC


/*! <service name="open_skin_selector">
/*!  <description>Open skin selector window.</description>
/*! </service> */
BEGIN_PPRO_SVC( open_skin_selector )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_SELSKIN, 0 );
}
END_PPRO_SVC


/*! <service name="open_url_dialog">
/*!  <description>Open 'Open location' dialog.</description>
/*! </service> */
BEGIN_PPRO_SVC( open_url_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON2_CTRL, 0 );
}
END_PPRO_SVC


/*! <service name="open_visual_options">
/*!  <description>Open visualization options. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( open_visual_options )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_VISSETUP, 0 );
}
END_PPRO_SVC


/*! <service name="open_visual_plugin_options">
/*!  <description>Open visualization plug-in options.</description>
/*! </service> */
BEGIN_PPRO_SVC( open_visual_plugin_options )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_PLGSETUP, 0 );
}
END_PPRO_SVC


/*! <service name="pause_unpause">
/*!  <description>Pause when playing, start playing when paused.</description>
/*! </service> */
BEGIN_PPRO_SVC( pause_unpause )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON3, 0 );
}
END_PPRO_SVC


/*! <service name="play_audio_cd">
/*!  <description>Begin playing Audio CD.</description>
/*! </service> */
BEGIN_PPRO_SVC( play_audio_cd )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_COMMAND, ID_MAIN_PLAY_AUDIOCD, 0 );
}
END_PPRO_SVC


/*! <service name="play_any_audio_cd">
/*!  <description>Begin playing Audio CD in specified drive.</description>
/*!  <argument name="drive" type="int">The drive number. First CD drive in the system is at 0. Supports up to 4 drives (thus accepted values are: 0, 1, 2, 3.)</argument>
/*! </service> */
BEGIN_PPRO_SVC( play_any_audio_cd )
{
	unsigned int drive;
	UINT msg;

	STARTUP( 1 );

	drive = (unsigned int) ppro_svcs->DecodeFloat( argv[0] );

	switch( drive )
	{
	case 0:  msg = ID_MAIN_PLAY_AUDIOCD;   break;
	case 1:  msg = ID_MAIN_PLAY_AUDIOCD2;  break;
	case 2:  msg = ID_MAIN_PLAY_AUDIOCD3;  break;
	case 3:  msg = ID_MAIN_PLAY_AUDIOCD4;  break;
	default: return;
	}

	PostMessage( winamp_wnd, WM_COMMAND, msg, 0 );
}
END_PPRO_SVC


/*! <service name="play_button">
/*!  <description>Same as hitting play button.
/*!   There's also play_selected service (see below).
/*!   According to Nullsoft there are some differences between them.
/*!  </description>
/*! </service> */
BEGIN_PPRO_SVC( play_button )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON2, 0 );
}
END_PPRO_SVC


/*! <service name="play_selected">
/*!  <description>Begin playing selected track.</description>
/*! </service> */
BEGIN_PPRO_SVC( play_selected )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_STARTPLAY );
}
END_PPRO_SVC


/*! <service name="previous_track">
/*!  <description>Start previous track.</description>
/*! </service> */
BEGIN_PPRO_SVC( previous_track )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON1, 0 );
}
END_PPRO_SVC


/*! <service name="reload_current_skin">
/*!  <description>Reload current skin.</description>
/*! </service> */
BEGIN_PPRO_SVC( reload_current_skin )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_REFRESHSKIN, 0 );
}
END_PPRO_SVC


/*! <service name="repeat_off">
/*!  <description>Turn repeat off.</description>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( repeat_off )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_SET_REPEAT );
}
END_PPRO_SVC


/*! <service name="repeat_on">
/*!  <description>Turn repeat on.</description>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( repeat_on )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 1, IPC_SET_REPEAT );
}
END_PPRO_SVC


/*! <service name="restart_winamp">
/*!  <description>Restart Winamp.</description>
/*!  <requirements>Winamp 2.2+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( restart_winamp )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_RESTARTWINAMP );
}
END_PPRO_SVC


/*! <service name="restore">
/*!  <description>Restore the Winamp's window and make it foreground.</description>
/*! </service> */
BEGIN_PPRO_SVC( restore )
{
	STARTUP( 0 );
	RestoreWindow( winamp_wnd, ppro_svcs );
}
END_PPRO_SVC


/*! <service name="rewind_5sec">
/*!  <description>Rewind 5 seconds.</description>
/*! </service> */
BEGIN_PPRO_SVC( rewind_5sec )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_REW5S, 0 );
}
END_PPRO_SVC


/*! <service name="save_preset_dialog">
/*!  <description>Open save preset dialog. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( save_preset_dialog )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, IDM_EQ_SAVEPRE, 0 );
}
END_PPRO_SVC


/*! <service name="save_preset_to_eqf">
/*!  <description>Save equaliser preset to EQF. Doesn't seem to work on Winamp 2.80.</description>
/*! </service> */
BEGIN_PPRO_SVC( save_preset_to_eqf )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, ID_SAVE_EQF, 0 );
}
END_PPRO_SVC


/*! <service name="set_eq_data">
/*!  <description>Change the status of equaliser.</description>
/*!  <argument name="position" type="int">Specifies the information to alter. <br />
/*!    0&#8211;9 - The 10 bands of EQ data, <br />
/*!    10 - the preamp value, <br />
/*!    11 - enabled status, <br />
/*!    12 - autoload status.
/*!  </argument>
/*!  <argument name="value" type="mixed">The accepted value and its type depend on the 'position' argument. <br />
/*!   For 0&#8211;10 specify a float from -20 to +20 (db). <br />
/*!   For 11&#8211;12 specify 0 to disable or 1 to enable.
/*!  </argument>
/*!  <requirements>Winamp 2.92+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_eq_data )
{
	BYTE pos;
	WORD value63;
	WPARAM position;

	STARTUP( 2 );

	pos = (BYTE) ppro_svcs->DecodeFloat( argv[0] );
	if( pos >= 0 && pos <= 10 )
	{
		double value = ppro_svcs->DecodeFloat( argv[1] );
		value63 = (WORD) ( ( -value + 20 ) * 63 / ( 20 + 20 ) );
	}
	else
	{
		value63 = (WORD) ppro_svcs->DecodeFloat( argv[1] );
	}
	position = MAKEWPARAM( value63, MAKEWORD( pos, 0xDB ) );
	PostMessage( winamp_wnd, WM_WA_IPC, position, IPC_SETEQDATA );
}
END_PPRO_SVC


/*! <service name="set_eq_data63">
/*!  <description>Change the status of equaliser using Winamp's data format.</description>
/*!  <argument name="position" type="int">Specifies the information to alter. <br />
/*!    0&#8211;9 - The 10 bands of EQ data, <br />
/*!    10 - the preamp value, <br />
/*!    11 - enabled status, <br />
/*!    12 - autoload status.
/*!  </argument>
/*!  <argument name="value" type="int">The accepted value depends on the 'position' argument. <br />
/*!   For 0&#8211;10 specify a number from 63 to 0 (which corresponds to -20&#8211;+20 db). <br />
/*!   For 11&#8211;12 specify 0 to disable or 1 to enable.
/*!  </argument>
/*!  <requirements>Winamp 2.92+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_eq_data63 )
{
	BYTE pos;
	WORD value;
	WPARAM position;

	STARTUP( 2 );

	pos = (BYTE) ppro_svcs->DecodeFloat( argv[0] );
	value = (WORD) ppro_svcs->DecodeFloat( argv[1] );
	position = MAKEWPARAM( value, MAKEWORD( pos, 0xDB ) );
	PostMessage( winamp_wnd, WM_WA_IPC, position, IPC_SETEQDATA );
}
END_PPRO_SVC


/*! <service name="set_panning">
/*!  <description>Change panning setting.</description>
/*!  <argument name="panning" type="int">The panning value from -100 (all left) to +100 (all right).</argument>
/*!  <requirements>Winamp 2.0+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_panning )
{
	WPARAM panning;

	STARTUP( 1 );

	panning = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	PostMessage( winamp_wnd, WM_WA_IPC, panning * 100 / 127, IPC_SETPANNING );
}
END_PPRO_SVC


/*! <service name="set_panning127">
/*!  <description>Change panning setting using Winamp's data format.</description>
/*!  <argument name="panning" type="int">The panning value from -127 (all left) to +127 (all right).</argument>
/*!  <requirements>Winamp 2.0+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_panning127 )
{
	WPARAM panning;

	STARTUP( 1 );

	panning = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	PostMessage( winamp_wnd, WM_WA_IPC, panning, IPC_SETPANNING );
}
END_PPRO_SVC


/*! <service name="set_plist_position">
/*!  <description>Set position in the playlist.</description>
/*!  <argument name="position" type="int">The position to set. First playlist entry is at 1.</argument>
/*!  <requirements>Winamp 2.0+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_plist_position )
{
	WPARAM position;

	STARTUP( 1 );

	/* change 1-based input into Winamp's 0-based index */
	position = (WPARAM) ( ppro_svcs->DecodeFloat( argv[0] ) - 1 );
	PostMessage( winamp_wnd, WM_WA_IPC, position, IPC_SETPLAYLISTPOS );
}
END_PPRO_SVC


/*! <service name="set_rating">
/*!  <description>Set rating of the current track.</description>
/*!  <argument name="rating" type="int">The rating value from 0 (no rating) to 5.</argument>
/*!  <requirements>Winamp 5.04+ with Media Library</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_rating )
{
	WPARAM rating;

	STARTUP( 1 );
	
	rating = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	PostMessage( winamp_wnd, WM_WA_IPC, rating, IPC_SETRATING );
}
END_PPRO_SVC


/*! <service name="set_volume">
/*!  <description>Change volume.</description>
/*!  <argument name="volume" type="int">The volume value from 0 (mute) to 100.</argument>
/*!  <requirements>Winamp 2.0+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_volume )
{
	WPARAM volume;
	WPARAM volume255;

	STARTUP( 1 );
	
	volume = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	volume255 = (WPARAM) ( volume * 2.55 );
	if( volume255 / 2.55 < volume )
		++volume255;
	PostMessage( winamp_wnd, WM_WA_IPC, volume255, IPC_SETVOLUME );
}
END_PPRO_SVC


/*! <service name="set_volume255">
/*!  <description>Change volume using Winamp's data format.</description>
/*!  <argument name="volume" type="int">The volume value from 0 (mute) to 255.</argument>
/*!  <requirements>Winamp 2.0+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( set_volume255 )
{
	WPARAM volume;

	STARTUP( 1 );

	volume = (WPARAM) ppro_svcs->DecodeFloat( argv[0] );
	PostMessage( winamp_wnd, WM_WA_IPC, volume, IPC_SETVOLUME );
}
END_PPRO_SVC


/*! <service name="show_edit_bookmarks">
/*!  <description>Show edit bookmarks window.</description>
/*! </service> */
BEGIN_PPRO_SVC( show_edit_bookmarks )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_EDIT_BOOKMARKS, 0 );
}
END_PPRO_SVC


/*! <service name="shuffle_off">
/*!  <description>Turn shuffle off.</description>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( shuffle_off )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_SET_SHUFFLE );
}
END_PPRO_SVC


/*! <service name="shuffle_on">
/*!  <description>Turn shuffle on.</description>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( shuffle_on )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 1, IPC_SET_SHUFFLE );
}
END_PPRO_SVC


/** Trim the ending " - Winamp"
**/
static void trim_caption_end( char *caption )
{
	static const char winamp[] = " - Winamp";
	char *p = &caption[strlen( caption ) - strlen( winamp )];

	if( p >= caption && 0 == _stricmp( p, winamp ) )
	{
		p = '\0';
	}
}


/*! <service name="song_and_number">
/*!  <description>Get the track title with preceding playlist entry number. The information is retrieved from Winamp's window caption.
/*!   In order for this to work properly, Winamp's option 'Scroll song title in the Windows taskbar'
/*!   must be disabled. Otherwise the result is undefined.
/*!  </description>
/*! </service> */
BEGIN_PPRO_SVC( song_and_number )
{
	char caption[600];
	STARTUP( 0 );

	GetWindowText( winamp_wnd, caption, sizeof( caption ) );
	trim_caption_end( caption );
	strcpys( retval, retval_size, caption );
}
END_PPRO_SVC


/*! <service name="song_name">
/*!  <description>Get the title of the current track. The information is retrieved from Winamp's window caption.
/*!   In order for this to work properly, Winamp's option 'Scroll song title in the Windows taskbar'
/*!   must be disabled. Otherwise the result is undefined.
/*!  </description>
/*! </service> */
BEGIN_PPRO_SVC( song_name )
{	
	char caption[600];
	char *p;

	STARTUP( 0 );

	GetWindowText( winamp_wnd, caption, sizeof( caption ) );

	trim_caption_end( caption );

	/* trim the beginning number "XX. " */
	p = strstr( caption, ". " );
	
	if( p != NULL )
	{
		strcpys( retval, retval_size , p + 1 );
	}
}
END_PPRO_SVC


/*! <service name="start_of_plist">
/*!  <description>Play the first file on the playlist. When shuffle is on it is the first played file.
/*!  </description>
/*! </service> */
BEGIN_PPRO_SVC( start_of_plist )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON1_CTRL, 0 );
}
END_PPRO_SVC


/*! <service name="stop">
/*!  <description>Stop playback.</description>
/*! </service> */
BEGIN_PPRO_SVC( stop )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON4, 0 );
}
END_PPRO_SVC


/*! <service name="stop_after_current">
/*!  <description>Stop playback after current track.</description>
/*! </service> */
BEGIN_PPRO_SVC( stop_after_current )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON4_CTRL, 0 );
}
END_PPRO_SVC


/*! <service name="stop_with_fadeout">
/*!  <description>Stop playback with fadeout.</description>
/*! </service> */
BEGIN_PPRO_SVC( stop_with_fadeout )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_BUTTON4_SHIFT, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_doublesize">
/*!  <description>Toggle double size option.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_doublesize )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_DSIZE, 0 );
}
END_PPRO_SVC

/*! <service name="toggle_easymove">
/*!  <description>Toggle easy move option.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_easymove )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_EASYMOVE, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_eq_window">
/*!  <description>Toggle equalizer window.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_eq_window )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_EQ, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_main_window">
/*!  <description>Toggle visibility of the Winamp's main window.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_main_window )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_MAIN_WINDOW, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_minibrowser">
/*!  <description>Toggle visibility of the minibrowser window.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_minibrowser )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_MINIBROWSER, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_on_top">
/*!  <description>Toggle always on top option.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_on_top )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_AOT, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_plist_window">
/*!  <description>Toggle playlist window.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_plist_window )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_PLEDIT, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_plist_windowshade">
/*!  <description>Toggle window shade of the playlist window.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_plist_windowshade )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_WINDOWSHADE_PL, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_repeat">
/*!  <description>Toggle repeat setting.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_repeat )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_FILE_REPEAT, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_shuffle">
/*!  <description>Toggle shuffle setting.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_shuffle )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_FILE_SHUFFLE, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_title_scrolling">
/*!  <description>Toggle title scrolling option.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_title_scrolling )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_TOGGLE_AUTOSCROLL, 0 );
}
END_PPRO_SVC


/*! <service name="toggle_windowshade">
/*!  <description>Toggle window shade option of the Winamp's main window.</description>
/*! </service> */
BEGIN_PPRO_SVC( toggle_windowshade )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_OPTIONS_WINDOWSHADE, 0 );
}
END_PPRO_SVC


/*! <service name="unblock_minibrowser">
/*!  <description>Unblock the Minibrowser from updates.</description>
/*!  <requirements>Winamp 2.4+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( unblock_minibrowser )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_MBBLOCK );
}
END_PPRO_SVC


/*! <service name="update_info">
/*!  <description>Make Winamp update the information about the current track.</description>
/*!  <requirements>Winamp 2.2+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( update_info )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_UPDTITLE );
}
END_PPRO_SVC


/*! <service name="volume_down">
/*!  <description>Decrease the volume a bit.</description>
/*! </service> */
BEGIN_PPRO_SVC( volume_down )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_VOLUMEDOWN, 0 );
}
END_PPRO_SVC


/*! <service name="volume_up">
/*!  <description>Increase the volume a bit.</description>
/*! </service> */
BEGIN_PPRO_SVC( volume_up )
{
	STARTUP( 0 );
	PostMessage( winamp_wnd, WM_COMMAND, WINAMP_VOLUMEUP, 0 );
}
END_PPRO_SVC


/*! <service name="windows_disable">
/*!  <description>Disable all main Winamp windows. Works with simple skin only.</description>
/*!  <requirements>Winamp 2.9+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( windows_disable )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 0xdeadbeef, IPC_ENABLEDISABLE_ALL_WINDOWS );
}
END_PPRO_SVC


/*! <service name="windows_enable">
/*!  <description>Enable back Winamp windows.</description>
/*!  <requirements>Winamp 2.9+</requirements>
/*! </service> */
BEGIN_PPRO_SVC( windows_enable )
{
	STARTUP( 0 );

	PostMessage( winamp_wnd, WM_WA_IPC, 0, IPC_ENABLEDISABLE_ALL_WINDOWS );
}
END_PPRO_SVC
