/* Definitions for PowerPro plugins */

#ifndef _POWERPRO_H_
#define _POWERPRO_H_


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>


/* Maximum length of PowerPro variable in single byte characters */
#define PPRO_MAX_VAR_LENGTH 531


typedef struct PPROSERVICES
{
	void (*ErrMessage)(LPSTR, LPSTR);
	BOOL (*MatchCaption)(HWND, LPSTR);
	HWND (*FindMatchingWindow)(LPSTR,BOOL);
	BOOL (*IsRolled)(HWND hw);
	BOOL (*IsTrayMinned)(HWND hw);
	void (*GetExeFullPath)(HWND hw, LPSTR szt);
	void (*RollUp)(HWND hw);
	void (*TrayMin)(HWND hw);
	void (*SendKeys)(LPSTR sz);
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
	HWND (*NoteOpen)(LPSTR szFile, LPSTR szKeyWords, BOOL bActivate);
	BOOL (*PumpMessages)();
	BOOL (*RegForConfig)(void ( *callback )(LPSTR szList), BOOL bReg );
	void (*SetPreviousFocus)(HWND h );
	UINT (*SetDebug)(LPSTR sz,LPSTR sz2 );
	UINT (*ScriptCancel)(LPSTR sz );
	void (*GetCurrentDir)(HWND h,LPSTR szt);
	void (*RegisterNonModal)(HWND h,BOOL b);
	UINT (*GetVarSize)(LPSTR p);
	BOOL (*RegisterSigOld)(BOOL b, LPSTR sig, LPSTR sig2, LPSTR szPlugName,
		void (*callback)(LPSTR sz), LPSTR szGet, LPSTR szSet,LPSTR szDo );
	void (*FreeIfHandle)(LPSTR sz);
	int (*LastMouse)(UINT u);
	BOOL (*RegisterSig)(BOOL b, LPSTR sig, LPSTR sig2, LPSTR szPlugName,
		void (*callback)(LPSTR sz), LPSTR szGet, LPSTR szSet,LPSTR szDo,LPSTR szSetDo );
	void (*ReturningNewHandle)();
	LPSTR (*GetStaticVarAddr)(LPSTR sz, LPSTR szScript);
	BOOL (*SetStaticVar)(LPSTR szName, LPSTR szScript, LPSTR szv,BOOL bCreate);
	HWND (*ActiveMenu)(LPSTR szCap, LPSTR szSwit);
	void (*SaveClip)(LPSTR szFileName, BOOL bTextOnly, BOOL bVerbose);
	void (*LoadClip)(LPSTR szFileName, BOOL bTextOnly, BOOL bVerbose);
	LPSTR (*EncodeFloat)(double x, LPSTR szBuff);
	double (*DecodeFloat)(LPSTR szBuff);
	void (*GetCaretPosScreen)(HWND h, POINT*pt);
	void (*SetAddRefCallback)(LPSTR sig1,void (*refcallback)(LPSTR sz, BOOL addref));
	void (*ChangeIfHandle)(LPSTR sz, BOOL b);
	BOOL (*CallScriptFile)(LPSTR szPath, int narg, LPSTR* pargs);
	LPSTR (*LoadScriptFile)(LPSTR szPath);
	BOOL (*RegisterForMouseUpDown)(BOOL b, void (*callback)(UINT msg, UINT msg2));
	LPSTR (*GetOutputAddr)(LPSTR szPath);
} PPROSERVICES;


#define BEGIN_PPRO_SVC( service_name ) \
	_declspec(dllexport) void service_name( LPVOID ppro_unused1, LPVOID ppro_unused2, \
		BOOL (*GetVar)(LPSTR, LPSTR), void (*SetVar)(LPSTR, LPSTR), DWORD *ppro_flags, \
		UINT argc, LPSTR *ppro_args, PPROSERVICES *ppro_svcs ) \
	{ \
		static const size_t retval_size = PPRO_MAX_VAR_LENGTH + 1; \
		char *retval = ppro_args[0]; \
		char **argv = &ppro_args[1]; \
		/* prevent 'argument not used' warnings */ \
		(void) ppro_unused1; (void) ppro_unused2; (void) GetVar; (void) SetVar; \
		(void) ppro_flags; (void) argc; (void) ppro_args; (void) ppro_svcs; \
		(void) retval_size; (void) retval; (void) argv; \
		/* by default return nothing (probably not needed) */ \
		retval[0] = '\0';


#define END_PPRO_SVC  }


#endif   /* #ifndef _POWERPRO_H_ */
