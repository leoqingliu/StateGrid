#include "StdAfx.h"
#include "ClientFunc.h"

HINSTANCE m_hApiLib;
pfnInstallService	_InstallService;
pfnUnInstallService	_UnInstallService;

BOOL InitClientLibrary(LPCTSTR pPath)
{
	m_hApiLib = LoadLibrary(pPath);
	if (NULL == m_hApiLib)
	{
		return FALSE;		
	}
	
	BOOL bLoadRet = FALSE;
	do
	{		
		DLL_LINK(_InstallService,	pfnInstallService,		"InstallService");
		DLL_LINK(_UnInstallService,	pfnUnInstallService,	"UnInstallService");
		bLoadRet = TRUE;
	}
	while (FALSE);
	if (FALSE == bLoadRet)
	{
		FreeLibrary(m_hApiLib);
		m_hApiLib = NULL;
	}
	return bLoadRet;
}

VOID UnInitClientLibrary()
{
	if (NULL != m_hApiLib)
	{
		FreeLibrary(m_hApiLib);
		m_hApiLib = NULL;
	}
}
