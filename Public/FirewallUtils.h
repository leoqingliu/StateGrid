#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdio.h>
#include <string>

#pragma comment(lib,"ws2_32.lib")

class FirewallUtils
{
public:
	FirewallUtils(void);
	~FirewallUtils(void);
	static BOOL AddPort(UINT uPort, LPCTSTR pName);
	//static BOOL IsPortExist(UINT uPort);
	static BOOL AddApp(LPCTSTR pPath, LPCTSTR pName);
};
