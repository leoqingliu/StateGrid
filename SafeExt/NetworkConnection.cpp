#include "StdAfx.h"
#include <windows.h>
#include <WINSOCK2.H>
#include <wtypes.h>
#include <tchar.h>
#include <Iprtrmib.h>
#include <TlHelp32.h>
#include <atlbase.h>
#include <atlconv.h>
#include "NetworkConnection.h"
#pragma comment(lib,"WS2_32.lib")

typedef struct{
	DWORD dwState;      //连接状态
	DWORD dwLocalAddr;  //本地地址
	DWORD dwLocalPort;  //本地端口
	DWORD dwRemoteAddr; //远程地址
	DWORD dwRemotePort; //远程端口
	DWORD dwProcessId;  //进程标识
}MIB_TCPEXROW,*PMIB_TCPEXROW;

typedef struct{
	DWORD dwLocalAddr;  //本地地址
	DWORD dwLocalPort;  //本地端口
	DWORD dwProcessId;  //进程标识
}MIB_UDPEXROW,*PMIB_UDPEXROW;

typedef struct{
	DWORD dwState;      //连接状态
	DWORD dwLocalAddr;  //本地地址
	DWORD dwLocalPort;  //本地端口
	DWORD dwRemoteAddr; //远程地址
	DWORD dwRemotePort; //远程端口
	DWORD dwProcessId;  //进程标识
	DWORD Unknown;      //待定标识
}MIB_TCPEXROW_VISTA,*PMIB_TCPEXROW_VISTA;

typedef struct{
	DWORD dwNumEntries;
	MIB_TCPEXROW table[ANY_SIZE];
}MIB_TCPEXTABLE,*PMIB_TCPEXTABLE;

typedef struct{
	DWORD dwNumEntries;
	MIB_TCPEXROW_VISTA table[ANY_SIZE];
}MIB_TCPEXTABLE_VISTA,*PMIB_TCPEXTABLE_VISTA;

typedef struct{
	DWORD dwNumEntries;
	MIB_UDPEXROW table[ANY_SIZE];
}MIB_UDPEXTABLE,*PMIB_UDPEXTABLE;

//enum OSVersion {XP,VISTA,OTHER};

typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(
	PMIB_TCPEXTABLE *pTcpTabel,
	bool bOrder,
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(
	PMIB_UDPEXTABLE *pUdpTable,
	bool bOrder,
	HANDLE heap,
	DWORD zero,
	DWORD flags
	);

typedef DWORD (WINAPI *_InternalGetTcpTable2)(
	PMIB_TCPEXTABLE_VISTA *pTcpTable_Vista,
	HANDLE heap,
	DWORD flags
	);

typedef DWORD (WINAPI *_InternalGetUdpTableWithOwnerPid)(
	PMIB_UDPEXTABLE *pUdpTable,
	HANDLE heap,
	DWORD flags
	);

LPCTSTR GetTcpState(DWORD dwStat)
{
	LPCTSTR pStr = NULL;
	switch (dwStat)
	{
	case MIB_TCP_STATE_CLOSED:
		pStr = _T("CLOSED");
		break;
	case MIB_TCP_STATE_LISTEN:
		pStr = _T("LISTEN");
		break;
	case MIB_TCP_STATE_SYN_SENT:
		pStr = _T("SYN-SENT");
		break;
	case MIB_TCP_STATE_SYN_RCVD:
		pStr = _T("SYN-RECEIVED");
		break;
	case MIB_TCP_STATE_ESTAB:
		pStr = _T("ESTABLISHED");
		break;
	case MIB_TCP_STATE_FIN_WAIT1:
		pStr = _T("FIN-WAIT-1");
		break;
	case MIB_TCP_STATE_FIN_WAIT2:
		pStr = _T("FIN-WAIT-2");
		break;
	case MIB_TCP_STATE_CLOSE_WAIT:
		pStr = _T("CLOSE-WAIT");
		break;
	case MIB_TCP_STATE_CLOSING:
		pStr = _T("CLOSING");
		break;
	case MIB_TCP_STATE_LAST_ACK:
		pStr = _T("LAST-ACK");
		break;
	case MIB_TCP_STATE_TIME_WAIT:
		pStr = _T("TIME-WAIT");
		break;
	case MIB_TCP_STATE_DELETE_TCB:
		pStr = _T("DELETE-TCB");
		break;
	default:
		pStr = _T("UNKNOWN dwState value");
		break;
	}
	return pStr;
}

BOOL GetProcessName(DWORD dwProcessId, LPTSTR pProcessName)
{
	/*
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pInfo->dwPid);
	if (hProcess)
	{
		HMODULE hMod;
		DWORD cbNeeded;
		if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
		{
			GetModuleBaseName(hProcess, hMod, pInfo->szProcessName, _countof(pInfo->szProcessName));
		}
	}
	*/

	/*
	pProcessName[0] = 0;
	BOOL bRet = FALSE;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE, dwProcessId);
	if (hProcess)
	{
		GetModuleBaseName(hProcess, NULL, pProcessName, MAX_PATH);
		bRet = TRUE;
		CloseHandle(hProcess);
	}
	*/

	pProcessName[0] = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return FALSE;
	}
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe = { sizeof(pe) };
	BOOL fOk;
	for (fOk = Process32First(hSnapshot, &pe); fOk; fOk = Process32Next(hSnapshot, &pe))
	{
		if(pe.th32ProcessID == dwProcessId)
		{
			lstrcpy(pProcessName, pe.szExeFile);
			bRet = TRUE;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return bRet;
}

NetworkConnectInfo *GetTcpInfo(INT &iCount)
{
	HMODULE hModule = LoadLibrary(_T("iphlpapi.dll"));
	if (hModule == NULL)
		return NULL;

	NetworkConnectInfo *pFullInfo = NULL;
	do 
	{
		PFNAllocateAndGetTcpExTableFromStack pAllcoteAndGetTcpExTableFromStack;
		pAllcoteAndGetTcpExTableFromStack = (PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");
		if (pAllcoteAndGetTcpExTableFromStack != NULL)
		{
			PMIB_TCPEXTABLE pTcpExTable;
			if (pAllcoteAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)
				break;

			iCount = pTcpExTable->dwNumEntries;
			pFullInfo = new NetworkConnectInfo[pTcpExTable->dwNumEntries];
			for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)
			{
				PMIB_TCPEXROW pTable = pTcpExTable->table + i;
				NetworkConnectInfo *pInfo = pFullInfo + i;
				memset(pInfo, 0, sizeof(NetworkConnectInfo));
				struct in_addr IpAddr;
				USES_CONVERSION;
				IpAddr.S_un.S_addr = (u_long)pTable->dwLocalAddr;
				lstrcpy(pInfo->szLocalAddr, A2T(inet_ntoa(IpAddr)));
				IpAddr.S_un.S_addr = (u_long) pTable->dwRemoteAddr;
				lstrcpy(pInfo->szRemoteAddr, A2T(inet_ntoa(IpAddr)));
				pInfo->uLocalPort = ntohs((u_short)pTable->dwLocalPort);
				pInfo->uRemotePort = ntohs((u_short)pTable->dwRemotePort);
				pInfo->dwPid = pTable->dwProcessId;
				pInfo->uType = 0;
				pInfo->dwStat = pTable->dwState;
				lstrcpy(pInfo->szStat, GetTcpState(pTable->dwState));
				GetProcessName(pInfo->dwPid, pInfo->szProcessName);
			}
		}
		else
		{
			PMIB_TCPEXTABLE_VISTA pTcpTable_Vista;
			_InternalGetTcpTable2 pGetTcpTable = (_InternalGetTcpTable2)GetProcAddress(hModule,"InternalGetTcpTable2");
			if (pGetTcpTable == NULL)
				break;

			if (pGetTcpTable(&pTcpTable_Vista, GetProcessHeap(), 1))
				break;

			iCount = pTcpTable_Vista->dwNumEntries;
			pFullInfo = new NetworkConnectInfo[pTcpTable_Vista->dwNumEntries];
			for (UINT i = 0; i < pTcpTable_Vista->dwNumEntries; i++)
			{
				PMIB_TCPEXROW_VISTA pTable = pTcpTable_Vista->table + i;
				NetworkConnectInfo *pInfo = pFullInfo + i;
				memset(pInfo, 0, sizeof(NetworkConnectInfo));
				struct in_addr IpAddr;
				USES_CONVERSION;
				IpAddr.S_un.S_addr = (u_long)pTable->dwLocalAddr;
				lstrcpy(pInfo->szLocalAddr, A2T(inet_ntoa(IpAddr)));
				IpAddr.S_un.S_addr = (u_long) pTable->dwRemoteAddr;
				lstrcpy(pInfo->szRemoteAddr, A2T(inet_ntoa(IpAddr)));
				pInfo->uLocalPort = ntohs((u_short)pTable->dwLocalPort);
				pInfo->uRemotePort = ntohs((u_short)pTable->dwRemotePort);
				pInfo->dwPid = pTable->dwProcessId;
				pInfo->uType = 0;
				GetProcessName(pInfo->dwPid, pInfo->szProcessName);
				lstrcpy(pInfo->szStat, GetTcpState(pTable->dwState));
				GetProcessName(pInfo->dwPid, pInfo->szProcessName);
			}
		}
	}
	while(FALSE);
	FreeLibrary(hModule);
	return pFullInfo;
}

NetworkConnectInfo *GetUdpInfo(INT &iCount)
{
	HMODULE hModule = LoadLibrary(_T("iphlpapi.dll"));
	if (hModule == NULL)
		return NULL;

	NetworkConnectInfo *pFullInfo = NULL;
	do 
	{
		PMIB_UDPEXTABLE pUdpExTable;
		PFNAllocateAndGetUdpExTableFromStack pAllcoteAndGetUdpExTableFromStack;
		pAllcoteAndGetUdpExTableFromStack = (PFNAllocateAndGetUdpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetUdpExTableFromStack");
		if (pAllcoteAndGetUdpExTableFromStack != NULL)
		{
			if (pAllcoteAndGetUdpExTableFromStack(&pUdpExTable, TRUE, GetProcessHeap(), 2, 2) != 0)
				break;

			iCount = pUdpExTable->dwNumEntries;
			pFullInfo = new NetworkConnectInfo[pUdpExTable->dwNumEntries];
			for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)
			{
				PMIB_UDPEXROW pTable = pUdpExTable->table + i;
				NetworkConnectInfo *pInfo = pFullInfo + i;
				memset(pInfo, 0, sizeof(NetworkConnectInfo));
				struct in_addr IpAddr;
				USES_CONVERSION;
				IpAddr.S_un.S_addr = (u_long)pTable->dwLocalAddr;
				lstrcpy(pInfo->szLocalAddr, A2T(inet_ntoa(IpAddr)));
				lstrcpy(pInfo->szRemoteAddr, _T("0.0.0.0"));
				pInfo->uLocalPort = ntohs((u_short)pTable->dwLocalPort);
				pInfo->uRemotePort = 0;
				pInfo->dwPid = pTable->dwProcessId;
				pInfo->uType = 1;
				GetProcessName(pInfo->dwPid, pInfo->szProcessName);
			}
		}
		else
		{
			_InternalGetUdpTableWithOwnerPid pGetUdpTable;
			pGetUdpTable = (_InternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");
			if (pGetUdpTable == NULL)
				break;
			
			if (pGetUdpTable(&pUdpExTable, GetProcessHeap(), 1))
				break;

			iCount = pUdpExTable->dwNumEntries;
			pFullInfo = new NetworkConnectInfo[pUdpExTable->dwNumEntries];
			for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)
			{
				PMIB_UDPEXROW pTable = pUdpExTable->table + i;
				NetworkConnectInfo *pInfo = pFullInfo + i;
				memset(pInfo, 0, sizeof(NetworkConnectInfo));
				struct in_addr IpAddr;
				USES_CONVERSION;
				IpAddr.S_un.S_addr = (u_long)pTable->dwLocalAddr;
				lstrcpy(pInfo->szLocalAddr, A2T(inet_ntoa(IpAddr)));
				lstrcpy(pInfo->szRemoteAddr, _T("0.0.0.0"));
				pInfo->uLocalPort = ntohs((u_short)pTable->dwLocalPort);
				pInfo->uRemotePort = 0;
				pInfo->dwPid = pTable->dwProcessId;
				pInfo->uType = 1;
				GetProcessName(pInfo->dwPid, pInfo->szProcessName);
			}
		}
	}
	while (FALSE);
	FreeLibrary(hModule);
	return pFullInfo;
}

