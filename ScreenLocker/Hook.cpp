#include "StdAfx.h"
#include "Hook.h"

HINSTANCE m_hApiLib;
pfnIsTaskKeysDisabled	_IsTaskKeysDisabled;
pfnDisableTaskKeys		_DisableTaskKeys;

BOOL InitApiHookLibrary(LPCTSTR pPath)
{
	m_hApiLib = LoadLibrary(pPath);
	if (NULL == m_hApiLib)
	{
		return FALSE;
	}

	BOOL bLoadRet = FALSE;
	do
	{
		DLL_LINK(_IsTaskKeysDisabled,	pfnIsTaskKeysDisabled,		"IsTaskKeysDisabled");
		DLL_LINK(_DisableTaskKeys,		pfnDisableTaskKeys,			"DisableTaskKeys");
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

VOID UnInitApiHookLibrary()
{
	if (NULL != m_hApiLib)
	{
		FreeLibrary(m_hApiLib);
		m_hApiLib = NULL;
	}
}
