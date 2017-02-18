#include "StdAfx.h"
#include "Conv.h"

LPTSTR CConv::UtfToChar(LPCSTR pInput)
{
	return CustomToChar(pInput, CP_UTF8, NULL);
}

LPSTR CConv::CharToUtf(LPCTSTR pInput, LPDWORD lpRetLen /*= NULL*/)
{
	return CharToCustom(pInput, CP_UTF8, lpRetLen);
}

LPTSTR CConv::CustomToChar(LPCSTR pInput, UINT uCodePage, LPDWORD lpRetLen /*= NULL*/)
{
	// Convert UTF to Unicode
	INT iUnicodeSize = MultiByteToWideChar(uCodePage, 0, pInput, -1, NULL, 0);
	PWCHAR pUnicode = new WCHAR[iUnicodeSize + 1];  
	memset(pUnicode, 0, (iUnicodeSize + 1) * sizeof(WCHAR)); 
	MultiByteToWideChar(uCodePage, 0, pInput, -1, (LPWSTR)pUnicode, iUnicodeSize);  
#ifndef UNICODE
	//
	INT iAnsiSize = WideCharToMultiByte(CP_OEMCP,NULL, pUnicode, -1, NULL, 0, NULL, FALSE);
	LPSTR lpszStr = new CHAR[iAnsiSize];
	WideCharToMultiByte(CP_OEMCP, NULL, pUnicode, -1, lpszStr, iAnsiSize, NULL, FALSE);
	delete[] pUnicode;
	pUnicode = NULL;
	return lpszStr;
#else
	//
	return pUnicode;
#endif
}

LPSTR CConv::CharToCustom(LPCTSTR pInput, UINT uCodePage, LPDWORD lpRetLen /*= NULL*/)
{
#ifndef UNICODE
	// Convert
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, pInput, -1, NULL, 0);
	PWCHAR pUnicode = new WCHAR[dwNum];
	MultiByteToWideChar (CP_ACP, 0, pInput, -1, pUnicode, dwNum);

	dwNum = WideCharToMultiByte(uCodePage, NULL, pUnicode, -1, NULL, 0, NULL, FALSE);
	PCHAR pText = new CHAR[dwNum];
	WideCharToMultiByte(uCodePage, NULL, pUnicode, -1, pText, dwNum, NULL, FALSE);
	delete[] pUnicode;
	pUnicode = NULL;
#else
	// Convert TCHAR to UTF
	DWORD dwNum = WideCharToMultiByte(uCodePage, NULL, pInput, -1, NULL, 0, NULL, FALSE);
	PCHAR pText = new CHAR[dwNum];
	WideCharToMultiByte(uCodePage, NULL, pInput, -1, pText, dwNum, NULL, FALSE);
#endif
	if (lpRetLen)
		*lpRetLen = dwNum - 1;
	return pText;
}