#pragma once

#include <sys/types.h>
#include <winsock2.h>
#include <string.h>
#include <microhttpd.h>
#include <internal.h>
#include <stdio.h>

class CEventHttpFileServer
{
public:
	CEventHttpFileServer(void);
	~CEventHttpFileServer(void);

public:
	BOOL Bind(USHORT uPort);
	VOID UnBind();

private:
	struct MHD_Daemon *m_daemon;
};
