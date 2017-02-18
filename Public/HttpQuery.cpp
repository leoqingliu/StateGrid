// HttpDownLoad.cpp: implementation of the CHttpDownLoad class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HttpQuery.h"
#include <memory>

#pragma comment(lib, "Wininet.lib")

namespace HttpQuery
{

BOOL QuerfUrl(LPCTSTR lpHost, INTERNET_PORT wPort, LPCTSTR lpUrl,
				  LPCTSTR lpLocalFilePath,
				  QueryInfo &info,
				  LPBYTE *lpRetData /*= NULL*/,
				  DWORD *dwRetData /*= 0*/,
				  LPCTSTR lpMethod /*= _T("GET")*/,
				  LPVOID lpData /*= NULL*/,
				  DWORD wDataSize /*= 0*/
				  )
{
	//OutputLog(_T("QuerfUrl: Host: %s, Port: %d\n"), lpHost, wPort);

	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hHttpFile = NULL;
	BOOL bRet = FALSE;
	DWORD dwTick = GetTickCount();
	do 
	{
		// Open Internet Session
		hSession = ::InternetOpen(
			_T("Fantasia"),
			PRE_CONFIG_INTERNET_ACCESS,
			NULL,
			INTERNET_INVALID_PORT_NUMBER,
			0);
		if (NULL == hSession)
		{
			OutputLog(_T("InternetOpen Failed"));
			break;
		}
		
		// Connect To Host
		hConnect = ::InternetConnect(
			hSession,
			lpHost,
			wPort,
			_T(""),
			_T(""),
			INTERNET_SERVICE_HTTP,
			0,
			0);
		if (NULL == hConnect)
		{
			OutputLog(_T("InternetConnect Failed"));
			break;
		}

		if(::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
		{
			OutputLog(_T("InternetAttemptConnect Failed"));
			break;
		}

		// Request File From Server
		hHttpFile = ::HttpOpenRequest(
			hConnect,
			lpMethod,
			lpUrl,
			HTTP_VERSION,
			NULL,
			0,
			//INTERNET_FLAG_DONT_CACHE,
			INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_RELOAD,
			0);
		if (NULL == hHttpFile)
		{
			OutputLog(_T("HttpOpenRequest Failed"));
			break;
		}

		//TCHAR szHeaders[] = _T("Connection: close");
		//BOOL bAddHeaders = ::HttpAddRequestHeaders(hHttpFile, szHeaders, lstrlen(szHeaders), HTTP_ADDREQ_FLAG_ADD);

		BOOL bSendRequest = ::HttpSendRequest(hHttpFile, NULL, 0, lpData, wDataSize);
		if (FALSE == bSendRequest)
		{
			OutputLog(_T("HttpSendRequest Failed\n"));
			break;
		}

//		ERROR_HTTP_HEADER_NOT_FOUND
		BOOL bQuery;

		/*
		bQuery = ::HttpEndRequest(hHttpFile, NULL, HSR_INITIATE, 0);
		if(!bQuery)
		{
			OutputLog(_T("HttpEndRequest Failed: %lu"), GetLastError());
			break;
		}
		*/

		TCHAR bufQuery[128];
		DWORD dwLengthBufQuery = _countof(bufQuery);

		bQuery = HttpQueryInfo(hHttpFile, HTTP_QUERY_STATUS_CODE,
			bufQuery, &dwLengthBufQuery, NULL);
		if (FALSE == bQuery)
		{
			OutputLog(_T("HttpQueryInfo Failed"));
			break;
		}
		DWORD dwHttpStatus = (DWORD)_ttol(bufQuery);
		if (dwHttpStatus >= 400)
		{
			OutputLog(_T("HttpStatus >= 400"));
			break;
		}

		// Get the length of the file
		dwLengthBufQuery = _countof(bufQuery);
		bQuery = ::HttpQueryInfo(hHttpFile, HTTP_QUERY_CONTENT_LENGTH,
			bufQuery, &dwLengthBufQuery, NULL);
		if (FALSE == bQuery)
		{
			OutputLog(_T("HttpQueryInfo Failed"));
			break;
		}

		// Convert length from ASCII string to a DWORD
		DWORD dwServerFileSize = (DWORD)_ttol(bufQuery);
		if (lpRetData)
		{
			*lpRetData = new (std::nothrow) BYTE[dwServerFileSize + 1];
			if (NULL == *lpRetData)
			{
				OutputLog(_T("Malloc FileSize Failed"));
				break;
			}
		}
		if (dwRetData)
			*dwRetData = dwServerFileSize;

		HANDLE hFile = INVALID_HANDLE_VALUE;
		if (lpLocalFilePath)
		{
			hFile = CreateFile(lpLocalFilePath, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if(INVALID_HANDLE_VALUE == hFile)
			{
				OutputLog(_T("CreateFile Failed"));
				break;
			}
		}

		// Allocate a buffer for the file
		DWORD dwBytesByQuery = info.dwBytesByQuery;
		BYTE *lpBuf = new (std::nothrow) BYTE[dwBytesByQuery];
		if(NULL == lpBuf)
		{
			if (lpLocalFilePath)
				CloseHandle(hFile);

			OutputLog(_T("Malloc Buffer Failed"));
			break;
		}

		DWORD dwSaved = 0;
		DWORD lLeft = dwServerFileSize;
		DWORD lBytes = dwBytesByQuery;
		BOOL bWriteOK = FALSE;
		while(TRUE)
		{
			if (lLeft == 0)
			{
				bWriteOK = TRUE;
				break;
			}

			if (lLeft >= dwBytesByQuery)
				lBytes = dwBytesByQuery;
			else
				lBytes = lLeft;
			lLeft -= lBytes;

			// Read the file into the buffer
			DWORD dwBytesRead;
			BOOL bRead = ::InternetReadFile(
				hHttpFile,
				lpBuf,
				lBytes,
				&dwBytesRead);
			if (FALSE == bRead || dwBytesRead != lBytes)
			{
				break;
			}
			
			if (lpRetData)
			{
				memcpy(*lpRetData + dwSaved, lpBuf, lBytes);
				dwSaved += lBytes;
			}

			if (lpLocalFilePath)
			{
				DWORD dwBytesWrite;
				BOOL bWrite = WriteFile(hFile, lpBuf, dwBytesRead, &dwBytesWrite, NULL);
				if(FALSE == bWrite || dwBytesWrite != lBytes)
				{
					break;
				}
			}
		}
		if (lpLocalFilePath)
			CloseHandle(hFile);
		
		delete[] lpBuf;
		lpBuf = NULL;

		//
		if (FALSE == bWriteOK)
		{
			OutputLog(_T("Write Failed"));
			break;
		}

// 		WIN32_FIND_DATA FindFileData;
// 		HANDLE hFind;
// 		hFind = FindFirstFile(lpLocalFilePath, &FindFileData);
// 		if (hFind == INVALID_HANDLE_VALUE)
// 		{
// 			break;
// 		}
// 		FindClose(hFind);

		if (lpLocalFilePath)
		{
			if (-1 == GetFileAttributes(lpLocalFilePath))
			{
				OutputLog(_T("GetFileAttributes Failed"));
				break;
			}
		}

		dwTick = GetTickCount() - dwTick;
		if (dwTick)
		{
			info.dwBytesPerSecond = (DWORD)(((__int64)dwServerFileSize * 1000) / dwTick);
		}
		info.dwFileSize = dwServerFileSize;

		//
		bRet = TRUE;
	}
	while (FALSE);

	if (FALSE == bRet)
	{
		info.dwErrorCode = GetLastError();
	}

	// Close all of the Internet handles
	if (hHttpFile)
	{
		::InternetCloseHandle(hHttpFile);
		hHttpFile = NULL;
	}
	if (hConnect)
	{
		::InternetCloseHandle(hConnect);
		hConnect = NULL;
	}
	if (hSession)
	{
		::InternetCloseHandle(hSession);
		hSession = NULL;
	}

	return bRet;
}

BOOL DownLoadFile(LPCTSTR lpHost, INTERNET_PORT wPort, LPCTSTR lpUrl,
				  LPCTSTR lpLocalFilePath,
				  QueryInfo &info)
{
	return QuerfUrl(lpHost, wPort, lpUrl, lpLocalFilePath, info);
}

BOOL PostFile(
			  LPCTSTR lpHost, INTERNET_PORT wPort, LPCTSTR lpUrl,
			  LPCTSTR lpLocalFilePath, LPCTSTR lpLocalFileName)
{
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	BOOL bRet = FALSE;
	DWORD dwTick = GetTickCount();
	do 
	{
		// Open Internet Session
		hSession = ::InternetOpen(
			_T("Fantasia"),
			PRE_CONFIG_INTERNET_ACCESS,
			NULL,
			INTERNET_INVALID_PORT_NUMBER,
			0);
		
		// Connect To Host
		hConnect = ::InternetConnect(
			hSession,
			lpHost, wPort,
			_T(""),
			_T(""),
			INTERNET_SERVICE_HTTP,
			0,
			0);
		if (NULL == hConnect)
		{
			OutputLog(_T("InternetConnect Failed"));
			break;
		}
		
		if(::InternetAttemptConnect(NULL) != ERROR_SUCCESS)
		{
			OutputLog(_T("InternetAttemptConnect Failed"));
			break;
		}

		HINTERNET hRequest = HttpOpenRequest(
			hConnect,
			_T("POST"),
			lpUrl,
			HTTP_VERSION,
			NULL,
			0,
			//INTERNET_FLAG_DONT_CACHE,
			INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_RELOAD,
			0);
		if (!hRequest)
		{
			OutputLog(_T("Failed to open request handle: %lu\n"), GetLastError());
			break;
		}
		
		
		tstring strBoundary = tstring(_T("------------------102c6b0af9c2"));
		tstring strHeader(_T("Content-Type: multipart/form-data, boundary="));
		strHeader += strBoundary;  	
		if (!HttpAddRequestHeaders(hRequest, strHeader.c_str(), DWORD(strHeader.size()), HTTP_ADDREQ_FLAG_ADD))
		{
			OutputLog(_T("Failed add head to request: %lu\n"), GetLastError());
			break;
		}
		
		hFile = CreateFile (lpLocalFilePath, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			OutputLog(_T("Failed to open local file %s\n"), lpLocalFilePath);
			break;
		}
		
		tstring strMultipartBegin(_T("--"));
		strMultipartBegin += strBoundary;
		strMultipartBegin += _T("\r\nContent-Disposition: form-data; name=\"up_file_path\"; filename=");
		strMultipartBegin += _T("\"");
		strMultipartBegin += lpLocalFileName;
		strMultipartBegin += _T("\"");
		strMultipartBegin += _T("\r\nContent-Type: application/octet-stream\r\n\r\n");
		tstring strMultipartEnd(_T("\r\n--"));  
		strMultipartEnd += strBoundary;  
		strMultipartEnd += _T("--\r\n");

		DWORD dwFileLen = GetFileSize (hFile, NULL);
		DWORD dwBufferTotal = strMultipartBegin.size() + dwFileLen + strMultipartEnd.size();  

		INTERNET_BUFFERS BufferIn;
		BufferIn.dwStructSize = sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
		BufferIn.Next = NULL; 
		BufferIn.lpcszHeader = NULL;
		BufferIn.dwHeadersLength = 0;
		BufferIn.dwHeadersTotal = 0;
		BufferIn.lpvBuffer = NULL;                
		BufferIn.dwBufferLength = 0;
		BufferIn.dwBufferTotal = dwBufferTotal; // This is the only member used other than dwStructSize
		BufferIn.dwOffsetLow = 0;
		BufferIn.dwOffsetHigh = 0;
		if (!HttpSendRequestEx( hRequest, &BufferIn, NULL, 0, 0))
		{
			OutputLog(_T("Error on HttpSendRequestEx %d\n"), GetLastError());
			break;
		}

		DWORD dwBytesWritten;
		USES_CONVERSION;
		// Send Begin
		LPSTR lpMultipartBegin = T2A((LPTSTR)strMultipartBegin.c_str());
		if (!InternetWriteFile(hRequest, lpMultipartBegin, strlen(lpMultipartBegin), &dwBytesWritten) ||
			strlen(lpMultipartBegin) != dwBytesWritten)
		{
			OutputLog(_T("Write Multipart Begin Failed %d\n"), GetLastError());
			break;
		}

		// Send File
		DWORD dwBytesRead = 0;
		BYTE pBuffer[1024]; // Read from file in 1K chunks
		DWORD sum = 0;
		do
		{
			if (!ReadFile (hFile, pBuffer, sizeof(pBuffer), &dwBytesRead, NULL))
			{
				OutputLog(_T("ReadFile failed on buffer %lu\n"), GetLastError());
				break;
			}
			if (!InternetWriteFile(hRequest, pBuffer, dwBytesRead, &dwBytesWritten))
			{
				OutputLog(_T("InternetWriteFile failed %lu\n"), GetLastError());
				break;
			}
			sum += dwBytesWritten;
		}
		while (dwBytesRead == sizeof(pBuffer));

		// Send End
		LPSTR lpMultipartEnd = T2A((LPTSTR)strMultipartEnd.c_str());
		if (!InternetWriteFile(hRequest, lpMultipartEnd, strlen(lpMultipartEnd), &dwBytesWritten) ||
			strlen(lpMultipartEnd) != dwBytesWritten)
		{
			OutputLog(_T("Write Multipart End Failed %d\n"), GetLastError());
			break;
		}

		if(!HttpEndRequest(hRequest, NULL, 0, 0))
		{
			OutputLog(_T("Error on HttpEndRequest %lu \n"), GetLastError());
			break;
		}

		bRet = TRUE;
	}
	while (FALSE);

	if (hConnect)
	{
		::InternetCloseHandle(hConnect);
		hConnect = NULL;
	}
	if (hSession)
	{
		::InternetCloseHandle(hSession);
		hSession = NULL;
	}
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return bRet;
}

}
