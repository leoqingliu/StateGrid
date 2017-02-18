#pragma once

#define MD5HASH_SIZE 16

typedef const BYTE *LPCBYTE;

namespace Crypt
{
	typedef struct
	{      
		BYTE state[256];       
		BYTE x;        
		BYTE y;
	}RC4KEY;
	
	void init(void);
	
	void uninit(void);
	
	bool _md5Hash(LPBYTE output, void *inputData, DWORD dataSize);
#if 0
	DWORD mtRand(void);
	
	DWORD mtRandRange(DWORD minVal, DWORD maxVal);
#endif
	
	DWORD crc32Hash(const void *data, DWORD size);
	
	void _rc4Init(const void *binKey, WORD binKeySize, RC4KEY *key);
	
	void _rc4(void *buffer, DWORD size, RC4KEY *key);
	
	void _rc4Full(const void *binKey, WORD binKeySize, void *buffer, DWORD size);
	
	void _visualEncrypt(void *buffer, DWORD size);
	
	void _visualDecrypt(void *buffer, DWORD size);
	
	LPSTR _base64Encode(LPCBYTE source, SIZE_T sourceSize, SIZE_T *destSize);
	
	LPBYTE _base64Decode(LPCSTR source, SIZE_T sourceSize, SIZE_T *destSize);

#if 0	
	void _generateBinaryData(void *buffer, DWORD size, BYTE minValue, BYTE maxValue, bool extMode);
	
	void _generateRc4Key(Crypt::RC4KEY *key);
#endif

	VOID ContentEnc(LPSTR szPassword, int iSize, CString& strContentOut, LPCSTR pKey);
	VOID ContentDec(const CString& strContent, LPCSTR pKey, LPSTR& szPasswordA, SIZE_T& iSize);
};
