/*
 * Helper definitions for PowerPro plugins.
 *
 * Originally created by Artur Dorochowicz and put in the Public Domain.
 *
**/


/* Definitions for PowerPro plugins */

#ifndef _POWERPRO_H_
#define _POWERPRO_H_


/* PPRO_VERSION specifies the oldest PowerPro version supported by a plugin that
 * includes this file. The version is specified as 4 digit decimal number.
 * This is the same format that is used in PowerPro variable 'pproversion'.
 */
#ifndef PPRO_VERSION
#error "PPRO_VERSION must specify the oldest PowerPro version that you intend to support."
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
 * No information available for versions between 3600 and 3700.
 */
#if PPRO_VERSION < 3700
	#define PPRO_RETURN_NOTHING_BY_DEFAULT   pp.ret[0] = '\0';
#else
	#define PPRO_RETURN_NOTHING_BY_DEFAULT
#endif


#ifdef  __cplusplus
	#define PPRO_EXTERN_C extern "C" 
#else
	#define PPRO_EXTERN_C
#endif


typedef struct PPROHELPER
{
	unsigned int argc;              /*< number of arguments */
	char **argv;                    /*< array of arguments */
	char *ret;                      /*< pointer to place for string with return value */
	size_t retsize;                 /*< size of ret buffer in bytes (532 bytes in recent versions) */
	unsigned long *flgs;            /*< pointer to PowerPro flags */
	PPROSERVICES *svcs;             /*< PowerPro services */
	BOOL (*GetVar)( char*, char* ); /*< the usual GetVar function */
	void (*SetVar)( char*, char* ); /*< the usual SetVar function */
} PPROHELPER;


/* Use these macros like this:
 * BEGIN_PPRO_SVC( my_service_name )
 * {
 *    ...
 *    service code goes here
 *    ...
 * }
 * END_PPRO_SVC
 *
 * The following meaningful variable is defined inside the block:
 *    PPROHELPER pp;
 */
#define BEGIN_PPRO_SVC( service_name ) \
	PPRO_EXTERN_C __declspec(dllexport) void service_name( LPVOID __ppro_unused1, LPVOID __ppro_unused2, \
		BOOL (*__ppro_get_var)(LPSTR, LPSTR), void (*__ppro_set_var)(LPSTR, LPSTR), \
		DWORD *__ppro_flags, UINT __ppro_argc, LPSTR *__ppro_argv, PPROSERVICES *__ppro_services ) \
	{ \
		PPROHELPER pp; \
		pp.argc = __ppro_argc; \
		pp.argv = &__ppro_argv[1]; \
		pp.ret = __ppro_argv[0]; \
		PPRO_RETURN_NOTHING_BY_DEFAULT \
		pp.retsize = PPRO_MAX_VAR_LENGTH + 1; \
		pp.svcs = __ppro_services; \
		pp.flgs = __ppro_flags; \
		pp.GetVar = __ppro_get_var; \
		pp.SetVar = __ppro_set_var; \
		/* prevent 'argument not used' warnings */ \
		(void) __ppro_unused1; (void) __ppro_unused2; (void) pp;


#define END_PPRO_SVC \
	}


#define PPRO_SVC_RETURN_INT( integer )  sprintf( pp.ret, "%d", (int) (integer) )
#define PPRO_SVC_RETURN_UINT( uinteger )  sprintf( pp.ret, "%u", (unsigned int) (uinteger) )

/* PowerPro 4.5.12 is the first version to support EncodeFloat/DecodeFloat */
#if PPRO_VERSION < 4512
#pragma message( "PPRO_SVC_RETURN_FLOAT returns string instead of PowerPro float" )
#define PPRO_SVC_RETURN_FLOAT( double_num )  sprintf( pp.ret, "%.3f", (double) (double_num) )
#else
#define PPRO_SVC_RETURN_FLOAT( double_num )  pp.svcs->EncodeFloat( (double) (double_num), pp.ret )
#endif


#endif   /* #ifndef _POWERPRO_H_ */
