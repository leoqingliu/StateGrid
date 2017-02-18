// ServiceUpgrade.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ServiceUpgrade.h"
#include "../Defines.h"
#include "../Public/SvchostServiceModule.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	CSvchostServiceModule::Control(SERVICE_DLL_NAME, FALSE);
	BOOL bOpRet = FALSE;
	DWORD dwTime = 0;
	while (TRUE)
	{
		BOOL bRunning = CSvchostServiceModule::IsRunning(SERVICE_DLL_NAME);
		if (!bRunning)
		{
			bOpRet = TRUE;
			break;
		}

		dwTime++;
		Sleep(200);
		if (dwTime >= 50)
		{
			break;
		}
	}
	if (!bOpRet)
	{
		MessageBox(NULL, _T("πÿ±’MacServer ß∞‹!"), _T("MacMan"), MB_ICONERROR);
		return -1;
	}

	CSvchostServiceModule::Control(SERVICE_DLL_NAME, TRUE);
	return 0;
}
