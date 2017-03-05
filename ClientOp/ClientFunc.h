#pragma once


#ifdef __cplusplus
extern "C" {
#endif

	typedef BOOL (CDECL *pfnInstallService)();
	typedef BOOL (CDECL *pfnUnInstallService)();

#define DLL_LINK(var, type, name)  var = (type)GetProcAddress(m_hApiLib, name); \
	if (NULL == var) break;

#ifdef __cplusplus
}
#endif

extern pfnInstallService	_InstallService;
extern pfnUnInstallService	_UnInstallService;

BOOL InitClientLibrary(LPCTSTR pPath);
VOID UnInitClientLibrary();

