// WMIUtils.h: interface for the CWMIUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WMIUTILS_H__21EE3D69_B9AD_453C_A9DC_5BE40801A091__INCLUDED_)
#define AFX_WMIUTILS_H__21EE3D69_B9AD_453C_A9DC_5BE40801A091__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRSAFE_NO_DEPRECATE
#include <strsafe.h>
#include <objbase.h>
#include <WbemCli.h>
#pragma comment(lib, "Wbemuuid.lib")
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "Ws2_32.lib")

class CWMIUtils  
{
public:
	CWMIUtils();
	virtual ~CWMIUtils();

public:
	BOOL GetDiskId(CString& strId);
	BOOL GetBiosId(CString& strId);

protected:
	HRESULT Init();
	VOID UnInit();

	IWbemServices *m_pSvc;
};

#endif // !defined(AFX_WMIUTILS_H__21EE3D69_B9AD_453C_A9DC_5BE40801A091__INCLUDED_)
