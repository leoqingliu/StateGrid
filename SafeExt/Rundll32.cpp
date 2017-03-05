#include "StdAfx.h"
#include "../Public/SvchostServiceModule.h"
#include "resource.h"
#include <atlstr.h>
#include "../ConnConf.h"
#include "../Public/IPC.h"
#include "../Public/Utils.h"

extern HINSTANCE ghInstance;

//
//	used to install by rundll32.exe
//	Platform SDK: Tools - Rundll32
//	The Run DLL utility (Rundll32.exe) included in Windows enables you to call functions exported from a 32-bit DLL. 
//	These functions must have the following syntax:
//
//	rundll32.exe SafeExt.dll,RunDll_Install
//
#define RUNDLL_FUNCTION(_x)\
	void CALLBACK _x(\
	HWND		hWnd,		/* handle to owner window */		\
	HINSTANCE	hInst,		/* instance handle for the DLL */	\
	LPSTR		szParam,	/* string the DLL will parse */		\
	int			nCmdShow	/* show state */					\
	)

STDAPI_(BOOL) InstallService()
{
	TCHAR szServiceName[MAX_PATH];
	lstrcpy(szServiceName, SERVICE_DLL_NAME);
	TCHAR szDescription[MAX_PATH];
	lstrcpy(szDescription, SERVICE_DLL_DESC);
	TCHAR szDllPath[MAX_PATH] = L"";
	CSvchostServiceModule::GetDllPath(szDllPath, SERVICE_DLL_FILE_NAME, GetCurrentProcessId());
	AfxMessageBox(szDllPath);
	BOOL bRet = CSvchostServiceModule::Install(szServiceName, szDescription, szDllPath, _T("MacMan"));
	if (bRet)
	{
		OutputLog(_T("Install Service Success"));
		bRet = CSvchostServiceModule::Control(szServiceName);
		if (bRet)
		{
			OutputLog(_T("Start Service Success"));
		}
		else
		{
			OutputLog(_T("Start Service Failed!"));
		}
	}
	else
	{
		OutputLog(_T("Install Service Failed!"));
	}
	return bRet;
}

STDAPI_(BOOL) UnInstallService()
{
	TCHAR szServiceName[MAX_PATH];
	lstrcpy(szServiceName, SERVICE_DLL_NAME);
	BOOL bRet = CSvchostServiceModule::Control(szServiceName, FALSE);
	if (bRet)
	{
		OutputLog(_T("Stop Service Success"));
	}
	else
	{
		OutputLog(_T("Stop Service Failed!"));
	}

	bRet = CSvchostServiceModule::Uninstall(szServiceName, _T("MacMan"));
	if (bRet)
	{
		OutputLog(_T("Uninstall Service Success"));
	}
	else
	{
		OutputLog(_T("Uninstall Service Failed!"));
	}

	return bRet;
}

RUNDLL_FUNCTION(RunDll_Install)
{
#if 0
	BOOL bRet = InstallService();
	if (bRet)
	{
		MessageBox(hWnd, _T("Install Success"), _T(""), MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(hWnd, _T("Install Fail"), _T(""), MB_ICONERROR);
	}
#else
	InstallService();
#endif
}

RUNDLL_FUNCTION(RunDll_UnInstall)
{
#if 0
	BOOL bRet = UnInstallService();
	if (bRet)
	{
		MessageBox(hWnd, _T("UnInstall Success"), _T(""), MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(hWnd, _T("UnInstall Fail"), _T(""), MB_ICONERROR);
	}
#else
	UnInstallService();
#endif
}

BOOL SaveCapturedBitmap(HDC hCaptureDC, HBITMAP hCaptureBitmap, LPCTSTR lpFilePath)
{
#ifndef CAPTUREBLT
#define CAPTUREBLT (DWORD)0x40000000
#endif

	if (NULL == hCaptureDC || NULL == hCaptureBitmap || NULL == lpFilePath)
	{
		return FALSE;
	}

	DWORD dwCreateFileAccess	= GENERIC_READ | GENERIC_WRITE;//FILE_ALL_ACCESS;
	DWORD dwCreateFileShare		= FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwCreatePos			= CREATE_ALWAYS;

	//Create the File handle
	HANDLE hFile = CreateFile(
		lpFilePath, dwCreateFileAccess, dwCreateFileShare, 
		0, dwCreatePos, FILE_ATTRIBUTE_NORMAL, 0);		
	if (hFile == INVALID_HANDLE_VALUE)
	{	
		return FALSE;
	}

	BITMAP bmp;
	::GetObject(hCaptureBitmap, sizeof(BITMAP), &bmp);

	BITMAPINFOHEADER bih = {0};
	bih.biBitCount = bmp.bmBitsPixel;
	bih.biCompression = BI_RGB;
	bih.biHeight = bmp.bmHeight;
	bih.biPlanes = 1;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biSizeImage = bmp.bmWidthBytes * bmp.bmHeight;
	bih.biWidth = bmp.bmWidth;

	BITMAPFILEHEADER bfh = {0};
	bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bfh.bfSize = bfh.bfOffBits + bmp.bmWidthBytes * bmp.bmHeight;
	bfh.bfType = (WORD)0x4d42;

	DWORD dwWriten = 0;
	WriteFile(hFile, &bfh, sizeof(BITMAPFILEHEADER), &dwWriten, NULL);
	WriteFile(hFile, &bih, sizeof(BITMAPINFOHEADER), &dwWriten, NULL);

	LPBYTE pBuf = new byte[bmp.bmWidthBytes * bmp.bmHeight];
	GetDIBits(
		hCaptureDC,
		(HBITMAP)hCaptureBitmap,
		0,
		bmp.bmHeight,
		pBuf,
		(LPBITMAPINFO) &bih,
		DIB_RGB_COLORS);
	WriteFile(hFile, pBuf, bmp.bmWidthBytes * bmp.bmHeight, &dwWriten, NULL);
	delete [] pBuf;
	CloseHandle(hFile);
	return TRUE;
}

RUNDLL_FUNCTION(CaptureScreen)
{
	USES_CONVERSION;
	//MessageBox(NULL, A2T(szParam), _T("1"), MB_ICONERROR);
	TCHAR lpFilePath[MAX_PATH];
	//lstrcpy(lpFilePath, _T("C:\\Users\\xltyt\\Desktop\\Screen\\Debug\\2.bmp"));
	lstrcpy(lpFilePath, A2T(szParam));
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
	SelectObject(hCaptureDC,hCaptureBitmap); 
	BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight, hDesktopDC,0,0,SRCCOPY|CAPTUREBLT); 
	BOOL bRet = SaveCapturedBitmap(hCaptureDC, hCaptureBitmap, lpFilePath);
	ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteDC(hCaptureDC);
	DeleteObject(hCaptureBitmap);
}

BOOL RealCmd(LPTSTR strNewCmd)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb			= sizeof(si);
	si.wShowWindow	= SW_SHOW;
	si.dwFlags		= STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;

	LPCTSTR lpApplicationName = NULL;
	LPTSTR lpCommandLine = NULL;
	/*
	if (bExpanded)
	{
		lpApplicationName = NULL;
		lpCommandLine = (LPTSTR)(LPCTSTR)strNewCmd;
	}
	else
	{
		lpApplicationName = (LPTSTR)(LPCTSTR)strNewCmd;
		lpCommandLine = (LPTSTR)(LPCTSTR)strCmd;
	}
	*/
	lpApplicationName = NULL;
	lpCommandLine = strNewCmd;
	BOOL bRet = ::CreateProcess(
		lpApplicationName,
		lpCommandLine,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE /*| CREATE_SUSPENDED*/,
		NULL,
		NULL,
		&si,
		&pi);
	if (!bRet)
	{
		//MessageBox(NULL, _T("启动程序失败!"), _T(""), MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

RUNDLL_FUNCTION(ExtractLockScreen)
{
	//BOOL bLock = LockWorkStation();
	//bLock = TRUE;
	USES_CONVERSION;
	TCHAR lpFilePath[MAX_PATH];
	lstrcpy(lpFilePath, A2T(szParam));
	/*
#ifdef _WIN64
	INT iResId = IDR_BIN_LOCKER_X64;
#else
	INT iResId = IDR_BIN_LOCKER;
#endif
	*/
	BOOL bWow64 = FALSE;
	Utils::CheckWow64(bWow64);
	INT iResId = (bWow64 ? IDR_BIN_LOCKER_X64 : IDR_BIN_LOCKER);
	BOOL bRet = Utils::Extractfile(
		ghInstance,
		MAKEINTRESOURCE(iResId), _T("BIN"), lpFilePath
		);
	if (!bRet)
	{
		//OutputLog(_T("释放文件失败(Error: %d)，请检查磁盘空间和杀毒软件相关配置!"), GetLastError());
		return;
	}

	OutputLog(_T("Cmd: %s"), lpFilePath);
	RealCmd(lpFilePath);
}

#if 0
RUNDLL_FUNCTION(LockScreen)
{
	DWORD dwType = 1;
	HWND hTargetWnd = ::FindWindow(_T("{46D29001-2748-4859-B945-ADB6531FDD58}"), NULL);
	COPYDATASTRUCT copyData = {0};
	copyData.lpData = &dwType;
	copyData.cbData = sizeof(DWORD);
	::SendMessage(hTargetWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&copyData);
	OutputLog(_T("Lock Wnd: %x"), hTargetWnd);
}

RUNDLL_FUNCTION(UnLockScreen)
{
	/*
	HWND hWndFind = ::FindWindow(_T("{46D29001-2748-4859-B945-ADB6531FDD58}"), NULL);
	if (hWndFind)
	{
		::SendMessage(hWndFind, WM_CLOSE, NULL, NULL);
	}
	*/

	DWORD dwType = 2;
	HWND hTargetWnd = ::FindWindow(_T("{46D29001-2748-4859-B945-ADB6531FDD58}"), NULL);
	COPYDATASTRUCT copyData = {0};
	copyData.lpData = &dwType;
	copyData.cbData = sizeof(DWORD);
	::SendMessage(hTargetWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&copyData);
}
#endif

/*
RUNDLL_FUNCTION(UpdateInfo)
{
	IPC ipc;
	BOOL bOpen = ipc.Open();
	if (bOpen)
	{
		OutputLog(_T("UpdateInfo Open OK"));
		_CLIENT_LOCK_INFO info;
		DWORD dwSize = sizeof(_CLIENT_LOCK_INFO);
		BOOL bRead = ipc.Read((LPBYTE)&info, dwSize);
		if (bRead)
		{
			OutputLog(_T("UpdateInfo Read OK: %d"), info.iWinCount);

			HWND hTargetWnd = ::FindWindow(_T("{46D29001-2748-4859-B945-ADB6531FDD58}"), NULL);
			COPYDATASTRUCT copyData = {0};
			copyData.lpData = &info;
			copyData.cbData = sizeof(_CLIENT_LOCK_INFO);
			::SendMessage(hTargetWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&copyData);
			OutputLog(_T("UpdateInfo Wnd: %x"), hTargetWnd);
		}
		else
		{
			OutputLog(_T("UpdateInfo Read Filad"));
		}
	}
	else
	{
		OutputLog(_T("UpdateInfo Open Failed"));
	}
}
*/

/*
RUNDLL_FUNCTION(UpdateInfo)
{
}
*/