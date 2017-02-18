// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__C76A67CC_4AC7_4C23_AAD3_C4FE814C6164__INCLUDED_)
#define AFX_STDAFX_H__C76A67CC_4AC7_4C23_AAD3_C4FE814C6164__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define STRICT
#include "targetver.h"


#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <afxwin.h>				// MFC core and standard components
#include <afxext.h>				// MFC extensions
//#include <afxmt.h>
//#include <afxdisp.h>			// MFC Automation classes

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>				// MFC OLE
#include <afxodlgs.h>			// MFC OLE Dialog
#include <afxdisp.h>			// MFC Auto
#endif // _AFX_NO_OLE_SUPPORT

#include <afxdtctl.h>			// MFC Support Internet Explorer 4 Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>				// MFC Support Windows Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

/*
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WINSOCK2.H>
*/

#pragma comment(lib, "ShlWapi.lib")
#include "../Defines.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_STDAFX_H__C76A67CC_4AC7_4C23_AAD3_C4FE814C6164__INCLUDED)
