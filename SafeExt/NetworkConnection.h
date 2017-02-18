#ifndef _NETWORK_CONNECTION_H
#define _NETWORK_CONNECTION_H

struct NetworkConnectInfo
{
	// 0: TCP
	// 1: UDP
	BYTE	uType;
	TCHAR	szLocalAddr[128];
	USHORT	uLocalPort;
	TCHAR	szRemoteAddr[128];
	USHORT	uRemotePort;
	DWORD	dwPid;
	TCHAR	szProcessName[256];
	DWORD	dwStat;
	TCHAR	szStat[256];
};

NetworkConnectInfo *GetTcpInfo(INT &iCount);
NetworkConnectInfo *GetUdpInfo(INT &iCount);

#endif