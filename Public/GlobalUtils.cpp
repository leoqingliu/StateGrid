#include "StdAfx.h"
#include "GlobalUtils.h"
#include <afxinet.h>
#include <string>
using namespace std;

map<tstring, HICON> CGlobalUtility::m_MapIcon;

CGlobalUtility::CGlobalUtility(void)
{
}

CGlobalUtility::~CGlobalUtility(void)
{
}

HICON CGlobalUtility::GetIconFromFileExt( LPCTSTR lpszPathFilename, BOOL bDirectory )
{
	/*
	static BOOL bFirst = TRUE;
	if(bFirst == TRUE)
	{
	bFirst = FALSE;
	gImageList.Attach(GetShellImageList(TRUE));
	}
	SHFILEINFO sfi;
	SHGetFileInfo(lpszPathFilename, 0, &sfi, sizeof(SHFILEINFO), 
	SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES);
	return gImageList.ExtractIcon(sfi.iIcon);
	*/

	//
	TCHAR szFilePath[256] = {0};
	lstrcpy(szFilePath, lpszPathFilename);
	CharUpper(szFilePath);

	//
	// > - Directory
	// < - Have not
	//
	if(bDirectory)
	{
		HICON hIcon = m_MapIcon[_T(">")];
		if(hIcon == NULL)
		{
			SHFILEINFO  sfi;
			DWORD	dwType = SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX;
			::SHGetFileInfo(_T("C:\\Program Files"), 0, &sfi, sizeof(SHFILEINFO), dwType);
			m_MapIcon[_T(">")] = hIcon = sfi.hIcon;
		}
		return hIcon;
	}
	else
	{
		LPTSTR pPoint = 
			StrRChr(szFilePath, szFilePath + lstrlen(szFilePath), _T('.'));
		if(pPoint == NULL)
		{
			HICON hIcon = m_MapIcon[_T("<")];
			if(hIcon == NULL)
			{
				SHFILEINFO sfi;
				DWORD dwType = SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES;
				::SHGetFileInfo(szFilePath, 0, &sfi, sizeof(SHFILEINFO), dwType);
				m_MapIcon[_T("<")] = hIcon = sfi.hIcon;
			}
			return hIcon;
		}
		else
		{
			TCHAR szBuffer[MAX_PATH] = {0};
			lstrcpyn(szBuffer, (LPCTSTR)pPoint + 1, (int)(szFilePath + lstrlen(szFilePath) - pPoint));
			HICON hIcon = m_MapIcon[szBuffer];
			if(hIcon == NULL)
			{
				SHFILEINFO sfi;
				DWORD dwType = SHGFI_ICON|SHGFI_LARGEICON|SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES;
				::SHGetFileInfo(szFilePath, 0, &sfi, sizeof(SHFILEINFO), dwType);
				m_MapIcon[szBuffer] = hIcon = sfi.hIcon;
			}
			return hIcon;
		}
	}
}

// 2 CHARS
BYTE CGlobalUtility::MakeByte(CString str)
{
	BYTE bRet = 0;
	for(int i = 0; i < 2; i++)
	{
		TCHAR cTemp = str.GetAt(i);
		if(cTemp >= _T('0') && cTemp <= _T('9'))
		{
			bRet = bRet * 16 + cTemp - _T('0');
		}
		else 
		{
			bRet = bRet * 16 + cTemp - _T('A') + 10;
		}
	}
	return bRet;
}

// 4 CHARS
WORD CGlobalUtility::MakeWord(CString str)
{
	return MAKEWORD(MakeByte(str.Mid(2, 2)), MakeByte(str.Mid(0, 2)));
}

// 8 CHARS
DWORD CGlobalUtility::MakeDWord(CString str)
{
	return MAKELONG(MakeWord(str.Mid(4, 4)), MakeWord(str.Mid(0, 4)));
}

#pragma warning(push)
#pragma warning(disable:4996)
LPTSTR CGlobalUtility::FileSizeToString(DWORD dwFileSize, LPTSTR lpSize)
{
	DWORD dwIndex = 0;
	DWORD dwSize = dwFileSize;
	DWORD dwLasted = 0;
	while(dwSize / 1024)
	{
		dwIndex++;
		dwLasted = dwSize % 1024;
		dwSize /= 1024;
	}

	TCHAR szEndString[MAX_PATH] = {0};
	switch(dwIndex)
	{
	case 0:
		lstrcpy(szEndString, _T("B"));
		break;
	case 1:
		lstrcpy(szEndString, _T("KB"));
		break;
	case 2:
		lstrcpy(szEndString, _T("MB"));
		break;
	case 3:
		lstrcpy(szEndString, _T("GB"));
		break;
	default:
		break;
	}

	if (0 == dwIndex)
	{
		_stprintf(lpSize, _T("%d %s"), dwFileSize, szEndString);
	}
	else
	{
		double dExtra = (double)dwLasted / 1024;
		double dwInputFileSize = dwSize + dExtra;
		_stprintf(lpSize, _T("%.2f %s"), dwInputFileSize, szEndString);
	}
	return lpSize;
}
#pragma warning(pop)

PTSTR CGlobalUtility::BigNumToString(LONGLONG lNum, PTSTR szBuf) 
{
	TCHAR szNum[100];
	wsprintf(szNum, TEXT("%d"), lNum);
	NUMBERFMT nf;
	nf.NumDigits		= 0;
	nf.LeadingZero		= FALSE;
	nf.Grouping			= 3;
	nf.lpDecimalSep		= TEXT(".");
	nf.lpThousandSep	= TEXT(",");
	nf.NegativeOrder	= 0;
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNum, &nf, szBuf, 100);
	return(szBuf);
}

/*
#define  KLoginUrlFormatter _T("https://login.%s.com/rst.srf")
#define  KSoapEnvelopeFormatter \
"<Envelope xmlns=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:wsse=\"http://schemas.xmlsoap.org/ws/2003/06/secext\" xmlns:saml=\"urn:oasis:names:tc:SAML:1.0:assertion\" xmlns:wsp=\"http://schemas.xmlsoap.org/ws/2002/12/policy\" xmlns:wsu=\"http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd\" xmlns:wsa=\"http://schemas.xmlsoap.org/ws/2004/03/addressing\" xmlns:wssc=\"http://schemas.xmlsoap.org/ws/2004/04/sc\" xmlns:wst=\"http://schemas.xmlsoap.org/ws/2004/04/trust\">\
	<Header>\
		<wsse:Security>\
			<wsse:UsernameToken Id=\"user\">\
				<wsse:Username>%s</wsse:Username>\
				<wsse:Password>%s</wsse:Password>\
			</wsse:UsernameToken>\
		</wsse:Security>\
	</Header>\
	<Body>\
		<ps:RequestMultipleSecurityTokens xmlns:ps=\"http://schemas.microsoft.com/Passport/SoapServices/PPCRL\" Id=\"RSTS\">\
			<wst:RequestSecurityToken Id=\"RST0\">\
				<wst:RequestType>http://schemas.xmlsoap.org/ws/2004/04/security/trust/Issue</wst:RequestType>\
				<wsp:AppliesTo>\
					<wsa:EndpointReference>\
						<wsa:Address>http://passport.net/tb</wsa:Address>\
					</wsa:EndpointReference>\
				</wsp:AppliesTo>\
			</wst:RequestSecurityToken>\
			<wst:RequestSecurityToken Id=\"RST0\">\
				<wst:RequestType>http://schemas.xmlsoap.org/ws/2004/04/security/trust/Issue</wst:RequestType>\
					<wsp:AppliesTo>\
						<wsa:EndpointReference>\
							<wsa:Address>cumulus.services.%s.com</wsa:Address>\
						</wsa:EndpointReference>\
					</wsp:AppliesTo>\
				<wsse:PolicyReference URI=\"MBI\"/>\
			</wst:RequestSecurityToken>\
		</ps:RequestMultipleSecurityTokens>\
	</Body>\
</Envelope>"

LONG CGlobalUtility::CheckMSNPassword(LPCSTR lpUser, LPCSTR lpPassword)
{
	LPSTR lpPostData = new CHAR[lstrlenA(KSoapEnvelopeFormatter) + 128];
	sprintf_s(lpPostData, lstrlenA(KSoapEnvelopeFormatter) + 128, 
		KSoapEnvelopeFormatter, lpUser, lpPassword, "live");

	LPTSTR lpServer = new TCHAR[lstrlen(KLoginUrlFormatter) + 128];
	wsprintf(lpServer, KLoginUrlFormatter, _T("live"));

	LONG lRet = 0;
	CInternetSession InternetSession;
	CHttpFile* pHttpFile = NULL; 
	try   
	{ 
		// 		CInternetSession* pSession = NULL; 
		// 		CHttpConnection* pConnection = NULL; 
		// 		pSession = new CInternetSession(NULL, 1, INTERNET_OPEN_TYPE_PRECONFIG); 
		// 		pConnection = pSession->GetHttpConnection(_T("https://login.live.com"), (INTERNET_PORT)80, NULL, NULL); 
		// 		DWORD dwFlag= INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE; 
		// 		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("rst.srf"), NULL, 1, NULL, NULL, dwFlag); 
		// 		if (pHttpFile != NULL)   
		// 		{ 
		// 			pHttpFile->Close(); 
		// 			delete pHttpFile;   
		// 		}   
		// 		if (pConnection != NULL)   
		// 		{ 
		// 			pConnection->Close(); 
		// 			delete pConnection;   
		// 		}   
		// 		if (pSession != NULL)   
		// 		{ 
		// 			pSession->Close(); 
		// 			delete pSession;   
		// 		} 

		pHttpFile = (CHttpFile*)InternetSession.OpenURL(lpServer);
		pHttpFile->AddRequestHeaders(_T("Content-Type: application/soap+xml; charset=utf-8"), HTTP_ADDREQ_FLAG_ADD_IF_NEW, -1L); 
		pHttpFile->SendRequest(NULL,0, (LPVOID)lpPostData, lstrlenA(lpPostData)); 
		if(pHttpFile)   
		{ 
			DWORD dwHttpStatus;
			if (pHttpFile->QueryInfoStatusCode(dwHttpStatus) != 0) 
			{
				if (dwHttpStatus < 400) 
				{ 
					int nRead = 0; 
					LPSTR pBuffer = new char[10240]; 
					string csResponse;
					do   
					{ 
						nRead = pHttpFile->Read(pBuffer, 10240); 
						if (nRead != 0)   
						{ 
							pBuffer[nRead] = 0; 
							csResponse += pBuffer; 
						}   

					} 
					while (nRead != 0); 
					if(pBuffer) 
					{ 
						delete[] pBuffer; 
						pBuffer = NULL; 
					}

					// Find
					if(csResponse.find("BinarySecurityToken") != string::npos)
					{
						lRet = TRUE;
					}
					else
					{
						lRet = 2;
					}
				}   
			}
		}
		delete pHttpFile;
		pHttpFile = NULL;
	}
	catch (CInternetException* e)   
	{ 
//			m_pPopopDlg->OutputHistroy(getInetError(e->m_dwError));
		e->Delete(); 
	}   
	catch(...)   
	{ 
	}   

	InternetSession.Close();
	delete[] lpPostData;
	lpPostData = NULL;
	delete[] lpServer;
	lpServer = NULL;
	return lRet;
}
*/

BOOL CGlobalUtility::Extractfile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpPath)
{
	// Find and Load the resource containing file(s) data
	HRSRC hResLoad = FindResource(hInstance, lpType, lpName);
	HGLOBAL hResData = LoadResource(hInstance, hResLoad);
	LPCSTR data = (LPCSTR)LockResource(hResData);

	// We should have properly loaded the resource
	assert(data != NULL && hResData != NULL);

	DWORD dwFileLen = SizeofResource(hInstance, hResLoad);
	HANDLE hFileHandle = CreateFile(lpPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	if(hFileHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	SetFilePointer(hFileHandle, 0, 0, FILE_BEGIN);   
	DWORD dwBytesWrite;
	if(!WriteFile(hFileHandle, data, dwFileLen, &dwBytesWrite, 0))
	{
		CloseHandle(hFileHandle);
		return FALSE;
	}
	CloseHandle(hFileHandle);

	// Set Attributes
	//SetFileAttributes(lpPath, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN);

	// Perform cleanup
	//UnlockResource(hResData);
	FreeResource(hResData);

	return TRUE;
}


BOOL CGlobalUtility::CreateDir(LPCTSTR pDir)
{
	TCHAR szDir[MAX_PATH];
	lstrcpy(szDir, pDir);
	if (szDir[lstrlen(szDir) - 1])
	{
		szDir[lstrlen(szDir) - 1] = 0;
	}

	LPCTSTR szDelimiters = _T("\\");
	LPTSTR pContext = NULL;
	LPTSTR pTerm = NULL;
	TCHAR szPath[MAX_PATH] = {0};
	if (memcmp(pDir, _T("\\\\.\\"), sizeof(TCHAR) * 4) == 0)
	{
		lstrcpy(szPath, _T("\\\\.\\"));
		pTerm = _tcstok_s(szDir + 4, szDelimiters, &pContext);
	}
	else
	{
		pTerm = _tcstok_s(szDir, szDelimiters, &pContext);
	}
	INT iIndex = 0;
	while (NULL != pTerm)
	{
		lstrcat(szPath, pTerm);
		lstrcat(szPath, _T("\\"));
		iIndex++;
		if (iIndex <= 1)
		{
			pTerm = _tcstok_s(NULL, szDelimiters, &pContext);
			continue;
		}
		BOOL bCreateDirRet = TRUE;
		if (CreateDirectory(szPath, NULL) == FALSE)
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_FILE_EXISTS && dwError != ERROR_ALREADY_EXISTS)
			{
				bCreateDirRet = FALSE;
			}
		}
		if (FALSE == bCreateDirRet)
		{
			return FALSE;
		}
		pTerm = _tcstok_s(NULL, szDelimiters, &pContext);
	}

	return TRUE;
}