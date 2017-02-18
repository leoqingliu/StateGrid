#pragma once

#include <string>
#include <map>
using namespace std;

#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif

class CGlobalUtility
{
public:
	CGlobalUtility(void);
	~CGlobalUtility(void);
	static HICON GetIconFromFileExt( LPCTSTR lpszPathFilename, BOOL bDirectory );
	static map<tstring, HICON> m_MapIcon;

	// 2 CHARS
	static BYTE MakeByte(CString str);

	// 4 CHARS
	static WORD MakeWord(CString str);

	// 8 CHARS
	static DWORD MakeDWord(CString str);
	
	static PTSTR BigNumToString(LONGLONG lNum, PTSTR szBuf);
	static LPTSTR FileSizeToString(DWORD dwFileSize, LPTSTR lpSize);
	static BOOL Extractfile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpPath);

	//static LONG CheckMSNPassword(LPCSTR lpUser, LPCSTR lpPassword);
	static BOOL CreateDir(LPCTSTR pDir);
};
