#pragma once

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>  
#include <winsock2.h>  

#include <event.h>  
#include <evhttp.h>

class CEventHttpServer
{
public:
	CEventHttpServer(void);
	~CEventHttpServer(void);

public:
	BOOL Bind(USHORT uPort);
	VOID UnBind();

private:
	struct evhttp *mHttpd;
	struct event_base *mEventBase;
	HANDLE m_ServerRunHandle;
};
