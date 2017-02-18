// Redirect.h : header file
//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//
#include <winscard.h>	// For LPCBYTE

/////////////////////////////////////////////////////////////////////////////
// CRedirect class
namespace NCmd
{
	#define BUFFER_SIZE 256

	#define STDIO_CLOSE 2
	#define STDIO_WRITE 3

	#define SET_WINDOW_TEXT   1
	#define STDIO_WRITE_TEXT  2
	#define STDERR_WRITE_TEXT 3

	class CRedirect
	{
		// Construction
	public:
		CRedirect();
		virtual ~CRedirect();

		void StartProcess(LPCTSTR lpszCmdLine, BOOL bShowChildWindow = FALSE);
	protected:
		BOOL IsChildRunning() const;
		BOOL WINAPI IsWinNT();
		
		void AddCommand(WPARAM wCommand, LPCSTR lpszCommand);
		BOOL StartChildProcess(LPCTSTR lpszCmdLine, BOOL bShowChildWindow = FALSE);
		void TerminateChildProcess();
		
		void WriteChildStdIn(LPCSTR lpszInput);
		BOOL ProcessStdioCommand( WPARAM wCommand, LPARAM lParam);

	protected:
		HANDLE m_hExitEvent;

		CRITICAL_SECTION m_criticalSection;
		vector<string> m_StdioCommand;

		// Child input(stdin) & output(stdout, stderr) pipes
		HANDLE m_hStdIn, m_hStdOut, m_hStdErr;
		// Parent output(stdin) & input(stdout) pipe
		HANDLE m_hStdInWrite, m_hStdOutRead, m_hStdErrRead;
		// stdout, stderr write threads
		HANDLE m_hStdOutThread, m_hStdErrThread;
		// Monitoring thread
		HANDLE m_hProcessThread;
		// Child process handle
		HANDLE m_hChildProcess;

		HANDLE PrepAndLaunchRedirectedChild(LPCTSTR lpszCmdLine,
			HANDLE hStdOut, HANDLE hStdIn, HANDLE hStdErr,
			BOOL bShowChildWindow);

		static BOOL m_bRunThread;
		static int staticStdOutThread(CRedirect *pRedirect)
		{ return pRedirect->StdOutThread(pRedirect->m_hStdOutRead); }
		static int staticStdErrThread(CRedirect *pRedirect)
		{ return pRedirect->StdErrThread(pRedirect->m_hStdErrRead); }
		static int staticProcessThread(CRedirect *pRedirect)
		{ return pRedirect->ProcessThread(); }
		int StdOutThread(HANDLE hStdOutRead);
		int StdErrThread(HANDLE hStdErrRead);
		int ProcessThread();

	public:
		virtual void OnChildStarted(LPCTSTR lpszCmdLine) {};
		virtual void OnChildStdOutWrite(LPCBYTE lpszOutput, DWORD nBytesRead) {};
		virtual void OnChildStdErrWrite(LPCBYTE lpszOutput, DWORD nBytesRead) {};
		virtual void OnChildTerminate() {};
	};

}