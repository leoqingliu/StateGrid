#pragma once

#include <windows.h>

namespace PEUtility
{
	enum EDEPENCYHEADER
	{
		ECOL_ORDINAL = 0,
		ECOL_HINT,
		ECOL_FUNCTION,
		ECOL_ENTRYPOINT
	};

	enum ESECTIONSHEADER
	{
		ESEC_ADDRESS,
		ESEC_VALUE,
		ESEC_DESC
	};

	enum ETREEDATA
	{
		ETREEBLANK = 0,
		ETREEDOSHEADER = 1,
		ETREECOFFHEADER = 2,
		ETREEOPTIONALHEADER = 3,
		ETREESECTIONHEADER = 4,

		ETREEIMPORT = 20,
		ETREEEXPORT = 120,
		ETREERES = 220
	};

	enum RESOURCE_TYPE
	{
		ECURSOR = 1,
		EBITMAP,
		EICON,
		EMENU,
		EDIALOG,
		ESTRING,
		EFONTDIR,
		EFONT,
		EACCELERATORS,
		EUNFORMAT,
		EMESSAGETAB,
		EGROUPCURSOR,
		EGROUPICON = 14,
		EVERION = 16,
		EUNKNOW = 17
	};

	typedef struct _Function
	{
		string		strOrdinal;
		string		strHint;
		string		strFunction;
		string		strEntryPoint;

	}FunctionDescriper;
	
	//////////////////////////////////////////////////////////////////////////

	typedef struct _SectionItem
	{
		string		strAddress;
		string		strValue;
		string		strDesc;

	}SectionItem;

	typedef struct _Section
	{
		vector<_SectionItem>	vecSectionItem;

	}Section;

	//////////////////////////////////////////////////////////////////////////

	typedef struct _PEFuncDescriper
	{
		string						strPEPath;
		vector<FunctionDescriper>	vecFunction;
	}PEFuncDescriper;

	//
	//	Struct For Resource
	//
	typedef struct _ResourceTreeData
	{
		DWORD dwType;
		DWORD dwSize;
		DWORD dwBase;
		DWORD dwCodePage;
	}ResourceTreeData;

	//
	//	Utility For PE Header
	//
	class CPEHeader
	{
	public:
		CPEHeader();
		~CPEHeader();
		BOOL InitiateHeader(stringx strPath, BOOL bCanWrite, PWCHAR pwszType, UINT uResId);
		VOID UnInit();
	
	private:	
		BOOL	GetResType(INT iID, LPTSTR lpTypes, BOOL bFirstRes);
		VOID	LookForDirectories(
			const VOID* const section_data,
			const DWORD section_start_virtual,
			const size_t section_length,
			const IMAGE_DATA_DIRECTORY* const directories
			);
		VOID DumpResourceDir(const IMAGE_RESOURCE_DIRECTORY* const res_start, const IMAGE_RESOURCE_DIRECTORY* const dir, const BOOL output_types, const DWORD dwResType);

	private:
		IMAGE_DOS_HEADER				m_ImageDos;
		IMAGE_NT_HEADERS				m_ImageNT;
	public:	
		//pointer for the mapping file begin
		LPVOID							pBasePointer;				
	private:
		PIMAGE_SECTION_HEADER			pHeaderSection;
		//
		HANDLE m_hFile;						//handle for the file detecting 
		HANDLE m_hMapping;					//handle for the mapping file detecting
		// For Resource Address
		DWORD dwSubSav; 
		// For Resource Type
		//vector<DWORD>	vecResTypes;
		DWORD			dwResTypes;

		//
		BOOL			m_bShoudSkip;
		//
		BOOL			m_bFindRes;
		//
		BOOL			m_bFindDir;

		WCHAR			m_wszType[128];
		UINT			m_ResId;
	public:
		//
		BOOL			m_bFind;
		//
		DWORD			m_dwStartAddress;
		//
		DWORD			m_dwLen;
	};


}
