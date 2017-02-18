#pragma once

#define _OUTPUT_DEBUG_
#ifdef _OUTPUT_DEBUG_

class CDebug
{
public:
	CDebug(void);
	~CDebug(void);
	
private:
	//
	//
	//
	LPTSTR	m_szDebug;
	
	//
	//
	//
	LPTSTR	m_szPre;
	
public:
	//
	// !!! This is Valid in WINCE Platform
	// Print Debug Info
	//
	void Print(LPCTSTR lpszFormat, ...);
};

extern CDebug m_Debug;
#define OutputLog m_Debug.Print
#else
#define OutputLog
#endif
