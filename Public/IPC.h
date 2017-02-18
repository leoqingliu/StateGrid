#pragma once

class IPC
{
public:
	IPC(void);
	~IPC(void);

public:
	BOOL Create(LPCTSTR pszName);
	BOOL Open(LPCTSTR pszName);
	VOID Close();

	BOOL IsOpen(void) const {return (m_hFileMap != NULL);}

	BOOL Read(LPBYTE pBuf, DWORD &dwBufSize);
	BOOL Write(const LPBYTE pBuf, const DWORD dwBufSize);

protected:
	HANDLE m_hFileMap;
};
