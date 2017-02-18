#pragma once

#include <vector>
#include <map>
#include <set>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <SetupAPI.h>
#include <WinSvc.h>

class Utils
{
public:
	static std::vector<CString> Split(CString str, LPCTSTR pDelim);
	static std::vector<CString> SplitFull(CString str, LPCTSTR pDelim);
	static BOOL Extractfile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR lpPath);
	static BOOL Decompress(LPCSTR pFile, LPCTSTR pDstDir);
	static BOOL ExtractCompressedFile(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPCTSTR pPath, LPCTSTR pDir);
	struct ProcessInfo
	{
		CString strProcessName;
		DWORD dwProcessId;
		CString strUserName;
	};
	static VOID GetProcessNames(std::vector<ProcessInfo>& vecInfos);
	struct FILE_INFO_MAP
	{
		FILETIME ftModify;
		CString strFileDesc;
		CString strProductDesc;
	};
	static std::map<CString, FILE_INFO_MAP> m_FileInfoMap;
	static VOID GetFileInfo(LPCTSTR pFilePath, CString& strFileDesc, CString& strProductDesc, CString& strFileVersion);
	static VOID KillProcessName(CString strProcessName);
	static VOID KillProcessNameFuzzy(CString strProcessName, DWORD dwIngnorePid /*= 0*/);
	static VOID KillProcessDesc(CString strDesc);
	static BOOL GetPidInfo(DWORD dwPid, CString& strFileDesc, CString& strProductDesc, CString& strFileVersion);
	static BOOL GetProcessId(LPTSTR pName, std::vector<DWORD>& vecIds);
	static BOOL GetProcessId(std::vector<DWORD>& vecIds);
	static BOOL GetProcessName(std::vector<CString>& vecNames, std::vector<DWORD>& vecIds);
	static CString GetProcessName(DWORD dwProcessId);
	static BOOL CheckPid(DWORD dwProcessId);
	static VOID KillPid(DWORD dwProcessId);
	static LPBYTE CompressData(LPBYTE pBuf, size_t iSize, size_t& iOutputSize);
	static LPBYTE DeCompressData(LPBYTE pBuf, size_t iSize, size_t& iOutputSize);
	static BOOL SaveCapturedBitmap(HDC hCaptureDC, HBITMAP hCaptureBitmap, LPCTSTR lpFilePath);
	static BOOL CaptureScreenBitmap(LPCTSTR pDeskTop, LPCTSTR lpFilePath);

	struct SERIVCE_ITEM
	{
		CString strServiceName;
		CString strDisplayName;
	};
	static BOOL EnumServicesItems(SC_HANDLE hSCHandle, LPENUM_SERVICE_STATUS pServiceStatus, DWORD dwServices, std::vector<SERIVCE_ITEM>& vecContent);
	static BOOL Service(std::vector<SERIVCE_ITEM>& vecContent);
	
	struct PROCESS_ITEM
	{
		CString strName;
		DWORD dwProcessId;
	};
	static BOOL Process(std::vector<PROCESS_ITEM>& vecItems);
	static int CheckWow64(BOOL &bWow64);
	static DWORD GetCurrentTimestamp();
	static DWORD GetCurrentTimestamp(COleDateTime oleDate);
};