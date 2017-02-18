#include "StdAfx.h"
#include "FileEx.h"

namespace NGlobalUtils
{
	CFileEx::CFileEx(void)
	{
	}

	CFileEx::~CFileEx(void)
	{
	}


	BOOL CFileEx::OpenFile(LPCTSTR lpFilePath, CFileStruct* pFileStruct, 
		BOOL bCanWrite /*= FALSE*/, BOOL bCreate /*= FALSE*/)
	{
		DWORD dwCreateFileAccess = GENERIC_READ;
		DWORD dwCreateFileShare	 = FILE_SHARE_READ;
		DWORD dwCreatePos		 = OPEN_EXISTING;
		if(bCanWrite)
		{
			dwCreateFileAccess	= GENERIC_READ | GENERIC_WRITE;//FILE_ALL_ACCESS;
			dwCreateFileShare	= FILE_SHARE_READ | FILE_SHARE_WRITE;
		}
		if(bCreate)
		{
			dwCreatePos			= CREATE_ALWAYS;
		}

		//Create the File handle
		pFileStruct->hFile = CreateFile(
			lpFilePath, 
			dwCreateFileAccess, 
			dwCreateFileShare, 
			0, 
			dwCreatePos, 
			FILE_ATTRIBUTE_NORMAL, 
			0);		
		if (pFileStruct->hFile == INVALID_HANDLE_VALUE)
		{	
			return FALSE; 
		} 

		return TRUE;
	}

	BOOL CFileEx::MapFile(CFileStruct* pFileStruct, BOOL bCanWrite /*= FALSE*/)
	{
		DWORD dwViewAccess		 = FILE_MAP_READ;
		DWORD dwMapProtect		 = PAGE_READONLY;
		if(bCanWrite)
		{
			dwViewAccess		= FILE_MAP_READ | FILE_MAP_WRITE;
			dwMapProtect		= PAGE_READWRITE | SEC_COMMIT;
		}

		if (!(pFileStruct->hFileMap = CreateFileMapping(
			pFileStruct->hFile, 0, dwMapProtect, 0, 0, 0)))
		{	
			CloseHandle(pFileStruct->hFile);
			CloseHandle(pFileStruct->hFileMap);
			return FALSE;
		}
		if (!(pFileStruct->pBasePointer = ::MapViewOfFile(
			pFileStruct->hFileMap, dwViewAccess, 0, 0, 0)))
		{				
			CloseHandle(pFileStruct->hFile);
			CloseHandle(pFileStruct->hFileMap);
			return FALSE;
		}
		return TRUE;
	}

	void CFileEx::CloseFile(CFileStruct* pFileStruct)
	{
		pFileStruct->Close();
	}

	DWORD CFileEx::GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh)
	{
		return ::GetFileSize(hFile, lpFileSizeHigh);
	}

}