#pragma once

namespace NGlobalUtils
{
	class CFileEx
	{
	public:
		CFileEx(void);
		~CFileEx(void);

	public:
		//
		class CFileStruct
		{
		public:
			CFileStruct()
			{
				hFile = NULL;
				hFileMap = NULL;
				pBasePointer = NULL;
			}
			~CFileStruct()
			{
			}

			void Close()
			{
				if(hFile)
				{
					CloseHandle(hFile);
					hFile = NULL;
				}
				if(hFileMap)
				{
					CloseHandle(hFileMap);
					hFileMap = NULL;
				}
				if(pBasePointer)
				{
					UnmapViewOfFile(pBasePointer);
					pBasePointer = NULL;
				}
			}
		public:
			HANDLE hFile;
			HANDLE hFileMap;
			LPVOID pBasePointer; 
		};

		//
		// File Operation Utils
		//
		static BOOL  OpenFile(LPCTSTR lpFilePath, CFileStruct* pFileStruct, 
			BOOL bCanWrite = FALSE, BOOL bCreate = FALSE);
		static BOOL  MapFile(CFileStruct* pFileStruct, BOOL bCanWrite = FALSE);
		static void  CloseFile(CFileStruct* pFileStruct);
		static DWORD GetFileSize(HANDLE hFile, LPDWORD lpFileSizeHigh);
	};
}