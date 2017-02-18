#include "StdAfx.h"
#include "IPC.h"

IPC::IPC(void)
{
	m_hFileMap = NULL;
}

IPC::~IPC(void)
{
}

BOOL IPC::Create(LPCTSTR pszName)
{
	if (m_hFileMap != NULL)
	{
		return FALSE;
	}

	SECURITY_ATTRIBUTES sa; 
	SECURITY_DESCRIPTOR sd; 
	InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = FALSE;

	m_hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,
		&sa,
		PAGE_READWRITE,
		0,
		1024 * 1024 * 10,
		pszName);
	if (m_hFileMap == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL IPC::Open(LPCTSTR pszName)
{
	if (m_hFileMap != NULL)
	{
		return TRUE;
	}

	m_hFileMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
		FALSE,
		pszName);
	if (m_hFileMap != NULL)
	{
		return TRUE;
	}

	return FALSE;
}

VOID IPC::Close()
{
	if (m_hFileMap != NULL)
	{
		CloseHandle(m_hFileMap);
		m_hFileMap = NULL;
	}
}

BOOL IPC::Read(LPBYTE pBuf, DWORD &dwBufSize)
{
	if (m_hFileMap == NULL)
		return FALSE;

	DWORD dwBaseMMF = (DWORD)MapViewOfFile(m_hFileMap,
		FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, 0);
	if (0 == dwBaseMMF)
	{
		return FALSE;
	}
	
	DWORD dwSizeofInBuf = dwBufSize;
	CopyMemory(&dwBufSize, (LPVOID)dwBaseMMF, sizeof(DWORD));

	BOOL bSuccess = TRUE;
	if (dwSizeofInBuf != 0)
	{
		if(dwBufSize > dwSizeofInBuf)
			bSuccess = FALSE;
		else
			CopyMemory(pBuf, (LPVOID)(dwBaseMMF + sizeof(DWORD)), dwBufSize);
	}
	UnmapViewOfFile((LPVOID)dwBaseMMF);
	return bSuccess;
}

BOOL IPC::Write(const LPBYTE pBuf, const DWORD dwBufSize)
{
	if (m_hFileMap == NULL)
		return FALSE;

	DWORD dwBaseMMF = (DWORD)MapViewOfFile(m_hFileMap,
		FILE_MAP_READ | FILE_MAP_WRITE,
		0, 0, 0);
	if (0 == dwBaseMMF)
	{
		return FALSE;
	}

	CopyMemory((LPVOID)dwBaseMMF, &dwBufSize, sizeof(DWORD));
	CopyMemory((LPVOID)(dwBaseMMF + sizeof(DWORD)), pBuf, dwBufSize);

	UnmapViewOfFile((LPVOID)dwBaseMMF);

	return TRUE;
}
