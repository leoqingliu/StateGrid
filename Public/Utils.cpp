#include "StdAfx.h"
#include "Utils.h"
#include <TlHelp32.h>
#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Version.lib")

// Decompress
extern "C"
{
	#include "7z/7z.h"
	#include "7z/7zAlloc.h"
	#include "7z/7zCrc.h"
	#include "7z/7zFile.h"
	#include "7z/LzmaLib.h"
};

#include <assert.h>

std::map<CString, Utils::FILE_INFO_MAP> Utils::m_FileInfoMap;

std::vector<CString> Utils::Split(CString str, LPCTSTR pDelim)
{
	std::vector<CString> vecArray;
	int curPos = 0;
	while(1)
	{
		CString resToken = str.Tokenize(pDelim, curPos);
		if(resToken.IsEmpty())
			break;
		vecArray.push_back(resToken);
	}

	return vecArray;
}

std::vector<CString> Utils::SplitFull(CString str, LPCTSTR pDelim)
{
	std::vector<CString> vecArray;
	int iCurPos = 0;
	while (TRUE)
	{
		int iFindPos = str.Find(pDelim, iCurPos);
		if (-1 == iFindPos)
		{
			CString strToken = str.Mid(iCurPos);
			vecArray.push_back(strToken);
			break;
		}

		CString resToken = str.Mid(iCurPos, iFindPos - iCurPos);
		vecArray.push_back(resToken);
		iCurPos = iFindPos + lstrlen(pDelim);
	}

	return vecArray;
}

BOOL Utils::Extractfile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpPath)
{
	// Find and Load the resource containing file(s) data
	HRSRC hResLoad = FindResource(hInstance, lpType, lpName);
	HGLOBAL hResData = LoadResource(hInstance, hResLoad);
	LPCSTR data = (LPCSTR)LockResource(hResData);

	// We should have properly loaded the resource
	assert(data != NULL && hResData != NULL);

	DWORD dwFileLen = SizeofResource(hInstance, hResLoad);
	HANDLE hFileHandle = CreateFile(lpPath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0);
	if (hFileHandle == INVALID_HANDLE_VALUE)
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

BOOL Utils::Decompress(LPCSTR pFile, LPCTSTR pDstDir)
{
	USES_CONVERSION;

	CFileInStream archiveStream;
	if (InFile_Open(&archiveStream.file, pFile))
	{
		return FALSE;
	}

	CLookToRead lookStream;
	FileInStream_CreateVTable(&archiveStream);
	LookToRead_CreateVTable(&lookStream, False);

	lookStream.realStream = &archiveStream.s;
	LookToRead_Init(&lookStream);

	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	allocImp.Alloc = SzAlloc;
	allocImp.Free = SzFree;
	allocTempImp.Alloc = SzAllocTemp;
	allocTempImp.Free = SzFreeTemp;

	UInt16 *temp = NULL;
	size_t tempSize = 0;

	CSzArEx db;
	CrcGenerateTable();
	SzArEx_Init(&db);
	SRes res = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp);
	if (res == SZ_OK)
	{
		UInt32 i;

		/*
		if you need cache, use these 3 variables.
		if you use external function, you can make these variable as static.
		*/
		UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
		Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
		size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */

		for (i = 0; i < db.db.NumFiles; i++)
		{
			size_t offset = 0;
			size_t outSizeProcessed = 0;
			const CSzFileItem *f = db.db.Files + i;
			size_t len = SzArEx_GetFileNameUtf16(&db, i, NULL);
			if (len > tempSize)
			{
				SzFree(NULL, temp);
				tempSize = len;
				temp = (UInt16 *)SzAlloc(NULL, tempSize * sizeof(temp[0]));
				if (temp == 0)
				{
					res = SZ_ERROR_MEM;
					break;
				}
			}

			SzArEx_GetFileNameUtf16(&db, i, temp);
			if (res != SZ_OK)
				break;
			if (f->IsDir)
			{
			}
			else
			{
				res = SzArEx_Extract(&db, &lookStream.s, i,
					&blockIndex, &outBuffer, &outBufferSize,
					&offset, &outSizeProcessed,
					&allocImp, &allocTempImp);
				if (res != SZ_OK)
					break;
			}

			CSzFile outFile;
			size_t processedSize;
			size_t j;
			UInt16 *name = (UInt16 *)temp;
			const UInt16 *destName = (const UInt16 *)name;
			for (j = 0; name[j] != 0; j++)
			{
				if (name[j] == '/')
				{
					name[j] = 0;
					CString strDirMid;
					strDirMid.Format(_T("%s\\%s"), pDstDir, (LPCTSTR)name);
					BOOL bCreated = CreateDirectory(strDirMid, NULL);
					name[j] = CHAR_PATH_SEPARATOR;
				}
			}
			TCHAR destPath[MAX_PATH];
			wsprintf(destPath, _T("%s\\%s"), pDstDir, destName);

			if (f->IsDir)
			{
				CreateDirectory((LPCTSTR)destPath, NULL);
				continue;
			}
			else if (OutFile_OpenW(&outFile, T2W(destPath)))
			{
				res = SZ_ERROR_FAIL;
				break;
			}
			processedSize = outSizeProcessed;
			if (File_Write(&outFile, outBuffer + offset, &processedSize) != 0 || processedSize != outSizeProcessed)
			{
				res = SZ_ERROR_FAIL;
				break;
			}
			if (File_Close(&outFile))
			{
				res = SZ_ERROR_FAIL;
				break;
			}
			if (f->AttribDefined)
				SetFileAttributes((LPCTSTR)destPath, f->Attrib);
		}
		IAlloc_Free(&allocImp, outBuffer);
	}

	SzArEx_Free(&db, &allocImp);
	SzFree(NULL, temp);

	File_Close(&archiveStream.file);
	if (res != SZ_OK)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL Utils::ExtractCompressedFile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR pPath, LPCTSTR pDir)
{
	USES_CONVERSION;
	BOOL bRet = Utils::Extractfile(
		hInstance,
		lpType, lpName, pPath
		);
	if (!bRet)
	{
		return FALSE;
	}
	BOOL bCreateDirRet = TRUE;
	if (CreateDirectory(pDir, NULL) == FALSE)
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
	bRet = Utils::Decompress(CT2A(pPath).m_psz, pDir);
	DeleteFile(pPath);
	if (!bRet)
	{
		return FALSE;
	}

	return TRUE;
}

VOID Utils::GetProcessNames(std::vector<ProcessInfo>& vecInfos)
{
	DWORD dwCount = 0;
	PWTS_PROCESS_INFO pi = {0};
	if (WTSEnumerateProcesses(NULL, 0, 1, &pi, &dwCount))
	{
		for (DWORD i = 0; i < dwCount; i++)
		{
			if (NULL == pi[i].pUserSid)
			{
				continue;
			}

			LPTSTR AcctName = NULL;
			LPTSTR DomainName = NULL;
			DWORD dwAcctName = 0;
			DWORD dwDomainName = 0;
			SID_NAME_USE eUse = SidTypeUser;
			LookupAccountSid(
				NULL,
				pi[i].pUserSid,
				AcctName,
				&dwAcctName,
				DomainName,
				&dwDomainName,
				&eUse);
			if (0 == dwAcctName || 0 == dwDomainName)
			{
				continue;
			}
			// Reallocate memory for the buffers.
			AcctName = (LPTSTR)GlobalAlloc(GMEM_FIXED, (dwAcctName + 1) * sizeof(TCHAR));
			if (AcctName == NULL)
			{
				continue;
			}
			ZeroMemory(AcctName, (dwAcctName + 1) * sizeof(TCHAR));
			DomainName = (LPTSTR)GlobalAlloc(GMEM_FIXED, (dwDomainName + 1) * sizeof(TCHAR));
			if (DomainName == NULL)
			{
				GlobalFree(DomainName);
				continue;
			}
			ZeroMemory(DomainName, (dwDomainName + 1) * sizeof(TCHAR));
			BOOL bRtnBool = LookupAccountSid(
				NULL,
				pi[i].pUserSid,     // security identifier
				AcctName,           // account name buffer
				&dwAcctName,		// size of account name buffer 
				DomainName,         // domain name
				&dwDomainName,		// size of domain name buffer
				&eUse
				);
			if (!bRtnBool)
			{
				GlobalFree(AcctName);
				GlobalFree(DomainName);
				continue;
			}

			ProcessInfo info;
			info.strProcessName = pi[i].pProcessName;
			info.dwProcessId = pi[i].ProcessId;
			info.strUserName = AcctName;
			vecInfos.push_back(info);

			GlobalFree(AcctName);
			GlobalFree(DomainName);
		}

		WTSFreeMemory(pi);
	}
}

VOID Utils::GetFileInfo(LPCTSTR pFilePath, CString& strFileDesc, CString& strProductDesc, CString& strFileVersion)
{
	BOOL bResult = FALSE;
	DWORD dwSize = GetFileVersionInfoSize(pFilePath, NULL);
	if (0 != dwSize)
	{
		LPVOID pBlock = malloc(dwSize);
		if (NULL != pBlock)
		{
			BOOL bRet = GetFileVersionInfo(pFilePath, 0, dwSize, pBlock);
			if (bRet)
			{
				LPBYTE pTranslationValue = NULL;
				UINT nTranslationSize = 0;
				bRet = VerQueryValue(pBlock, TEXT("\\VarFileInfo\\Translation"), (LPVOID*)&pTranslationValue, &nTranslationSize);
				if (bRet && 0 != nTranslationSize)
				{
					std::vector<CString> vecTypes;
					std::vector<CString> vecRet;
					vecTypes.push_back(_T("FileDescription"));
					vecTypes.push_back(_T("ProductName"));
					vecTypes.push_back(_T("FileVersion"));
					for (int i = 0; i < (int)vecTypes.size(); i++)
					{
						CString strValue;
						LPBYTE pValue = NULL;
						UINT nValueSize = 0;
						TCHAR szQuery[MAX_PATH];
						wsprintf(szQuery, _T("\\StringFileInfo\\%04X%04X\\%s"), *(WORD *)pTranslationValue, *(WORD *)(pTranslationValue + 2), vecTypes[i]);
						bRet = VerQueryValue(pBlock, szQuery, (LPVOID*)&pValue, &nValueSize);
						if (bRet)
						{
							LPTSTR pTmp = new TCHAR[nValueSize + 1];
							ZeroMemory(pTmp, (nValueSize + 1) * sizeof(TCHAR));
							memcpy(pTmp, pValue, nValueSize * sizeof(TCHAR));
							strValue = pTmp;
							strValue.Trim();
							delete[] pTmp;
						}
						vecRet.push_back(strValue);
					}
					strFileDesc = vecRet[0];
					strProductDesc = vecRet[1];
					strFileVersion = vecRet[2];
				}
			}

			free(pBlock);
			pBlock = NULL;
		}
	}
}

CString Utils::GetProcessName(DWORD dwProcessId)
{
	PROCESSENTRY32 pe32 = {0};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != (HANDLE)-1)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			while(1)
			{
				if (pe32.th32ProcessID == dwProcessId)
				{
					CloseHandle(hProcessSnap);
					return pe32.szExeFile;
				}

				if (!Process32Next(hProcessSnap, &pe32))
				{
					break;
				}
			}
		}
		CloseHandle(hProcessSnap);
	}
	return _T("");
}

VOID Utils::KillProcessName(CString strProcessName)
{
	PROCESSENTRY32 pe32 = {0};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != (HANDLE)-1)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			while(1)
			{
				if (0 == strProcessName.CompareNoCase(pe32.szExeFile))
				{
					HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
					if (hProc != 0)
					{
						TerminateProcess(hProc, -1);
					}
				}

				if (!Process32Next(hProcessSnap, &pe32))
				{
					break;
				}
			}
		}
		CloseHandle(hProcessSnap);
	}
}

VOID Utils::KillProcessNameFuzzy(CString strProcessName, DWORD dwIngnorePid /*= 0*/)
{
	PROCESSENTRY32 pe32 = {0};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != (HANDLE)-1)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			while(1)
			{
				if (pe32.th32ProcessID != dwIngnorePid)
				{
					CString strName = pe32.szExeFile;
					strName.MakeUpper();
					if (-1 != strProcessName.MakeUpper().Find(strName) ||
						-1 != strName.MakeUpper().Find(strProcessName))
					{
						HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
						if (hProc != 0)
						{
							TerminateProcess(hProc, -1);
						}
					}
				}

				if (!Process32Next(hProcessSnap, &pe32))
				{
					break;
				}
			}
		}
		CloseHandle(hProcessSnap);
	}
}

VOID Utils::KillProcessDesc(CString strDesc)
{
	PROCESSENTRY32 pe32 = {0};
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap != (HANDLE)-1)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			while(1)
			{
				/*


				if (0 == strProcessName.CompareNoCase(pe32.szExeFile))
				{
					HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
					if (hProc != 0)
					{
						TerminateProcess(hProc, -1);
					}
				}
				*/

				if (!Process32Next(hProcessSnap, &pe32))
				{
					break;
				}
			}
		}
		CloseHandle(hProcessSnap);
	}
}

BOOL Utils::GetPidInfo(DWORD dwPid, CString& strFileDesc, CString& strProductDesc, CString& strFileVersion)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if (NULL != hProcess)
	{
		TCHAR strProcPath[_MAX_PATH] = {0};
		GetModuleFileNameEx(hProcess, NULL, strProcPath, _MAX_PATH);
		if (0 != lstrlen(strProcPath))
		{
			std::map<CString, FILE_INFO_MAP>::iterator pIter = m_FileInfoMap.find(strProcPath);
			if (pIter == m_FileInfoMap.end())
			{
				Utils::GetFileInfo(strProcPath, strFileDesc, strProductDesc, strFileVersion);

				FILE_INFO_MAP infoMap;
				infoMap.strFileDesc = strFileDesc;
				infoMap.strProductDesc = strProductDesc;
				m_FileInfoMap.insert(std::map<CString, FILE_INFO_MAP>::value_type(strProcPath, infoMap));
			}
			else
			{
				strFileDesc = pIter->second.strFileDesc;
				strProductDesc = pIter->second.strProductDesc;
			}
		}
		CloseHandle(hProcess);

		return TRUE;
	}

	return FALSE;
}


BOOL Utils::GetProcessId(LPTSTR pName, std::vector<DWORD>& vecIds)
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};

	//
	// Take a snapshot of all processes in the system.
	//
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == (HANDLE)-1)
	{
		return FALSE;
	}

	//
	// Fill in the size of the structure before using it.
	//
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//
	// Walk the snapshot of the processes, and for each process,
	// display information.
	//
	if (Process32First(hProcessSnap, &pe32))
	{
		while(1)
		{
			if (NULL != _tcsstr(pe32.szExeFile, pName))
			{
				vecIds.push_back(pe32.th32ProcessID);
			}

			if (!Process32Next(hProcessSnap, &pe32))
			{
				break;
			}
		}
	}

	return TRUE;
}

BOOL Utils::GetProcessId(std::vector<DWORD>& vecIds)
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};

	//
	// Take a snapshot of all processes in the system.
	//
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == (HANDLE)-1)
	{
		return FALSE;
	}

	//
	// Fill in the size of the structure before using it.
	//
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//
	// Walk the snapshot of the processes, and for each process,
	// display information.
	//
	if (Process32First(hProcessSnap, &pe32))
	{
		while(1)
		{
			vecIds.push_back(pe32.th32ProcessID);

			if (!Process32Next(hProcessSnap, &pe32))
			{
				break;
			}
		}
	}

	return TRUE;
}

BOOL Utils::GetProcessName(std::vector<CString>& vecNames, std::vector<DWORD>& vecIds)
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};

	//
	// Take a snapshot of all processes in the system.
	//
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == (HANDLE)-1)
	{
		return FALSE;
	}

	//
	// Fill in the size of the structure before using it.
	//
	pe32.dwSize = sizeof(PROCESSENTRY32);

	//
	// Walk the snapshot of the processes, and for each process,
	// display information.
	//
	if (Process32First(hProcessSnap, &pe32))
	{
		while(1)
		{
			vecNames.push_back(pe32.szExeFile);
			vecIds.push_back(pe32.th32ProcessID);

			if (!Process32Next(hProcessSnap, &pe32))
			{
				break;
			}
		}
	}
	return TRUE;
}

BOOL Utils::CheckPid(DWORD dwProcessId)
{
// 	std::vector<DWORD> vecIds;
// 	Utils::GetProcessId(_T("360Safe"), vecIds);
// 	std::vector<DWORD> vecIdsTmp;
// 	Utils::GetProcessId(_T("ZhuDongFangYu"), vecIdsTmp);
// 	vecIds.insert(vecIds.end(), vecIdsTmp.begin(), vecIdsTmp.end());
// 	Utils::GetProcessId(_T("explorer"), vecIdsTmp);
// 	vecIds.insert(vecIds.end(), vecIdsTmp.begin(), vecIdsTmp.end());

	if (GetCurrentProcessId() == dwProcessId)
	{
		return TRUE;
	}

	std::vector<DWORD> vecIds;
	std::vector<CString> vecEnumNames;
	std::vector<DWORD> vecEnumIds;
	GetProcessName(vecEnumNames, vecEnumIds);
	for (int i = 0; i < (int)vecEnumNames.size(); i++)
	{
		CString strProcessName = vecEnumNames[i].MakeLower();
		if (// Anti
			-1 != strProcessName.Find(_T("360safe"))		||
			-1 != strProcessName.Find(_T("zhudongfangyu"))	||
			-1 != strProcessName.Find(_T("360rp"))			||
			-1 != strProcessName.Find(_T("360rps"))			||
			-1 != strProcessName.Find(_T("360rd"))			||
			-1 != strProcessName.Find(_T("360sd"))			||
			-1 != strProcessName.Find(_T("360tray"))		||
			//
			-1 != strProcessName.Find(_T("explorer"))		||
			// Dev
			-1 != strProcessName.Find(_T("devenv"))			||
			-1 != strProcessName.Find(_T("msvsmon"))		||
			// Default
			-1 != strProcessName.Find(_T("searchui"))				||
			-1 != strProcessName.Find(_T("dllhost"))				||
			-1 != strProcessName.Find(_T("sihost"))					||
			-1 != strProcessName.Find(_T("shellexperiencehost"))	||
			-1 != strProcessName.Find(_T("ime"))
			)
		{
			vecIds.push_back(vecEnumIds[i]);
		}
	}
	BOOL bMatched = FALSE;
	for (int j = 0; j < (int)vecIds.size(); j++)
	{
		if (vecIds[j] == dwProcessId)
		{
			bMatched = TRUE;
			break;
		}
	}
	return bMatched;
}

VOID Utils::KillPid(DWORD dwProcessId)
{
	HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
	if (hProc != 0)
	{
		TerminateProcess(hProc, -1);
	}
}

#define LZMA_PREFIX_LEN	13
LPBYTE Utils::CompressData(LPBYTE pBuf, size_t iSize, size_t& iOutputSize)
{
	unsigned char prop[5] = {0};
	size_t sizeProp = 5;
	size_t iDestLen = 8 * 1024 + LZMA_PREFIX_LEN;
	LPBYTE pNewBuf = (LPBYTE)malloc(iDestLen);
	BOOL bRet = FALSE;
	while (TRUE)
	{
		int iCompressRet = LzmaCompress(pNewBuf + LZMA_PREFIX_LEN, &iDestLen, pBuf, iSize, prop, &sizeProp, 5, (1 << 24), 3, 0, 2, 32, 2);
		if (SZ_OK == iCompressRet)
		{
			iOutputSize = iDestLen + LZMA_PREFIX_LEN;
			bRet = TRUE;
			MoveMemory(pNewBuf, prop, 5);
			MoveMemory(pNewBuf + 5, &iSize, sizeof(size_t));
			break;
		}
		else if (SZ_ERROR_OUTPUT_EOF == iCompressRet)
		{
			iDestLen = 2 * iDestLen + LZMA_PREFIX_LEN;
			pNewBuf = (LPBYTE)realloc(pNewBuf, iDestLen);
		}
		else
		{
			break;
		}
	}
	if (!bRet)
	{
		free(pNewBuf);
		return NULL;
	}

	return pNewBuf;
}

LPBYTE Utils::DeCompressData(LPBYTE pBuf, size_t iSize, size_t& iOutputSize)
{
	unsigned char prop[5] = {0};
	size_t sizeProp = 5;
	MoveMemory(prop, pBuf, 5);
	size_t iDestLen = 0;
	MoveMemory(&iDestLen, pBuf + 5, sizeof(size_t));
	LPBYTE pNewBuf = (LPBYTE)malloc(iDestLen);
	BOOL bRet = FALSE;
	iSize -= LZMA_PREFIX_LEN;
	int iCompressRet = LzmaUncompress(pNewBuf, &iDestLen, pBuf + LZMA_PREFIX_LEN, &iSize, prop, sizeProp);
	if (SZ_OK == iCompressRet)
	{
		iOutputSize = iDestLen;
		bRet = TRUE;
	}
// 	else if (SZ_ERROR_INPUT_EOF == iCompressRet)
// 	{
// 		iDestLen *= 2;
// 		pNewBuf = (LPBYTE)realloc(pNewBuf, iDestLen);
// 	}
	if (!bRet)
	{
		free(pNewBuf);
		return NULL;
	}

	return pNewBuf;
}

BOOL Utils::SaveCapturedBitmap(HDC hCaptureDC, HBITMAP hCaptureBitmap, LPCTSTR lpFilePath)
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

BOOL Utils::CaptureScreenBitmap(LPCTSTR pDeskTop, LPCTSTR lpFilePath)
{
	BOOL bRet = FALSE;
	HWINSTA hwinstaCurrent = NULL;
	HDESK hdeskCurrent = NULL;
	HWINSTA hwinsta = NULL;
	HDESK hdesk = NULL;
	do
	{
		hwinstaCurrent = GetProcessWindowStation();
		if (hwinstaCurrent == NULL)
		{
			OutputLog(_T("Get Window Station Error"));
			break;
		}

		hdeskCurrent = GetThreadDesktop(GetCurrentThreadId());
		if (hdeskCurrent == NULL)
		{
			OutputLog(_T("Get Window Desktop Error"));
			break;
		}

		hwinsta = OpenWindowStation(TEXT("winsta0"), FALSE, WINSTA_ALL_ACCESS);
		if (hwinsta == NULL)
		{
			OutputLog(_T("Open Window Station Error"));
			break;
		}

		if (!SetProcessWindowStation(hwinsta))
		{
			OutputLog(_T("Set Window Station Error"));
			break; 
		}

#define DESKTOP_OPTION		DESKTOP_CREATEMENU| DESKTOP_CREATEWINDOW |\
	DESKTOP_ENUMERATE			| DESKTOP_HOOKCONTROL		|\
	DESKTOP_JOURNALPLAYBACK		| DESKTOP_JOURNALRECORD		|\
	DESKTOP_READOBJECTS			| DESKTOP_SWITCHDESKTOP		|\
	DESKTOP_WRITEOBJECTS

		hdesk = OpenDesktop(pDeskTop /*SZ_DESKTOP_WINLOGON*/, 0, FALSE, MAXIMUM_ALLOWED);
		if (hdesk == NULL)
		{
			OutputLog(_T("Open Desktop Error"));
			break;
		}

		if(!SetThreadDesktop(hdesk))
		{
			OutputLog(_T("Can't Set Thread Desktop"));
			break;
		}

		bRet = TRUE;
	}
	while (FALSE);
	if (bRet)
	{
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
		HWND hDesktopWnd = GetDesktopWindow();
		if (NULL != hDesktopWnd)
		{
			HDC hDesktopDC = GetDC(hDesktopWnd);
			HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
			HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
			SelectObject(hCaptureDC,hCaptureBitmap); 
			BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight, hDesktopDC,0,0,SRCCOPY|CAPTUREBLT); 
			bRet = SaveCapturedBitmap(hCaptureDC, hCaptureBitmap, lpFilePath);
			ReleaseDC(hDesktopWnd, hDesktopDC);
			DeleteDC(hCaptureDC);
			DeleteObject(hCaptureBitmap);
		}
		else
			bRet = FALSE;
	}

	if (hwinstaCurrent)
	{
		SetProcessWindowStation(hwinstaCurrent);
		hwinstaCurrent = NULL;
	}

	if (hdeskCurrent)
	{
		SetThreadDesktop(hdeskCurrent);
		hdeskCurrent = NULL;
	}

	if (hwinsta)
	{
		CloseWindowStation(hwinsta);
		hwinsta = NULL;
	}

	if (hdesk)
	{
		CloseDesktop(hdesk);
		hdesk = NULL;
	}

	return bRet;
}

BOOL Utils::EnumServicesItems(SC_HANDLE hSCHandle, LPENUM_SERVICE_STATUS pServiceStatus, DWORD dwServices, std::vector<SERIVCE_ITEM>& vecContent)
{
	LPBYTE pConfigBuf = NULL;
	for (DWORD dwIndex = 0; dwIndex < dwServices; ++dwIndex)
	{
		SC_HANDLE hService = OpenService(hSCHandle, pServiceStatus->lpServiceName, GENERIC_READ);
		if (NULL == hService)
			continue;

		DWORD cbBytesNeeded = 0;
		QueryServiceConfig(hService, NULL, 0, &cbBytesNeeded);
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		{
			CloseServiceHandle(hService);
			continue;
		}

		pConfigBuf = new BYTE[cbBytesNeeded];
		BOOL bRet = QueryServiceConfig(hService, (LPQUERY_SERVICE_CONFIG)pConfigBuf, 
			cbBytesNeeded, &cbBytesNeeded);
		if (!bRet)
		{
			CloseServiceHandle(hService);
			delete[] pConfigBuf;
			continue;
		}

		LPQUERY_SERVICE_CONFIG pServiceConfig = (LPQUERY_SERVICE_CONFIG)pConfigBuf;
		//printf("---------------------------------------\n");
		//printf("DisplayName: %ws\n", pServiceConfig->lpDisplayName);
		//printf("BinaryPathName: %ws\n", pServiceConfig->lpBinaryPathName);
		//printf("ServiceStartName: %ws\n", pServiceConfig->lpServiceStartName);

		//strContent += pServiceStatus->lpServiceName;
		//strContent += _T("\"");
		//strContent += pServiceConfig->lpDisplayName;
		//strContent += _T("|");
		SERIVCE_ITEM item;
		item.strServiceName = pServiceStatus->lpServiceName;
		item.strDisplayName = pServiceStatus->lpDisplayName;
		vecContent.push_back(item);

		CloseServiceHandle(hService);
		delete[] pConfigBuf;
		++pServiceStatus;
	}
	return TRUE;
}

BOOL Utils::Service(std::vector<SERIVCE_ITEM>& vecContent)
{
	SC_HANDLE hSCHandle = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, GENERIC_READ);
	if ( hSCHandle == NULL )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	LPBYTE pDataBuffer = NULL;
	do
	{
		DWORD dwServices = 0;
		DWORD dwResumeHandle = 0;
		DWORD cbBytesNeeded = 0;

		// 这里只枚举服务.获取服务数据长度
		// 枚举服务项: SERVICE_WIN32
		// 枚举驱动服务项: SERVICE_DRIVER
		EnumServicesStatus(hSCHandle, SERVICE_WIN32, SERVICE_STATE_ALL, NULL,
			0, &cbBytesNeeded, &dwServices, &dwResumeHandle);

		if (GetLastError() != ERROR_MORE_DATA)
			break;

		pDataBuffer = new BYTE[cbBytesNeeded];

		// 开始获取服务项
		LPENUM_SERVICE_STATUS pServiceStatus = (LPENUM_SERVICE_STATUS)pDataBuffer;
		BOOL bEnumRet = EnumServicesStatus( hSCHandle, SERVICE_WIN32, SERVICE_STATE_ALL, 
			pServiceStatus, cbBytesNeeded, &cbBytesNeeded, &dwServices, &dwResumeHandle );

		if (!bEnumRet)
			break;

		// 枚举服务项信息
		EnumServicesItems(hSCHandle, pServiceStatus, dwServices, vecContent);

		bRet = TRUE;
	}
	while(FALSE);
	if (hSCHandle != NULL)
	{
		CloseServiceHandle(hSCHandle);
		hSCHandle = NULL;
	}
	if (NULL != pDataBuffer)
	{
		delete[] pDataBuffer;
		pDataBuffer = NULL;
	}

	return bRet;
}

BOOL Utils::Process(std::vector<PROCESS_ITEM>& vecItems)
{
	HANDLE hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = {0};

	//
	// Take a snapshot of all processes in the system.    
	//
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);    
	if(hProcessSnap == (HANDLE)-1)    
	{
		return FALSE;
	}

	//
	// Fill in the size of the structure before using it.
	//
	pe32.dwSize = sizeof(PROCESSENTRY32);    

	//
	// Walk the snapshot of the processes, and for each process,    
	// display information.    
	//
	DWORD dwServices = 0;
	if (Process32First(hProcessSnap, &pe32))    
	{  
		while(1)  
		{
		//	TCHAR szTmp[128];
		//	wsprintf(szTmp, _T("%s\"%d|"), pe32.szExeFile, pe32.th32ProcessID);
		//	strContent += szTmp;

			PROCESS_ITEM item;
			item.strName = pe32.szExeFile;
			item.dwProcessId = pe32.th32ProcessID;
			vecItems.push_back(item);

			if (!Process32Next(hProcessSnap, &pe32))
			{
				break;
			}
		}
	}
	CloseHandle(hProcessSnap);
	return TRUE;
}

int Utils::CheckWow64(BOOL &bWow64)
{
#ifndef PROCESSOR_ARCHITECTURE_AMD64
#define PROCESSOR_ARCHITECTURE_AMD64 9
#endif
	HMODULE hLib = LoadLibrary(TEXT("kernel32.dll"));
	if (NULL == hLib)
	{
		return -1;
	}

	typedef void (WINAPI *_GETNATIVESYSTEMINFO)(LPSYSTEM_INFO);
	_GETNATIVESYSTEMINFO pGetNativeSystemInfo = (_GETNATIVESYSTEMINFO)GetProcAddress(hLib, "GetNativeSystemInfo");
	if (NULL == pGetNativeSystemInfo)
	{
		FreeLibrary(hLib);
		return -1;
	}

	SYSTEM_INFO si;
	pGetNativeSystemInfo(&si);
	bWow64 = FALSE;
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		bWow64 = TRUE;
	}
	FreeLibrary(hLib);
	return 0;
}

DWORD Utils::GetCurrentTimestamp()
{
	COleDateTime oleDate = COleDateTime::GetCurrentTime();
	return GetCurrentTimestamp(oleDate);
}

DWORD Utils::GetCurrentTimestamp(COleDateTime oleDate)
{
	SYSTEMTIME sysTime;
	oleDate.GetAsSystemTime(sysTime);
	CTime dateNew(sysTime);
	return (DWORD)dateNew.GetTime();
}
