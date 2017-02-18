#pragma once
//
#if _MSC_VER <= 1200
#pragma warning(push)
#pragma warning(disable:4786)
#endif
//

#include "Redirect.h"

//
class CNetworkProcess;
namespace NCmd
{
	class CRedCmdCallBack
	{
	public:
		void virtual OnGetCommand(LPCTSTR lpszBuffer) = 0;
	};
	
	//
	class CRedCmd : public CRedirect
	{
	public:
		CRedCmd(void);
		~CRedCmd(void);

	protected:
		virtual void OnChildStarted(LPCTSTR lpszCmdLine);
		// Send stdout text to the display window.
		virtual void OnChildStdOutWrite(LPCBYTE lpszBuffer, DWORD nBytesRead);
		// Send stderr text to the display window.
		virtual void OnChildStdErrWrite(LPCBYTE lpszBuffer, DWORD nBytesRead);
		//
		void OnChildWrite(UINT OutputID, LPCBYTE lpszOutput, DWORD nBytesRead);
		// Child process is terminated correctly.
		virtual void OnChildTerminate();
		//
		CRedCmdCallBack* m_pCallBack;
		BOOL m_bFirstOut;
	public:
		void Initialize(CRedCmdCallBack* pCallBack);
		void WriteCommand(LPCTSTR Input, INT iType);
		void Create(LPCTSTR lpCmdPath);
		// Warning!!
		// Don't Call when App Destroy
		void Close();
	};
}

//
#if _MSC_VER <= 1200
#pragma warning(pop)
#endif
//