
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include <cstring>
#include <vector>
using namespace std;
#include <string>
using namespace std;
#ifdef _UNICODE
#define stringx wstring
#else
#define stringx string
#endif

#include "../Public/Debug.h"

#define LEO_DEBUG
#ifdef LEO_DEBUG
#include <fstream>
#include <mutex>
static std::mutex g_logLock;
static std::ofstream g_ofs;
static std::wofstream g_wofs;
#define LOG_TO_FILE(str) { \
       std::lock_guard<std::mutex> wl(g_logLock);      \
       g_ofs.open("G:\\log.txt", std::ios::app);       \
       g_ofs<<str<<std::endl;                                        \
       g_ofs.close();                                                       \
}
#define LOG_TO_FILE_W(str) {      \
       std::lock_guard<std::mutex> wl(g_logLock);      \
       g_wofs.open("G:\\log.txt", std::ios::app);      \
       g_wofs<<str<<std::endl;                                       \
       g_wofs.close();                                                      \
}
#else
#define LOG_TO_FILE(str)
#define LOG_TO_FILE_W(str)
#endif