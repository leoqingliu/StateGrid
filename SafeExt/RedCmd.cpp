#include "stdafx.h"
#include "Redirect.h"
#include "RedCmd.h"

#if _MSC_VER <= 1200
#pragma warning(push)
#pragma warning(disable:4786)
#endif

namespace NCmd
{
	CRedCmd::CRedCmd(void)
	{
	}

	CRedCmd::~CRedCmd(void)
	{
		//Close();
	}

	void CRedCmd::OnChildStarted(LPCTSTR lpszCmdLine)
	{
	}

	void CRedCmd::OnChildStdOutWrite(LPCBYTE lpszBuffer, DWORD nBytesRead)
	{
		OnChildWrite(STDIO_WRITE_TEXT, lpszBuffer, nBytesRead);
	}

	void CRedCmd::OnChildStdErrWrite(LPCBYTE lpszBuffer, DWORD nBytesRead)
	{
		OnChildWrite(STDERR_WRITE_TEXT, lpszBuffer, nBytesRead);
	}

	void CRedCmd::OnChildTerminate()
	{
		//AddCommand(STDIO_CLOSE, _T(""));
	}

	//////////////////////////////////////////////////////////////////////////

	void CRedCmd::Initialize(CRedCmdCallBack* pCallBack)
	{
		m_pCallBack = pCallBack;
	}

	void CRedCmd::Create(LPCTSTR lpCmdPath)
	{
		//
		m_bFirstOut = FALSE;
		StartProcess(lpCmdPath);
	}

	void CRedCmd::Close()
	{
		AddCommand(STDIO_CLOSE, "");
	}

	void CRedCmd::WriteCommand(LPCTSTR Input, INT iType)
	{
		if(lstrcmpi(Input, _T("exit")) == 0)
		{
			AddCommand(STDIO_WRITE, "\n\r");
			//PostQuitMessage(0);
		}
		else
		{
			TCHAR szInput[512] = {0};
			wsprintf(szInput, _T("%s\n\r"), Input);
			m_bFirstOut = TRUE;
			USES_CONVERSION;
			LPSTR pOutput = T2A(szInput);
			AddCommand(iType, pOutput);
		}
	}

	void CRedCmd::OnChildWrite(UINT OutputID, LPCBYTE lpszOutput, DWORD nBytesRead)
	{
		if(m_bFirstOut) 
		{
			m_bFirstOut = FALSE;
			return;
		}

		USES_CONVERSION;
		LPTSTR pOutput = NULL;
		__try
		{
			pOutput = A2T((LPSTR)lpszOutput);
#if 0
			pOutput = CGlobalUtility::CustomToChar((LPCSTR)lpszOutput, 936);
			/*
			size_t iOutSize = lstrlen(pOutput);
			if(pOutput[iOutSize - 1] != _T('\r'))
			{
				LPTSTR lpNewBuffer = new TCHAR[iOutSize + 2];
				lstrcpy(lpNewBuffer, pOutput);
				lpNewBuffer[iOutSize] = _T('\r');
				lpNewBuffer[iOutSize + 1] = 0;

				delete[] pOutput;
				pOutput = NULL;

				pOutput = lpNewBuffer;
			}
			*/
#endif
			m_pCallBack->OnGetCommand(pOutput);
		}
		__finally
		{
		//	delete[] pOutput;
		//	pOutput = NULL;
		}
	}
}


//
#if _MSC_VER <= 1200
#pragma warning(pop)
#endif
