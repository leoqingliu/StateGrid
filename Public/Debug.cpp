#include <StdAfx.h>
#include <tchar.h>
#include <assert.h>
#include <TlHelp32.h>
#include "Debug.h"

#ifdef _OUTPUT_DEBUG_

#pragma warning(push)
// 4996: “vswprintf”被声明为否决的
#pragma warning(disable:4996)

CDebug::CDebug(void)
{
	m_szDebug = NULL;
	m_szPre = NULL;
}

CDebug::~CDebug(void)
{
	if (m_szDebug)
		delete[] m_szDebug;
	if (m_szPre)
		delete[] m_szPre;
}

void CDebug::Print(LPCTSTR lpszFormat, ...)
{
	static bool bInited = false;
	if(!bInited)
	{
		bInited = true;
		m_szDebug = new TCHAR[4096];
		m_szPre = new TCHAR[128];
		
		//
		memset(m_szPre, 0, sizeof(m_szPre));
		
		HANDLE hProcessSnap = NULL;
		PROCESSENTRY32 pe32 = {0};
		
		//
		// Take a snapshot of all processes in the system.    
		//
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    
		if(hProcessSnap == (HANDLE)-1)    
		{
			return;
		}
		
		//
		// Fill in the size of the structure before using it.
		//
		pe32.dwSize = sizeof(PROCESSENTRY32);    
		
		//
		// Walk the snapshot of the processes, and for each process,    
		// display information.    
		//
		DWORD dwServices = 0;
		if(Process32First(hProcessSnap, &pe32))    
		{  
			while(1)  
			{
				if(pe32.th32ProcessID == GetCurrentProcessId())
				{
					wsprintf(m_szPre, _T("[%s:%d] "), pe32.szExeFile, pe32.th32ProcessID);
					break;
				}
				
				if(!Process32Next(hProcessSnap, &pe32))
				{
					break;
				}
			}
		}
	}
	
	//
	lstrcpy(m_szDebug, m_szPre);
	
	va_list args;
	int		nBuf;
	va_start(args, lpszFormat);
	//nBuf = _vstprintf_s(m_szDebug, _countof(m_szDebug), lpszFormat, args); 
	
#ifdef UNICODE
	nBuf = vswprintf(m_szDebug + lstrlen(m_szPre), lpszFormat, args);
#else
	nBuf = vsprintf(m_szDebug + lstrlen(m_szPre), lpszFormat, args);
#endif
	
	//assert(nBuf >= 0);
	va_end(args);
	
	OutputDebugString(m_szDebug);
}

#pragma warning(pop)

CDebug m_Debug;
#endif