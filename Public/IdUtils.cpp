// WMIUtils.cpp: implementation of the CWMIUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "IdUtils.h"
#include <WinIoCtl.h>
#include "Debug.h"
#include "crypt.h"
#include "Utils.h"

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIdUtils::CIdUtils()
:m_pSvc(NULL)
{

}

CIdUtils::~CIdUtils()
{

}

HRESULT CIdUtils::Init()
{
	HRESULT lRet;
	
    // Step 1: --------------------------------------------------
    // Initialize COM. ------------------------------------------
	
	lRet = CoInitializeEx(0, COINIT_APARTMENTTHREADED); 
	if (FAILED(lRet))
	{
	//	OutputLog(_T("Failed to initialize COM library. Error code = 0x%08X"), lRet);
		return lRet;
	}

	// Step 2: --------------------------------------------------
    // Set general COM security levels --------------------------
    // Note: If you are using Windows 2000, you need to specify -
    // the default authentication credentials for a user by using
    // a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
    // parameter of CoInitializeSecurity ------------------------
	
    lRet = CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
    if (FAILED(lRet) && RPC_E_TOO_LATE != lRet)
    {
    //    OutputLog(_T("Failed to initialize security. Error code = 0x%08X"), lRet);
        CoUninitialize();
        return lRet;
    }

	// Step 3: ---------------------------------------------------
    // Obtain the initial locator to WMI -------------------------
	
    IWbemLocator *pLoc = NULL;
    lRet = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
    if (FAILED(lRet))
    {
    //    OutputLog(_T("Failed to create IWbemLocator object. Err code = 0x%08X"), lRet);
        CoUninitialize();
        return lRet;
    }

	// Step 4: -----------------------------------------------------
    // Connect to WMI through the IWbemLocator::ConnectServer method
	
    IWbemServices *pSvc = NULL;
	
    // Connect to the root/cimv2 namespace with
    // the current user and obtain pointer pSvc
    // to make IWbemServices calls.

	WCHAR pszNamespace[] = L"\\\\.\\root\\cimv2";
    lRet = pLoc->ConnectServer(
		(BSTR)pszNamespace,      // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);
    if (FAILED(lRet))
    {
    //    OutputLog(_T("Could not connect. Error code = 0x%08X"), lRet);
        pLoc->Release();
        CoUninitialize();
        return lRet;                // Program has failed.
    }
	
	// Step 5: --------------------------------------------------
    // Set security levels on the proxy -------------------------
	
    lRet = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);
	
    if (FAILED(lRet))
    {
    //    OutputLog(_T("Could not set proxy blanket. Error code = 0x%08X"), lRet);
        pSvc->Release();
        pLoc->Release();     
        CoUninitialize();
        return lRet;
    }

	pLoc->Release();

	m_pSvc = pSvc;

	return TRUE;	
}

VOID CIdUtils::UnInit()
{
	if (m_pSvc)
	{
		m_pSvc->Release();
		m_pSvc = NULL;
	}
	CoUninitialize();
}

BOOL CIdUtils::GetDiskId(CString& strId)
{
	LRESULT lInit = Init();
	if (FAILED(lInit) || NULL == m_pSvc)
	{
		return FALSE;
	}

	// Get Device Drive
	TCHAR szWin[MAX_PATH];
	GetWindowsDirectory(szWin, MAX_PATH);
	*_tcschr(szWin, _T('\\')) = 0;
	TCHAR szDrive[MAX_PATH];
	wsprintf(szDrive, _T("\\\\.\\%s"), szWin);
	
	HANDLE hDev = CreateFile(szDrive,
		0,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (INVALID_HANDLE_VALUE == hDev)
	{
	//	OutputLog(_T("Open Windows Drive Failed: %d"), GetLastError());
		UnInit();
		return FALSE;
	}
	
	STORAGE_DEVICE_NUMBER sdn;
	DWORD dwBytesRetuned=0;
	BOOL bGetDevice = DeviceIoControl(hDev,
		IOCTL_STORAGE_GET_DEVICE_NUMBER,
		NULL,
		0,
		&sdn,sizeof(sdn),
		&dwBytesRetuned,
		NULL);
	CloseHandle(hDev);
	if (!bGetDevice)
	{
	//	OutputLog(_T("Get Device Number Failed: %d"), GetLastError());
		UnInit();
		return FALSE;
	}
	int iPhysicaldrivenum = (int)sdn.DeviceNumber;
	TCHAR szDevice[128];
	wsprintf(szDevice, _T("\\\\.\\PHYSICALDRIVE%d"), iPhysicaldrivenum);

	// Get Serial Number
//	BSTR className = L"Win32_PhysicalMedia";
	BSTR className = L"Win32_DiskDrive";
	HRESULT lRet;
	IEnumWbemClassObject* pEnumAdapters = NULL;
	
	// Get the list of logical storage device instances.
	lRet = m_pSvc->CreateInstanceEnum(
		className,			// name of class
		0,
		NULL,
		&pEnumAdapters);	// pointer to enumerator
	if (FAILED(lRet))
	{
	//	OutputLog(_T("CreateInstanceEnum Failed, Error: 0x%08X"), lRet);
		UnInit();
		return FALSE;
	}

	BOOL bRet = FALSE;
	ULONG uReturned = 1;
	while(uReturned == 1)
	{
		IWbemClassObject *pStorageDev = NULL;

		//---------------------------
		// enumerate through the result-set.
		lRet = pEnumAdapters->Next(
			WBEM_INFINITE,	// timeout in two seconds
			1,				// return just one storage device
			&pStorageDev,	// pointer to storage device
			&uReturned);	// number obtained: one or zero
		
		if (SUCCEEDED(lRet) && 1 == uReturned)
		{
			VARIANT pTagVal;
//			BSTR propTagName = L"Tag";
			BSTR propTagName = L"DeviceID";
			lRet = pStorageDev->Get(
				propTagName,		// property name 
				0L, 
				&pTagVal,		// output to this variant 
				NULL, 
				NULL);
			if (SUCCEEDED(lRet))
			{
				VARTYPE vTagType = V_VT(&pTagVal);
				if (vTagType == VT_BSTR)
				{
					USES_CONVERSION;
					LPTSTR pTag = W2T(pTagVal.bstrVal);
					if (0 == _tcsicmp(szDevice, pTag))
					{
						VARIANT pSerialVal;
						BSTR propSerialName = L"SerialNumber";
//						BSTR propSerialName = L"Signature";
						lRet = pStorageDev->Get(
							propSerialName,		// property name 
							0L, 
							&pSerialVal,		// output to this variant 
							NULL, 
							NULL);
						if (SUCCEEDED(lRet))
						{
							VARTYPE vSerialType = V_VT(&pSerialVal);
#if 0
							if (vSerialType == VT_I4)
							{
								TCHAR szTmp[256];
								wsprintf(szTmp, _T("%d"), pSerialVal.lVal);
								strId = szTmp;
								bRet = TRUE;
							}
#else
							if (vSerialType == VT_BSTR)
							{
								LPTSTR pId = W2T(pSerialVal.bstrVal);
								strId = pId;
								bRet = TRUE;
							}
#endif
						}
						VariantClear(&pSerialVal);
					}
				}
			}
			VariantClear(&pTagVal);
			
			// Done with this object.
			pStorageDev->Release();
		}
	}

	if(pEnumAdapters)
	{
		pEnumAdapters->Release();
		pEnumAdapters = NULL;
	}

	UnInit();
	return bRet;
}

BOOL CIdUtils::GetBiosId(CString& strId)
{
	LRESULT lInit = Init();
	if (FAILED(lInit) || NULL == m_pSvc)
	{
		return FALSE;
	}

	// Get Serial Number
	BSTR className = L"Win32_BIOS";
	HRESULT lRet;
	IEnumWbemClassObject* pEnumAdapters = NULL;

	// Get the list of logical storage device instances.
	lRet = m_pSvc->CreateInstanceEnum(
		className,			// name of class
		0,
		NULL,
		&pEnumAdapters);	// pointer to enumerator
	if (FAILED(lRet))
	{
	//	OutputLog(_T("CreateInstanceEnum Failed, Error: 0x%08X"), lRet);
		UnInit();
		return FALSE;
	}

	BOOL bRet = FALSE;
	ULONG uReturned = 1;
	while(uReturned == 1)
	{
		IWbemClassObject *pStorageDev = NULL;

		//---------------------------
		// enumerate through the result-set.
		lRet = pEnumAdapters->Next(
			WBEM_INFINITE,	// timeout in two seconds
			1,				// return just one storage device
			&pStorageDev,	// pointer to storage device
			&uReturned);	// number obtained: one or zero

		if (SUCCEEDED(lRet) && 1 == uReturned)
		{
			LPWSTR pTagName[] = 
			{
				L"Manufacturer",
				L"SerialNumber"
			};
			for (int i = 0; i < _countof(pTagName); i++)
			{
				VARIANT pTagVal;
				BSTR propTagName = pTagName[i];
				lRet = pStorageDev->Get(
					propTagName,		// property name 
					0L, 
					&pTagVal,		// output to this variant 
					NULL, 
					NULL);
				if (SUCCEEDED(lRet))
				{
					VARTYPE vTagType = V_VT(&pTagVal);
					if (vTagType == VT_BSTR)
					{
						USES_CONVERSION;
						LPTSTR pTag = W2T(pTagVal.bstrVal);
						strId += pTag;
						strId += _T("___");
						bRet = TRUE;
					}
				}
			}

			// Done with this object.
			pStorageDev->Release();
		}
	}

	if(pEnumAdapters)
	{
		pEnumAdapters->Release();
		pEnumAdapters = NULL;
	}

	UnInit();
	return bRet;
}

BOOL CIdUtils::GetId(CString& strId)
{
	BOOL bRet = FALSE;
	CString strDiskId;
	if (GetDiskId(strDiskId))
	{
		CString strBiosId;
		if (GetBiosId(strBiosId))
		{
			strId = strDiskId + _T("___") + strBiosId;
			bRet = TRUE;
		}
	}
	
	return bRet;
}

VOID CIdUtils::LocalEnc(LPSTR szPassword, int iSize, CString& strIdOut)
{
	Crypt::ContentEnc(szPassword, iSize, strIdOut, "O0xpHhCeFcA6dSjB24DbAqkGMDKvLo4DSfmQnR37sFuTTiTv3ZVhEKkKNVdjyOpP");
}

VOID CIdUtils::LocalDec(const CString& strId, LPSTR& szPasswordA, SIZE_T& iSize)
{
	Crypt::ContentDec(strId, "O0xpHhCeFcA6dSjB24DbAqkGMDKvLo4DSfmQnR37sFuTTiTv3ZVhEKkKNVdjyOpP", szPasswordA, iSize);
}

VOID CIdUtils::RemoteEnc(LPSTR szPassword, int iSize, CString& strIdOut)
{
	Crypt::ContentEnc(szPassword, iSize, strIdOut, "JSe3uVFm8jF4e8f2UwdY6HXrgfi1kUYH1Omentbf3ad2KrBKflLOa3TxImpn544U");
}

VOID CIdUtils::RemoteDec(const CString& strId, LPSTR& szPasswordA, SIZE_T& iSize)
{
	Crypt::ContentDec(strId, "JSe3uVFm8jF4e8f2UwdY6HXrgfi1kUYH1Omentbf3ad2KrBKflLOa3TxImpn544U", szPasswordA, iSize);
}
