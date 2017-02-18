#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <tchar.h>
#include <comdef.h>
#include <shlobj.h>
#include <SHELLAPI.H>
#include <assert.h>

//#define _OUTPUT_DEBUG_
#include "public/Debug.h"

// Verify
#ifdef _DEBUG
	#ifndef VERIFY
	#define VERIFY(f)          assert(f)
	#endif
#else
	#define VERIFY(f)          ((void)(f))
#endif

// string
#include <vector>
#include <string>
using namespace std;

#ifdef UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif

	
#ifndef SAFE_DELETE_AR
#define SAFE_DELETE_AR(ptr) if (ptr) { delete [] ptr; ptr = NULL; }
#endif
	
#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) if (ptr) { delete ptr; ptr = NULL; }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(ptr) if (ptr) { free(ptr); ptr = NULL; }
#endif

// Connect Set File
//#define FILE_CONNECT_CONFIG_PATH _T("\\Program Files\\Internet Explorer\\SafeNet")
//#define FILE_CONNECT_CONFIG_NAME _T("SafeCredential.DAT")

// SafeNet File(DLL)
//#define FILE_SAFENET_PATH		_T("\\Program Files\\Internet Explorer\\SafeNet")
//#define FILE_SAFENET_NAME		_T("SafeExt.dll")
//#define FILE_SAFENET_NAME_ORG	_T("SafeExt.org")

#define SERVICE_DLL_FILE_NAME	_T("CommuExt.dll")
#define SERVICE_DLL_NAME		_T("MacMan-Service")
#define SERVICE_DLL_DESC		_T("Mac Manager Service")

#define CONFIG_KEY				"Oay5SEfpixZZRZPBCF5C6pJJdyS9Fvb9Ul992vPhEgrETRtsVpo2gwWL6W9BKdKn"

#define COMMUNICATION_KEY		"6dNfg8Upn5fBzGgj8licQHblQvLnUY19z5zcNKNFdsDhUzuI8otEsBODrzFCqCKr"

#define IPC_SHARED_MMF_INFO		_T("Global\\{706DD2CA-3FDB-44a7-ABF2-91FEC3F72880}")
#define IPC_SHARED_MMF_LOCK		_T("Global\\{4C9DB58C-1BEC-4ca4-9094-5F1683321AAF}")

#define PIPE_NAME				_T("SCREEN_LOCKER_PIPE_SERVER")
#define PIPE_SERVICE_NAME		_T("SERVICE_LOCKER_PIPE_SERVER")

#define SERVICE_VNC_NAME		_T("tvnserver")


#define PIPE_TYPE_USB			100
#define PIPE_TYPE_LOGOFF		101
#define PIPE_TYPE_LOCK			102
#define PIPE_TYPE_UNLOCK		103
#define PIPE_TYPE_INFO			104

#define CLIENT_VERSION			500

#endif