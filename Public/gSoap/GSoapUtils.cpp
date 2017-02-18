#include "StdAfx.h"
#include "GSoapUtils.h"
#include "stdsoap2.h"
//#include "add.h"
#include "ns.nsmap"
#include "resource.h"
#include "../SafeDiskManager/ProtoManager.h"
#include "../SafeDiskManager/SafeDiskManager.h"
#include "../SafeDiskManager/MainFrm.h"
#include "../Public/Utils.h"
#include "../Public/Conv.h"

BOOL LoadRes(HMODULE hInstance, LPCTSTR lpType, LPCTSTR lpName, LPBYTE& pBuff, DWORD& dwFileLen)
{
	// Find and Load the resource containing file(s) data
	HRSRC hResLoad = FindResource(hInstance, lpType, lpName);
	if (NULL == hResLoad)
	{
		return FALSE;
	}

	HGLOBAL hResData = LoadResource(hInstance, hResLoad);
	if (NULL == hResData)
	{
		return FALSE;
	}
	LPCSTR pData = (LPCSTR)LockResource(hResData);
	if (NULL == pData)
	{
		FreeResource(hResData);
		return FALSE;
	}

	dwFileLen = SizeofResource(hInstance, hResLoad);
	pBuff = (LPBYTE)malloc(dwFileLen);
	MoveMemory(pBuff, pData, dwFileLen);
	
	FreeResource(hResData);
	return TRUE;
}

int http_get(struct soap *soap)
{
	/*
	FILE *fd = NULL;
	fd = fopen("ns.wsdl", "rb"); //open WSDL file to copy
	if (!fd)
	{
		return 404; //return HTTP not found error
	}
	soap->http_content = "text/xml";  //HTTP header with text /xml content
	soap_response(soap, SOAP_FILE);
	for (;;)
	{
		size_t r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fd);
		if (!r)
		{
			break;
		}
		if (soap_send_raw(soap, soap->tmpbuf, r))
		{
			break; //cannot send, but little we can do about that
		}
	}
	fclose(fd);
	soap_end_send(soap);
	*/
	LPBYTE pBuf = NULL;
	DWORD dwLen = 0;
	BOOL bRet = LoadRes(
		AfxGetInstanceHandle(),
		MAKEINTRESOURCE(IDR_BIN_WSDL), _T("BIN"), pBuf, dwLen
		);
	if (!bRet)
	{
		return 404;
	}

	soap->http_content = "text/xml";  //HTTP header with text /xml content
	soap_response(soap, SOAP_FILE);
	soap_send_raw(soap, (const char *)pBuf, dwLen);
	soap_end_send(soap);
	free(pBuf);
	return SOAP_OK;
}

CGSoapServerUtils::CGSoapServerUtils(void)
{
}

CGSoapServerUtils::~CGSoapServerUtils(void)
{
}

BOOL CGSoapServerUtils::InitServer(USHORT uPort)
{
	int iRet;
	int s;
	struct soap add_soap;
	soap_init(&add_soap);
	add_soap.fget = http_get;
	//soap_set_namespaces(&add_soap, add_namespaces);
	iRet = soap_bind(&add_soap, NULL, uPort, 100);
	if (iRet < 0)
	{
		soap_print_fault(&add_soap, stderr);
		return FALSE;
	}
	for ( ; ; )
	{
		s = soap_accept(&add_soap);
		if (s < 0)
		{
			soap_print_fault(&add_soap, stderr);
			return FALSE;
		}
		soap_serve(&add_soap);
		soap_end(&add_soap);
	}
	return TRUE;
}

int ns__Syndwsadata(struct soap *add_soap, struct SyndwsadataRequest request, struct SyndwsadataResponse* serviceResponse) {
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	pFrame->OutputMessage(ICON_TYPE_INFO, _T("WebService: 读取机器列表"));
	USES_CONVERSION;
	serviceResponse->responseVersion = request.requestVersion;
	serviceResponse->requestCtxUUID = request.requestCtxUUID;
	serviceResponse->responseCode = "0";
	serviceResponse->responseExtCode = "0";
	serviceResponse->responseMessage = "Message";
	std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
	for (int i = 0; i < (int)vecInfo.size(); i++)
	{
		struct TermInfoPojo term;
		TCHAR szTmp[MAX_PATH] = {0};
		wsprintf(szTmp, _T("%d"), vecInfo[i].dwClientId);
		term.termId = CT2A(szTmp).m_psz;
		CString strName = theApp.GenDisplayName(&vecInfo[i]);
		//LPSTR pHost = CConv::CharToUtf(vecInfo[i].szHostname);
		LPSTR pHost = CConv::CharToUtf(strName);
		term.termName = pHost;
		delete[] pHost;
		term.termIp = CT2A(vecInfo[i].szIPOut);

		DWORD dwGroupId = 0;
		std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.FindGroup(vecInfo[i].dwClientId);
		if (pIter != theConfig.m_vecGroup.end())
		{
			dwGroupId = pIter->dwGroupId;
		}
		CString strTmp;
		strTmp.Format(_T("%d"), dwGroupId);
		term.propId = CT2A(strTmp).m_psz;
		serviceResponse->lstTermInfo.push_back(term);
	}

	std::vector<CAppConfig::GROUP_INFO> vecGroup = theConfig.m_vecGroup;
	CAppConfig::GROUP_INFO defaultGroupInfo;
	defaultGroupInfo.dwGroupId = 0;
	defaultGroupInfo.strName = _T("默认");
	vecGroup.push_back(defaultGroupInfo);
	for (int i = 0; i < (int)vecGroup.size(); i++)
	{
		struct PropInfoPojo prop;
		const CAppConfig::GROUP_INFO& info = vecGroup[i];
		CString strTmp;
		strTmp.Format(_T("%d"), info.dwGroupId);
		prop.propId = CT2A(strTmp).m_psz;
		LPSTR pName = CConv::CharToUtf(info.strName);
		prop.propName = pName;
		delete[] pName;
		serviceResponse->lstPropInfo.push_back(prop);
	}

	return 0;
}

int ns__syndwsaTicketInfo(struct soap *add_soap, struct SyndwsaTicketInfoRequest request, struct SyndwsaTicketInfoResponse* serviceResponse) {
	CMainFrame *pFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
	pFrame->OutputMessage(ICON_TYPE_INFO, _T("WebService: 设置锁定时间"));
	std::string strStart = request.wtPlanStartTime;
	std::string strEnd = request.wtPlanEndTime;
//	std::string strReason = request.reason;
	COleDateTime oleDateStart;
	COleDateTime oleDateEnd;
	bool bParseStart = oleDateStart.ParseDateTime(CA2T(strStart.c_str()).m_psz);
	bool bParseEnd = oleDateEnd.ParseDateTime(CA2T(strEnd.c_str()).m_psz);
	if (bParseStart && bParseEnd)
	{
		for (int i = 0; i < (int)request.lstTermInfo.size(); i++)
		{
			CString strIp = CA2T(request.lstTermInfo[i].termIp.c_str()).m_psz;
			DWORD dwClientId = (DWORD)-1;
			std::vector<ClientInfo_t> resClients = theProtoManager->GetResClient();
			for (int i = 0; i < (int)resClients.size(); i++)
			{
				if (resClients[i].szIPOut == strIp)
				{
					dwClientId = resClients[i].dwClientId;
					break;
				}
			}
			if (dwClientId != (DWORD)-1)
			{
				std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(dwClientId);
				if (pIter != theConfig.m_mapClient.end())
				{
//					pIter->second.uTimeStart = oleDateStart.GetHour() * 3600 + oleDateStart.GetMinute() * 60 + oleDateStart.GetSecond();
//					pIter->second.uTimeEnd = oleDateEnd.GetHour() * 3600 + oleDateEnd.GetMinute() * 60 + oleDateEnd.GetSecond();
					_TIME_RANGE range;
					range.uTimeStart = Utils::GetCurrentTimestamp(oleDateStart);
					range.uTimeEnd = Utils::GetCurrentTimestamp(oleDateEnd);
					pIter->second.vecTimeRange.push_back(range);
//					lstrcpy(pIter->second.szTimeDesc, CA2T(strReason.c_str()).m_psz);
					theConfig.Save();

					std::string strJsonConfig;
					pIter->second.SaveConfig(strJsonConfig);
					CString strConfig = CA2T(strJsonConfig.c_str()).m_psz;
					theProtoManager->InvokeConfigInfo(dwClientId, strConfig);
				}
			}
		}

		serviceResponse->responseVersion = "1.1.1";
		serviceResponse->requestCtxUUID = "UUID";
		serviceResponse->responseCode = "0";
		serviceResponse->responseExtCode = "0";
		serviceResponse->responseMessage = "Message";

		theApp.m_bLockInfoFromServiceUpdated = TRUE;

		return 0;
	}
	return 1;
}
