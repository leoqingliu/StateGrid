// HttpDownLoad.h: interface for the CHttpDownLoad class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HTTP_QUERY_H__
#define __HTTP_QUERY_H__

#define WIN32_LEAN_AND_MEAN
#include <wininet.h>
#include <Windows.h>
#include <tchar.h>

#include <string>
using namespace std;
#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#define DEFAULT_BYTES_BY_QUERY 10 * 1024

namespace HttpQuery
{

struct QueryInfo
{
	QueryInfo()
		:dwBytesPerSecond(0)
		,dwFileSize(0)
		,dwErrorCode(0)
		,dwBytesByQuery(DEFAULT_BYTES_BY_QUERY)
	{
	}
	DWORD dwBytesPerSecond;
	DWORD dwFileSize;
	DWORD dwErrorCode;
	DWORD dwBytesByQuery;
};

BOOL QuerfUrl(LPCTSTR lpHost, INTERNET_PORT wPort, LPCTSTR lpUrl,
			  LPCTSTR lpLocalFilePath,
			  QueryInfo &info,
			  LPBYTE *lpRetData = NULL,
			  DWORD *dwRetData = 0,
			  LPCTSTR lpMethod = _T("GET"),
			  LPVOID lpData = NULL,
			  DWORD wDataSize = 0
			  );


BOOL DownLoadFile(LPCTSTR lpHost, INTERNET_PORT wPort, LPCTSTR lpUrl,
				  LPCTSTR lpLocalFilePath,
				  QueryInfo &info);
BOOL PostFile( LPCTSTR lpHost, INTERNET_PORT wPort, LPCTSTR lpUrl, LPCTSTR lpLocalFilePath, LPCTSTR lpLocalFileName);

}

#endif // !defined(__HTTP_DOWNLOAD_H__)
