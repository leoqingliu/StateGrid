#include "StdAfx.h"
#include "PEUtility.h"
//#include <atlchecked.h>

#pragma warning(disable:4311)	// pointer truncation from 'LPVOID' to 'DWORD'
#pragma warning(disable:4312)	// conversion from 'DWORD' to 'LPVOID' of greater size

namespace PEUtility
{
#define IsInRange(address, start, length)\
	(\
		(LPSTR)(LPVOID)(address) >= (LPSTR)(LPVOID)(start) && \
		(LPSTR)(LPVOID)(address) <  (LPSTR)(LPVOID)(start) + (length) \
	)

#define adr(rva)\
	(LPVOID)(\
	(LPSTR)( (LPSTR)pBasePointer+ pHeaderSection->PointerToRawData)+\
	(DWORD)(rva) - pHeaderSection->VirtualAddress\
	)

#define RESOURCE_ADDR_PLUS(pPoint, rva)\
	(pPoint = (LPVOID)((LPSTR)pPoint + rva))

#define RESOURCE_ADDR(rva)\
	(LPVOID)(\
	(LPSTR)( (LPSTR)pBasePointer+ pHeaderSection->PointerToRawData)+\
	(DWORD)(rva)\
	)

#define INVALID_STRING "N/A"

	CPEHeader::CPEHeader():
	m_dwStartAddress(0)
	,m_dwLen(0)
	,m_bShoudSkip(FALSE)
	,m_bFindRes(FALSE)
	,m_bFindDir(FALSE)
	,m_bFind(FALSE)
	,m_hFile(NULL)
	,m_hMapping(NULL)
	{
	}

	CPEHeader::~CPEHeader()
	{
		UnInit();
	}

	BOOL CPEHeader::GetResType(INT iID, LPTSTR lpTypes, BOOL bFirstRes)
	{
		if(bFirstRes ) dwResTypes = iID;
		switch(iID)
		{
		case ECURSOR:		lstrcpy(lpTypes, _T("Cursor")); break;
		case EBITMAP:		lstrcpy(lpTypes, _T("Bitmap")); break;
		case EICON:			lstrcpy(lpTypes, _T("Icon")); break;
		case EMENU:			lstrcpy(lpTypes, _T("Menu")); break;
		case EDIALOG:		lstrcpy(lpTypes, _T("Dialog")); break;
		case ESTRING:		lstrcpy(lpTypes, _T("String")); break;
		case EFONTDIR:		lstrcpy(lpTypes, _T("Font Directory")); break;
		case EFONT:			lstrcpy(lpTypes, _T("Font")); break;
		case EACCELERATORS:	lstrcpy(lpTypes, _T("Accelerators")); break;
		case EUNFORMAT:		lstrcpy(lpTypes, _T("UnFormated")); break;
		case EMESSAGETAB:	lstrcpy(lpTypes, _T("Message Table")); break;
		case EGROUPCURSOR:	lstrcpy(lpTypes, _T("Group Cursor")); break;
		case EGROUPICON:	lstrcpy(lpTypes, _T("Group Icon")); break;
		case EVERION:		lstrcpy(lpTypes, _T("Version")); break;
		default:			
			{
				if(bFirstRes ) dwResTypes = EUNKNOW;
				lstrcpy(lpTypes, _T("*UnKown")); 
				break;
			}
		}
		return TRUE;
	}

	BOOL CPEHeader::InitiateHeader( stringx strPath, BOOL bCanWrite, PWCHAR pwszType, UINT uResId )
	{
		lstrcpyW(m_wszType, pwszType);
		m_ResId = uResId;

		DWORD dwCreateFileAccess = GENERIC_READ;
		DWORD dwCreateFileShare  = FILE_SHARE_READ;
		DWORD dwMapProtect = PAGE_READONLY;
		DWORD dwViewAccess = FILE_MAP_READ;
		if(bCanWrite)
		{
			dwCreateFileAccess	= GENERIC_READ | GENERIC_WRITE;
			dwCreateFileShare	= FILE_SHARE_READ | FILE_SHARE_WRITE;
			dwMapProtect		= PAGE_READWRITE | SEC_COMMIT;
			dwViewAccess		= FILE_MAP_READ | FILE_MAP_WRITE;
		}
		
		m_hFile = CreateFile(strPath.c_str(), dwCreateFileAccess, dwCreateFileShare, 
			0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);		//Create the File handle
		if (m_hFile==INVALID_HANDLE_VALUE)	//test File Handle
		{	
//			MessageBox(NULL, _T("1"), _T(""), MB_OK);
			return FALSE; 
		} 
		if (!(m_hMapping = CreateFileMapping(m_hFile, 0, dwMapProtect, 0, 0, 0)))
		{	
			//Create the File Map and test
//			MessageBox(NULL, _T("2"), _T(""), MB_OK);
			CloseHandle(m_hFile);
			CloseHandle(m_hMapping);
			return FALSE;
		}
		if (!(pBasePointer = ::MapViewOfFile(m_hMapping, dwViewAccess, 0, 0, 0)))
		{				
//			MessageBox(NULL, _T("3"), _T(""), MB_OK);
			CloseHandle(m_hFile);
			CloseHandle(m_hMapping);
			return FALSE;
		}
		// Fill DOS Header
		memcpy( &m_ImageDos, (PIMAGE_DOS_HEADER)pBasePointer, sizeof(IMAGE_DOS_HEADER) );
		if(!(m_ImageDos.e_magic == IMAGE_DOS_SIGNATURE))
		{
			CloseHandle(m_hFile);
			CloseHandle(m_hMapping);
			return FALSE;
		}

		// Fill NT Header
		PIMAGE_NT_HEADERS	pHeaderNT = (PIMAGE_NT_HEADERS)((LPSTR)pBasePointer + m_ImageDos.e_lfanew);
		memcpy( &m_ImageNT, pHeaderNT, sizeof(IMAGE_NT_HEADERS) );
		if( m_ImageNT.Signature != IMAGE_NT_SIGNATURE )//判断是否是PE格式文件
		{
			CloseHandle(m_hFile);
			CloseHandle(m_hMapping);
			return FALSE;
		}

		// Process Section 
		pHeaderSection = (PIMAGE_SECTION_HEADER)((LPSTR)pHeaderNT + sizeof(IMAGE_NT_HEADERS));
		for(INT i = 0; i < pHeaderNT->FileHeader.NumberOfSections; i++, pHeaderSection++)
		{
			dwSubSav = pHeaderSection->VirtualAddress - pHeaderSection->PointerToRawData;
			LookForDirectories(
				(LPSTR)pBasePointer + pHeaderSection->PointerToRawData,
				pHeaderSection->VirtualAddress,
				pHeaderSection->SizeOfRawData,
				pHeaderNT->OptionalHeader.DataDirectory
				);
		}
		return TRUE;
	}

	VOID CPEHeader::UnInit()
	{
		if(m_hFile)
		{
			CloseHandle(m_hFile);
			m_hFile = NULL;
		}
		if(m_hMapping)
		{
			CloseHandle(m_hMapping);
			m_hMapping = NULL;
		}
		if(pBasePointer)
		{
			UnmapViewOfFile(pBasePointer);
			pBasePointer = NULL;
		}
	}

	VOID	CPEHeader::LookForDirectories(
		const VOID* const section_data,
		const DWORD section_start_virtual,
		const size_t section_length,
		const IMAGE_DATA_DIRECTORY* const directories
		)
	{
		int iDir;
		for(iDir = 0; iDir < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; iDir++)
		{
			if( directories[iDir].VirtualAddress &&
				IsInRange(directories[iDir].VirtualAddress, section_start_virtual, section_length))
			{
				const void *const stuff_start = (char *)section_data + (directories[iDir].VirtualAddress - section_start_virtual);
				// (virtual address of stuff - virtual address of section) = offset of stuff in section
				const unsigned stuff_length = directories[iDir].Size;

				switch (iDir)
				{
				case IMAGE_DIRECTORY_ENTRY_RESOURCE:
					{
						DumpResourceDir(
							(const IMAGE_RESOURCE_DIRECTORY* const)stuff_start, 
							(const IMAGE_RESOURCE_DIRECTORY* const)stuff_start, 
							TRUE,
							EUNKNOW);
					}
					break;
				default:
					break;
				}
			}
		}
	}
	
	VOID CPEHeader::DumpResourceDir( const IMAGE_RESOURCE_DIRECTORY* const res_start, const IMAGE_RESOURCE_DIRECTORY* const dir, const BOOL output_types, const DWORD dwResType )
	{
		if(IsBadReadPtr(dir, sizeof(*dir)))  return;
		if(m_bShoudSkip)	return;

		INT i;
		IMAGE_RESOURCE_DIRECTORY_ENTRY *single_resource = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(LPVOID)(dir + 1);
		for (i = 0; i < dir->NumberOfNamedEntries + dir->NumberOfIdEntries; i++, single_resource++)
		{
			dwResTypes = EUNKNOW;
			if (single_resource->NameIsString)
			{
				// it's a UNICODE string without a 0-termination - cautious!
				IMAGE_RESOURCE_DIR_STRING_U *uni_name = (PIMAGE_RESOURCE_DIR_STRING_U)(LPVOID)((char *)res_start + single_resource->NameOffset);
				WCHAR szName[128];
				szName[0] = L'\"';
				lstrcpynW(szName + 1, uni_name->NameString, uni_name->Length + 1);
				szName[uni_name->Length + 1] = L'\"';
				szName[uni_name->Length + 2] = 0;

				if(lstrcmpW(szName, m_wszType) == 0)
				{
					m_bFindDir = TRUE;
				}
			}
			else if(output_types)
			{
				//GetResType(single_resource->Id, szName, bFirstRes);
			}
			else
			{
				if(m_bFindDir && single_resource->Id == m_ResId)
				{
					m_bFindRes = TRUE;
				}
			}
			if( single_resource->DataIsDirectory )
			{
				DumpResourceDir(res_start, 
					(PIMAGE_RESOURCE_DIRECTORY)(LPVOID)((char *)res_start + single_resource->OffsetToDirectory), 
					FALSE, dwResTypes);
			}
			else
			{
				if(m_bFindRes)
				{
					m_bShoudSkip = TRUE;
					m_bFind = TRUE;

					IMAGE_RESOURCE_DATA_ENTRY *data = (PIMAGE_RESOURCE_DATA_ENTRY)(LPVOID)((char *)res_start + single_resource->OffsetToData);
					m_dwStartAddress = data->OffsetToData - dwSubSav;
					m_dwLen			 = data->Size;
				}
			}
		}
	}
}
