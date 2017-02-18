#ifndef _CONV_H
#define _CONV_H

class CConv
{
public:
	static LPTSTR UtfToChar(LPCSTR pInput);
	static LPTSTR CustomToChar(LPCSTR pInput, UINT uCodePage, LPDWORD lpRetLen = NULL);

	static LPSTR CharToUtf(LPCTSTR pInput, LPDWORD lpRetLen = NULL);
	static LPSTR CharToCustom(LPCTSTR pInput, UINT uCodePage, LPDWORD lpRetLen = NULL);
};

#endif