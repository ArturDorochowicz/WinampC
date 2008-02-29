/* Definitions for PowerPro plugins */

#ifndef _POWERPRO_H_
#define _POWERPRO_H_


/* PPRO_VERSION specifies the oldest PowerPro version supported by the plugin
 * that includes this file. The version is specified as 4 digit decimal number.
 * This is the same format that is used by PowerPro variable 'pproversion'.
 */
#ifndef PPRO_VERSION
#error "PPRO_VERSION must specify the oldest PowerPro version that you intent to support."
#endif

/* 3500 (and earlier) is not supported because it has no PPROSERVICES struct and
 * the service function prototype is different than in all later versions.
 * The actual first supported version might be something between 3500 and 3600,
 * but I had no access to any version between these two.
 */
#if PPRO_VERSION < 3600
#error "The first supported PowerPro version is 3600. Check your PPRO_VERSION definition."
#endif


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/* For sprintf in PPRO_SVC_RETURN_* macros */
#include <stdio.h>


/* Maximum length of PowerPro variable in single byte characters.
 * The total buffer size is one byte greater.
 */
/* 3600 uses the first one, whereas 3700 and newer use the second.
 * The change might have been somewhere between 3600 and 3700, but I had no
 * access to any version between these two.
 */
#if PPRO_VERSION >= 3700
#define PPRO_MAX_VAR_LENGTH 531
#else
#define PPRO_MAX_VAR_LENGTH 263
#endif

/* The maximum number of arguments that a service can receive */
#if PPRO_VERSION >= 4404
#define PPRO_MAX_SVC_ARGS 24
#else
#define PPRO_MAX_SVC_ARGS 10
#endif


typedef struct PPROSERVICES
{
/* no struct at all in 3500, no versions available to check between 3500 and 3600 */
#if PPRO_VERSION >= 3600
	void (*ErrMessage)(LPSTR, LPSTR);
	BOOL (*MatchCaption)(HWND, LPSTR);
	HWND (*FindMatchingWindow)(LPSTR,BOOL);
	BOOL (*IsRolled)(HWND hw);
	BOOL (*IsTrayMinned)(HWND hw);
	void (*GetExeFullPath)(HWND hw, LPSTR szt);
	void (*RollUp)(HWND hw);
	void (*TrayMin)(HWND hw);
	void (*SendKeys)(LPSTR sz);
#endif
/* no versions available to check between 3600 and 3700 */
#if PPRO_VERSION >= 3700
	BOOL (*EvalExpr)(LPSTR sz, LPSTR szo);
	void (*Debug)(LPSTR sz1, LPSTR sz2,LPSTR sz3, LPSTR sz4, LPSTR sz5, LPSTR sz6);
	LPSTR (*AllocTemp)(UINT leng);
	void (*ReturnString)(LPSTR sz, LPSTR* szargs);
	LPSTR (*GetVarAddr)(LPSTR var);
	LPSTR (*SetVar)(LPSTR var, LPSTR val);
	void (*IgnoreNextClip)();
	void (*Show)(HWND h);
	void (*RunCmd)(LPSTR szCmd, LPSTR szParam, LPSTR szWork);
	BOOL (*InsertStringForBar)(LPSTR szStr, LPSTR szCmd);
	void (*ResetFocus)();
#endif
/* no versions available to check between 3800 and 3821 */
#if PPRO_VERSION >= 3821   /* checked up to 4304 */
	HWND (*NoteOpen)(LPSTR szFile, LPSTR szKeyWords, BOOL bActivate);
	BOOL (*PumpMessages)();
#endif
/* version 4305 not checked - could not find it */
#if PPRO_VERSION >= 4306
	BOOL (*RegForConfig)(void ( *callback )(LPSTR szList), BOOL bReg );
	void (*SetPreviousFocus)(HWND h );
	UINT (*SetDebug)(LPSTR sz,LPSTR sz2 );
	UINT (*ScriptCancel)(LPSTR sz );
	void (*GetCurrentDir)(HWND h,LPSTR szt);
#endif
#if PPRO_VERSION >= 4310
	void (*RegisterNonModal)(HWND h,BOOL b);
	UINT (*GetVarSize)(LPSTR p);
#endif
#if PPRO_VERSION >= 4405
	/* versions 4405 and 4406 define this entry differently than later versions */
	#if PPRO_VERSION == 4405 || PPRO_VERSION == 4406
	BOOL (*RegisterLocalFree)(BOOL b, LPSTR sig, void (*callback)(LPSTR sz));
	#else
	BOOL (*RegisterSigOld)(BOOL b, LPSTR sig, LPSTR sig2, LPSTR szPlugName,
		void (*callback)(LPSTR sz), LPSTR szGet, LPSTR szSet,LPSTR szDo );
	#endif
#endif
#if PPRO_VERSION >= 4407
	void (*FreeIfHandle)(LPSTR sz);
	int (*LastMouse)(UINT u);
#endif
#if PPRO_VERSION >= 4409
	BOOL (*RegisterSig)(BOOL b, LPSTR sig, LPSTR sig2, LPSTR szPlugName,
		void (*callback)(LPSTR sz), LPSTR szGet, LPSTR szSet,LPSTR szDo,LPSTR szSetDo );
#endif
#if PPRO_VERSION >= 4411
	void (*ReturningNewHandle)();
#endif
#if PPRO_VERSION >= 4412
	LPSTR (*GetStaticVarAddr)(LPSTR sz, LPSTR szScript);
	BOOL (*SetStaticVar)(LPSTR szName, LPSTR szScript, LPSTR szv,BOOL bCreate);
#endif
#if PPRO_VERSION >= 4415
	HWND (*ActiveMenu)(LPSTR szCap, LPSTR szSwit);
#endif
#if PPRO_VERSION >= 4512
	void (*SaveClip)(LPSTR szFileName, BOOL bTextOnly, BOOL bVerbose);
	void (*LoadClip)(LPSTR szFileName, BOOL bTextOnly, BOOL bVerbose);
	LPSTR (*EncodeFloat)(double x, LPSTR szBuff);
	double (*DecodeFloat)(LPSTR szBuff);
#endif
#if PPRO_VERSION >= 4513   /* checked up to 4703 */
	void (*GetCaretPosScreen)(HWND h, POINT*pt);
#endif
/* version 4704 not checked - could not find it */
#if PPRO_VERSION >= 4705
	void (*SetAddRefCallback)(LPSTR sig1,void (*refcallback)(LPSTR sz, BOOL addref));
	void (*ChangeIfHandle)(LPSTR sz, BOOL b);
	BOOL (*CallScriptFile)(LPSTR szPath, int narg, LPSTR* pargs);
	LPSTR (*LoadScriptFile)(LPSTR szPath);
#endif
#if PPRO_VERSION >= 4707   /* checked up to 4800 */
	BOOL (*RegisterForMouseUpDown)(BOOL b, void (*callback)(UINT msg, UINT msg2));
	LPSTR (*GetOutputAddr)(LPSTR szPath);
#endif
} PPROSERVICES;


/* Help file for 3600 says that one should set **szargs to '\0' if one doesn't
 * want to return anything. Help in 3700 does not mention that.
 * No information available for versions between 3600 and 3700
 */
#if PPRO_VERSION < 3700
#define PPRO_RETURN_NOTHING_BY_DEFAULT   retval[0] = '\0';
#else
#define PPRO_RETURN_NOTHING_BY_DEFAULT
#endif


/* Use these macros like this:
 * BEGIN_PPRO_SVC( my_service_name )
 * {
 *    ...
 *    service code goes here
 *    ...
 * }
 * END_PPRO_SVC
 *
 * The following meaningful variables are defined inside the block:
 * BOOL (*GetVar)(LPSTR, LPSTR) - the usual GetVar function
 * void (*SetVar)(LPSTR, LPSTR) - the usual SetVar function
 * DWORD *ppro_flags - pointer to PowerPro flags
 * PPROSERVICES *ppro_svcs - PowerPro services
 * UINT argc - number of arguments
 * char **argv - array of service arguments
 * char *retval - place for string with return value
 * size_t retval_size - size of retval buffer in bytes (532 bytes in recent versions)
 */
#define BEGIN_PPRO_SVC( service_name ) \
	_declspec(dllexport) void service_name( LPVOID ppro_unused1, LPVOID ppro_unused2, \
		BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD *ppro_flags, \
		UINT argc, LPSTR *ppro_args, PPROSERVICES *ppro_svcs ) \
	{ \
		static const size_t retval_size = PPRO_MAX_VAR_LENGTH + 1; \
		char *retval = ppro_args[0]; \
		char **argv = &ppro_args[1]; \
		PPRO_RETURN_NOTHING_BY_DEFAULT \
		/* prevent 'argument not used' warnings */ \
		(void) ppro_unused1; (void) ppro_unused2; (void) GetVar; (void) SetVar; \
		(void) ppro_flags; (void) argc; (void) ppro_args; (void) ppro_svcs; \
		(void) retval_size; (void) retval; (void) argv;

#define END_PPRO_SVC  }


#define PPRO_SVC_RETURN_INT( integer )  sprintf( retval, "%d", (int) (integer) );
#define PPRO_SVC_RETURN_UINT( uinteger )  sprintf( retval, "%u", (unsigned int) (uinteger) );

#if PPRO_VERSION < 4512
#pragma message( "PPRO_SVC_RETURN_FLOAT returns string instead of PowerPro float" )
#define PPRO_SVC_RETURN_FLOAT( double_num )  sprintf( retval, "%.3f", (double) (double_num) );
#else
#define PPRO_SVC_RETURN_FLOAT( double_num )  ppro_svcs->EncodeFloat( (double) (double_num), retval );
#endif


#endif   /* #ifndef _POWERPRO_H_ */
