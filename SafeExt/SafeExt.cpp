// SafeExt.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f SafeExtps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "SafeExt.h"
#include "SafeProcess.h"

void Dll_Init();

#include "SvchostServiceModule.h"
extern CSvchostServiceModule gSvchostServiceModule;
// Safe Process Instance
HINSTANCE ghInstance = NULL;
HANDLE ghEvent = NULL;

DWORD WINAPI _ServiceRoutine(CALLBACK_TYPE uType, LPVOID pParam, LPVOID lpParameter, LPVOID lpParameter1)
{
	// Start
	if (CALLBACK_TYPE_START == uType)
	{
		CSafeProcess *m_Process = new CSafeProcess;
		m_Process->Do(ghInstance);
		ghEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		WaitForSingleObject(ghEvent, INFINITE);
	}
	// Stop
	else if (CALLBACK_TYPE_STOP == uType)
	{
		SetEvent(ghEvent);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

#if 0
extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		OutputLog(_T("DllMain:DLL_PROCESS_ATTACH"));
        DisableThreadLibraryCalls(hInstance);
		
		mhInstance = hInstance;
		gSvchostServiceModule.Init(SERVICE_DLL_NAME, _ServiceRoutine, NULL);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
		OutputLog(_T("DllMain:DLL_PROCESS_DETACH"));
	}
    return TRUE;
}
#endif

#ifdef _AFX
	class CSafeNetApp : public CWinApp
	{
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSuperExtenderApp)
	public:
		virtual BOOL InitInstance();
		virtual int ExitInstance();
		//}}AFX_VIRTUAL

		//{{AFX_MSG(CSuperExtenderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

	BEGIN_MESSAGE_MAP(CSafeNetApp, CWinApp)
		//{{AFX_MSG_MAP(CSuperExtenderApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	CSafeNetApp theApp;
	BOOL CSafeNetApp::InitInstance()
	{
		HINSTANCE hInstance = m_hInstance;
		OutputLog(_T("DllMain(MFC): InitInstance"));
		DisableThreadLibraryCalls(hInstance);

		ghInstance = hInstance;
		gSvchostServiceModule.Init(SERVICE_DLL_NAME, _ServiceRoutine, NULL);

		return CWinApp::InitInstance();
	}

	int CSafeNetApp::ExitInstance()
	{
		OutputLog(_T("DllMain(MFC): ExitInstance"));
		return CWinApp::ExitInstance();
	}
#else
	extern "C"
	BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
	{
		if (dwReason == DLL_PROCESS_ATTACH)
		{
			OutputLog(_T("DllMain: DLL_PROCESS_ATTACH"));
			DisableThreadLibraryCalls(hInstance);

			ghInstance = hInstance;
			gSvchostServiceModule.Init(SERVICE_DLL_NAME, _ServiceRoutine, NULL);
		}
		else if (dwReason == DLL_PROCESS_DETACH)
		{
			OutputLog(_T("DllMain: DLL_PROCESS_DETACH"));
		}
		return TRUE;
	}
#endif