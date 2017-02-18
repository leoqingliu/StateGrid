#pragma once


#ifdef __cplusplus
extern "C" {
#endif

	typedef BOOL (CDECL *pfnIsTaskKeysDisabled)();
	typedef BOOL (CDECL *pfnDisableTaskKeys)(BOOL bDisable);
	
#define DLL_LINK(var, type, name)  var = (type)GetProcAddress(m_hApiLib, name); \
	if (NULL == var) break

#ifdef __cplusplus
}
#endif

extern pfnIsTaskKeysDisabled	_IsTaskKeysDisabled;
extern pfnDisableTaskKeys		_DisableTaskKeys;

BOOL InitApiHookLibrary(LPCTSTR pPath);
VOID UnInitApiHookLibrary();
