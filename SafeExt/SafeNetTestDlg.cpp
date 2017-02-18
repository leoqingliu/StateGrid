// SafeNetDlg.cpp : Defines the entry point for the application.
//
#include "StdAfx.h"
//#include "SafeExtDlg.h"
//#include "../../Public/SingletonHolder.h"
#include "SafeProcess.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	// Main Dialog
	//CSafeExtDlg *pSafeNetDlg = Utils::Singleton<CSafeExtDlg>::Instance();
	//pSafeNetDlg->Create((HINSTANCE)hInstance);
	
	CSafeProcess *pProcess = new CSafeProcess;
	pProcess->Do(hInstance);

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	WaitForSingleObject(hEvent, INFINITE);

 	return TRUE;
}
