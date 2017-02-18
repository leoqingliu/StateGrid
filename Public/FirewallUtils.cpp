#include "StdAfx.h"
#include "FirewallUtils.h"
#include "crypt.h"
#include "Debug.h"
#include <IPHlpApi.h>
#pragma comment(lib, "IPHlpApi.lib")

#define LOG_POST_KEY		"AdrT2DXyAZ8ybug1JYLW2iTFkjbR6nH4fEOuPxqcF5ye2gjxvR5cGh068OK3czN7"

FirewallUtils::FirewallUtils(void)
{
}

FirewallUtils::~FirewallUtils(void)
{
}

#include <netfw.h>
HRESULT WindowsFirewallInitialize(IN NET_FW_PROFILE_TYPE eProfileType, OUT INetFwProfile** fwProfile)
{
	HRESULT hr = S_OK;
	INetFwMgr* fwMgr = NULL;
	INetFwPolicy* fwPolicy = NULL;

	_ASSERT(fwProfile != NULL);

	*fwProfile = NULL;

	// Create an instance of the firewall settings manager.
	hr = CoCreateInstance(
		__uuidof(NetFwMgr),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(INetFwMgr),
		(void**)&fwMgr
		);
	if (FAILED(hr))
	{
		OutputLog(_T("CoCreateInstance failed: 0x%08lx"), hr);
		goto error;
	}

	// Retrieve the local firewall policy.
	hr = fwMgr->get_LocalPolicy(&fwPolicy);
	if (FAILED(hr))
	{
		OutputLog(_T("get_LocalPolicy failed: 0x%08lx"), hr);
		goto error;
	}

	// Retrieve the firewall profile currently in effect.
	//hr = fwPolicy->get_CurrentProfile(fwProfile);
	hr = fwPolicy->GetProfileByType(eProfileType, fwProfile);
	if (FAILED(hr))
	{
		OutputLog(_T("get_CurrentProfile failed: 0x%08lx"), hr);
		goto error;
	}

error:

	// Release the local firewall policy.
	if (fwPolicy != NULL)
	{
		fwPolicy->Release();
	}

	// Release the firewall settings manager.
	if (fwMgr != NULL)
	{
		fwMgr->Release();
	}

	return hr;
}

void WindowsFirewallCleanup(IN INetFwProfile* fwProfile)
{
	// Release the firewall profile.
	if (fwProfile != NULL)
	{
		fwProfile->Release();
	}
}

HRESULT WindowsFirewallPortIsEnabled(
									 IN INetFwProfile* fwProfile,
									 IN LONG portNumber,
									 IN NET_FW_IP_PROTOCOL ipProtocol,
									 OUT BOOL* fwPortEnabled
									 )
{
	HRESULT hr = S_OK;
	VARIANT_BOOL fwEnabled;
	INetFwOpenPort* fwOpenPort = NULL;
	INetFwOpenPorts* fwOpenPorts = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwPortEnabled != NULL);

	*fwPortEnabled = FALSE;

	// Retrieve the globally open ports collection.
	hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
	if (FAILED(hr))
	{
		OutputLog(_T("get_GloballyOpenPorts failed: 0x%08lx"), hr);
		goto error;
	}

	// Attempt to retrieve the globally open port.
	hr = fwOpenPorts->Item(portNumber, ipProtocol, &fwOpenPort);
	if (SUCCEEDED(hr))
	{
		// Find out if the globally open port is enabled.
		hr = fwOpenPort->get_Enabled(&fwEnabled);
		if (FAILED(hr))
		{
			OutputLog(_T("get_Enabled failed: 0x%08lx"), hr);
			goto error;
		}

		if (fwEnabled != VARIANT_FALSE)
		{
			// The globally open port is enabled.
			*fwPortEnabled = TRUE;

			OutputLog(_T("Port %ld is open in the firewall."), portNumber);
		}
		else
		{
			OutputLog(_T("Port %ld is not open in the firewall."), portNumber);
		}
	}
	else
	{
		// The globally open port was not in the collection.
		hr = S_OK;

		OutputLog(_T("Port %ld is not open in the firewall."), portNumber);
	}

error:

	// Release the globally open port.
	if (fwOpenPort != NULL)
	{
		fwOpenPort->Release();
	}

	// Release the globally open ports collection.
	if (fwOpenPorts != NULL)
	{
		fwOpenPorts->Release();
	}

	return hr;
}

HRESULT WindowsFirewallPortAdd(
							   IN INetFwProfile* fwProfile,
							   IN LONG portNumber,
							   IN NET_FW_IP_PROTOCOL ipProtocol,
							   IN const wchar_t* name
							   )
{
	HRESULT hr = S_OK;
	BOOL fwPortEnabled;
	BSTR fwBstrName = NULL;
	INetFwOpenPort* fwOpenPort = NULL;
	INetFwOpenPorts* fwOpenPorts = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(name != NULL);

	// First check to see if the port is already added.
	hr = WindowsFirewallPortIsEnabled(
		fwProfile,
		portNumber,
		ipProtocol,
		&fwPortEnabled
		);
	if (FAILED(hr))
	{
		OutputLog(_T("WindowsFirewallPortIsEnabled failed: 0x%08lx"), hr);
		goto error;
	}

	// Only add the port if it isn't already added.
	if (!fwPortEnabled)
	{
		// Retrieve the collection of globally open ports.
		hr = fwProfile->get_GloballyOpenPorts(&fwOpenPorts);
		if (FAILED(hr))
		{
			OutputLog(_T("get_GloballyOpenPorts failed: 0x%08lx"), hr);
			goto error;
		}

		// Create an instance of an open port.
		hr = CoCreateInstance(
			__uuidof(NetFwOpenPort),
			NULL,
			CLSCTX_INPROC_SERVER,
			__uuidof(INetFwOpenPort),
			(void**)&fwOpenPort
			);
		if (FAILED(hr))
		{
			OutputLog(_T("CoCreateInstance failed: 0x%08lx"), hr);
			goto error;
		}

		// Set the port number.
		hr = fwOpenPort->put_Port(portNumber);
		if (FAILED(hr))
		{
			OutputLog(_T("put_Port failed: 0x%08lx"), hr);
			goto error;
		}

		// Set the IP protocol.
		hr = fwOpenPort->put_Protocol(ipProtocol);
		if (FAILED(hr))
		{
			OutputLog(_T("put_Protocol failed: 0x%08lx"), hr);
			goto error;
		}

		// Allocate a BSTR for the friendly name of the port.
		fwBstrName = SysAllocString(name);
		if (SysStringLen(fwBstrName) == 0)
		{
			hr = E_OUTOFMEMORY;
			OutputLog(_T("SysAllocString failed: 0x%08lx"), hr);
			goto error;
		}

		// Set the friendly name of the port.
		hr = fwOpenPort->put_Name(fwBstrName);
		if (FAILED(hr))
		{
			OutputLog(_T("put_Name failed: 0x%08lx"), hr);
			goto error;
		}

		// Opens the port and adds it to the collection.
		hr = fwOpenPorts->Add(fwOpenPort);
		if (FAILED(hr))
		{
			OutputLog(_T("Add failed: 0x%08lx"), hr);
			goto error;
		}

		OutputLog(_T("Port %ld is now open in the firewall."), portNumber);
	}

error:

	// Free the BSTR.
	SysFreeString(fwBstrName);

	// Release the open port instance.
	if (fwOpenPort != NULL)
	{
		fwOpenPort->Release();
	}

	// Release the globally open ports collection.
	if (fwOpenPorts != NULL)
	{
		fwOpenPorts->Release();
	}

	return hr;
}

HRESULT WindowsFirewallIsOn(IN INetFwProfile* fwProfile, OUT BOOL* fwOn)
{
	HRESULT hr = S_OK;
	VARIANT_BOOL fwEnabled;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwOn != NULL);

	*fwOn = FALSE;

	// Get the current state of the firewall.
	hr = fwProfile->get_FirewallEnabled(&fwEnabled);
	if (FAILED(hr))
	{
		OutputLog(_T("get_FirewallEnabled failed: 0x%08lx"), hr);
		goto error;
	}

	// Check to see if the firewall is on.
	if (fwEnabled != VARIANT_FALSE)
	{
		*fwOn = TRUE;
		OutputLog(_T("The firewall is on."));
	}
	else
	{
		OutputLog(_T("The firewall is off."));
	}

error:

	return hr;
}

HRESULT WindowsFirewallTurnOff(IN INetFwProfile* fwProfile)
{
	HRESULT hr = S_OK;
	BOOL fwOn;

	_ASSERT(fwProfile != NULL);

	// Check to see if the firewall is on.
	hr = WindowsFirewallIsOn(fwProfile, &fwOn);
	if (FAILED(hr))
	{
		OutputLog(_T("WindowsFirewallIsOn failed: 0x%08lx"), hr);
		goto error;
	}

	// If it is, turn it off.
	if (fwOn)
	{
		// Turn the firewall off.
		hr = fwProfile->put_FirewallEnabled(VARIANT_FALSE);
		if (FAILED(hr))
		{
			OutputLog(_T("put_FirewallEnabled failed: 0x%08lx"), hr);
			goto error;
		}

		OutputLog(_T("The firewall is now off."));
	}

error:

	return hr;
}

HRESULT WindowsFirewallTurnOn(IN INetFwProfile* fwProfile)
{
	HRESULT hr = S_OK;
	BOOL fwOn;

	_ASSERT(fwProfile != NULL);

	// Check to see if the firewall is off.
	hr = WindowsFirewallIsOn(fwProfile, &fwOn);
	if (FAILED(hr))
	{
		OutputLog(_T("WindowsFirewallIsOn failed: 0x%08lx"), hr);
		goto error;
	}

	// If it is, turn it on.
	if (!fwOn)
	{
		// Turn the firewall on.
		hr = fwProfile->put_FirewallEnabled(VARIANT_TRUE);
		if (FAILED(hr))
		{
			OutputLog(_T("put_FirewallEnabled failed: 0x%08lx"), hr);
			goto error;
		}

		OutputLog(_T("The firewall is now on."));
	}

error:

	return hr;
}

HRESULT WindowsFirewallAppIsEnabled(
									IN INetFwProfile* fwProfile,
									IN const wchar_t* fwProcessImageFileName,
									OUT BOOL* fwAppEnabled
									)
{
	HRESULT hr = S_OK;
	BSTR fwBstrProcessImageFileName = NULL;
	VARIANT_BOOL fwEnabled;
	INetFwAuthorizedApplication* fwApp = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwProcessImageFileName != NULL);
	_ASSERT(fwAppEnabled != NULL);

	*fwAppEnabled = FALSE;

	// Retrieve the authorized application collection.
	hr = fwProfile->get_AuthorizedApplications(&fwApps);
	if (FAILED(hr))
	{
		OutputLog(_T("get_AuthorizedApplications failed: 0x%08lx"), hr);
		goto error;
	}

	// Allocate a BSTR for the process image file name.
	fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
	if (fwBstrProcessImageFileName == NULL)
	{
		hr = E_OUTOFMEMORY;
		OutputLog(_T("SysAllocString failed: 0x%08lx"), hr);
		goto error;
	}

	// Attempt to retrieve the authorized application.
	hr = fwApps->Item(fwBstrProcessImageFileName, &fwApp);
	if (SUCCEEDED(hr))
	{
		// Find out if the authorized application is enabled.
		hr = fwApp->get_Enabled(&fwEnabled);
		if (FAILED(hr))
		{
			OutputLog(_T("get_Enabled failed: 0x%08lx"), hr);
			goto error;
		}

		if (fwEnabled != VARIANT_FALSE)
		{
			// The authorized application is enabled.
			*fwAppEnabled = TRUE;

			OutputLog(
				_T("Authorized application %lS is enabled in the firewall."),
				fwProcessImageFileName
				);
		}
		else
		{
			OutputLog(
				_T("Authorized application %lS is disabled in the firewall."),
				fwProcessImageFileName
				);
		}
	}
	else
	{
		// The authorized application was not in the collection.
		hr = S_OK;

		printf(
			"Authorized application %lS is disabled in the firewall.\n",
			fwProcessImageFileName
			);
	}

error:

	// Free the BSTR.
	SysFreeString(fwBstrProcessImageFileName);

	// Release the authorized application instance.
	if (fwApp != NULL)
	{
		fwApp->Release();
	}

	// Release the authorized application collection.
	if (fwApps != NULL)
	{
		fwApps->Release();
	}

	return hr;
}

HRESULT WindowsFirewallAddApp(
							  IN INetFwProfile* fwProfile,
							  IN const wchar_t* fwProcessImageFileName,
							  IN const wchar_t* fwName
							  )
{
	HRESULT hr = S_OK;
	BOOL fwAppEnabled;
	BSTR fwBstrName = NULL;
	BSTR fwBstrProcessImageFileName = NULL;
	INetFwAuthorizedApplication* fwApp = NULL;
	INetFwAuthorizedApplications* fwApps = NULL;

	_ASSERT(fwProfile != NULL);
	_ASSERT(fwProcessImageFileName != NULL);
	_ASSERT(fwName != NULL);

	// First check to see if the application is already authorized.
	hr = WindowsFirewallAppIsEnabled(
		fwProfile,
		fwProcessImageFileName,
		&fwAppEnabled
		);
	if (FAILED(hr))
	{
		OutputLog(_T("WindowsFirewallAppIsEnabled failed: 0x%08lx"), hr);
		goto error;
	}

	// Only add the application if it isn't already authorized.
	if (!fwAppEnabled)
	{
		// Retrieve the authorized application collection.
		hr = fwProfile->get_AuthorizedApplications(&fwApps);
		if (FAILED(hr))
		{
			OutputLog(_T("get_AuthorizedApplications failed: 0x%08lx"), hr);
			goto error;
		}

		// Create an instance of an authorized application.
		hr = CoCreateInstance(
			__uuidof(NetFwAuthorizedApplication),
			NULL,
			CLSCTX_INPROC_SERVER,
			__uuidof(INetFwAuthorizedApplication),
			(void**)&fwApp
			);
		if (FAILED(hr))
		{
			OutputLog(_T("CoCreateInstance failed: 0x%08lx"), hr);
			goto error;
		}

		// Allocate a BSTR for the process image file name.
		fwBstrProcessImageFileName = SysAllocString(fwProcessImageFileName);
		if (fwBstrProcessImageFileName == NULL)
		{
			hr = E_OUTOFMEMORY;
			OutputLog(_T("SysAllocString failed: 0x%08lx"), hr);
			goto error;
		}

		// Set the process image file name.
		hr = fwApp->put_ProcessImageFileName(fwBstrProcessImageFileName);
		if (FAILED(hr))
		{
			OutputLog(_T("put_ProcessImageFileName failed: 0x%08lx"), hr);
			goto error;
		}

		// Allocate a BSTR for the application friendly name.
		fwBstrName = SysAllocString(fwName);
		if (SysStringLen(fwBstrName) == 0)
		{
			hr = E_OUTOFMEMORY;
			OutputLog(_T("SysAllocString failed: 0x%08lx"), hr);
			goto error;
		}

		// Set the application friendly name.
		hr = fwApp->put_Name(fwBstrName);
		if (FAILED(hr))
		{
			OutputLog(_T("put_Name failed: 0x%08lx"), hr);
			goto error;
		}

		// Add the application to the collection.
		hr = fwApps->Add(fwApp);
		if (FAILED(hr))
		{
			OutputLog(_T("Add failed: 0x%08lx"), hr);
			goto error;
		}

		OutputLog(
			_T("Authorized application %lS is now enabled in the firewall."),
			fwProcessImageFileName
			);
	}

error:

	// Free the BSTRs.
	SysFreeString(fwBstrName);
	SysFreeString(fwBstrProcessImageFileName);

	// Release the authorized application instance.
	if (fwApp != NULL)
	{
		fwApp->Release();
	}

	// Release the authorized application collection.
	if (fwApps != NULL)
	{
		fwApps->Release();
	}

	return hr;
}

BOOL FirewallUtils::AddPort(UINT uPort, LPCTSTR pName)
{
	HRESULT comInit = E_FAIL;
	HRESULT hr = S_OK;
	INetFwProfile* fwProfile = NULL;
	BOOL bRet = FALSE;

	USES_CONVERSION;

	// Initialize COM.
	comInit = CoInitializeEx(
		0,
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
		);

	// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
	// initialized with a different mode. Since we don't care what the mode is,
	// we'll just use the existing mode.
	if (comInit != RPC_E_CHANGED_MODE)
	{
		hr = comInit;
		if (FAILED(hr))
		{
			goto error;
		}
	}

	/*
	// Turn off the firewall.
	hr = WindowsFirewallTurnOff(fwProfile);
	if (FAILED(hr))
	{
		DebugPrint("WindowsFirewallTurnOff failed: 0x%08lx\n", hr);
		goto error;
	}
	
	// Turn on the firewall.
	hr = WindowsFirewallTurnOn(fwProfile);
	if (FAILED(hr))
	{
		DebugPrint("WindowsFirewallTurnOn failed: 0x%08lx\n", hr);
		goto error;
	}
	*/

	NET_FW_PROFILE_TYPE eType[] = {NET_FW_PROFILE_DOMAIN, NET_FW_PROFILE_STANDARD, NET_FW_PROFILE_CURRENT};

	for (int i = 0; i < _countof(eType); i++)
	{
		// Retrieve the firewall profile currently in effect.
		hr = WindowsFirewallInitialize(eType[i], &fwProfile);
		if (FAILED(hr))
		{
			// Release the firewall profile.
			WindowsFirewallCleanup(fwProfile);

			goto error;
		}

		// Add TCP to list of globally open ports.
		hr = WindowsFirewallPortAdd(fwProfile, uPort, NET_FW_IP_PROTOCOL_TCP, CT2W(pName).m_psz);
		if (FAILED(hr))
		{
			// Release the firewall profile.
			WindowsFirewallCleanup(fwProfile);

			OutputLog(_T("WindowsFirewallPortAdd failed: 0x%08lx"), hr);
			goto error;
		}

		// Release the firewall profile.
		WindowsFirewallCleanup(fwProfile);
	}

	bRet = TRUE;

error:

	// Uninitialize COM.
	if (SUCCEEDED(comInit))
	{
		CoUninitialize();
	}

	return bRet;
}

/*
BOOL FirewallUtils::IsPortExist( UINT uPort )
{
	HRESULT comInit = E_FAIL;
	HRESULT hr = S_OK;
	INetFwProfile* fwProfile = NULL;
	BOOL bRet = FALSE;

	USES_CONVERSION;

	// Initialize COM.
	comInit = CoInitializeEx(
		0,
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
		);

	// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
	// initialized with a different mode. Since we don't care what the mode is,
	// we'll just use the existing mode.
	if (comInit != RPC_E_CHANGED_MODE)
	{
		hr = comInit;
		if (FAILED(hr))
		{
			goto error;
		}
	}

	NET_FW_PROFILE_TYPE eType[] = {NET_FW_PROFILE_DOMAIN, NET_FW_PROFILE_STANDARD, NET_FW_PROFILE_CURRENT};

	for (int i = 0; i < _countof(eType); i++)
	{
		// Retrieve the firewall profile currently in effect.
		hr = WindowsFirewallInitialize(eType[i], &fwProfile);
		if (FAILED(hr))
		{
			// Release the firewall profile.
			WindowsFirewallCleanup(fwProfile);

			goto error;
		}

		// First check to see if the port is already added.
		BOOL fwPortEnabled;
		hr = WindowsFirewallPortIsEnabled(
			fwProfile,
			uPort,
			NET_FW_IP_PROTOCOL_TCP,
			&fwPortEnabled
			);
		if (FAILED(hr))
		{
			// Release the firewall profile.
			WindowsFirewallCleanup(fwProfile);

			OutputLog(_T("WindowsFirewallPortAdd failed: 0x%08lx"), hr);
			goto error;
		}

		// Release the firewall profile.
		WindowsFirewallCleanup(fwProfile);

		if (FALSE == fwPortEnabled)
		{
			goto error;
		}
	}

	bRet = TRUE;

error:

	// Uninitialize COM.
	if (SUCCEEDED(comInit))
	{
		CoUninitialize();
	}

	return bRet;
}
*/

BOOL FirewallUtils::AddApp(LPCTSTR pPath, LPCTSTR pName)
{
	HRESULT comInit = E_FAIL;
	HRESULT hr = S_OK;
	INetFwProfile* fwProfile = NULL;
	BOOL bRet = FALSE;

	USES_CONVERSION;

	// Initialize COM.
	comInit = CoInitializeEx(
		0,
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
		);

	// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
	// initialized with a different mode. Since we don't care what the mode is,
	// we'll just use the existing mode.
	if (comInit != RPC_E_CHANGED_MODE)
	{
		hr = comInit;
		if (FAILED(hr))
		{
			goto error;
		}
	}

	NET_FW_PROFILE_TYPE eType[] = {NET_FW_PROFILE_DOMAIN, NET_FW_PROFILE_STANDARD, NET_FW_PROFILE_CURRENT};
	for (int i = 0; i < _countof(eType); i++)
	{
		// Retrieve the firewall profile currently in effect.
		hr = WindowsFirewallInitialize(eType[i], &fwProfile);
		if (FAILED(hr))
		{
			OutputLog(_T("WindowsFirewallInitialize failed: 0x%08lx"), hr);

			// Release the firewall profile.
			WindowsFirewallCleanup(fwProfile);

			goto error;
		}

		// Add Windows Messenger to the authorized application collection.
		hr = WindowsFirewallAddApp(
			fwProfile,
			CT2W(pPath).m_psz,
			CT2W(pName).m_psz
			);
		if (FAILED(hr))
		{
			OutputLog(_T("WindowsFirewallAddApp failed: 0x%08lx"), hr);

			// Release the firewall profile.
			WindowsFirewallCleanup(fwProfile);

			goto error;
		}
	}

	bRet = TRUE;

error:

	// Release the firewall profile.
	WindowsFirewallCleanup(fwProfile);

	// Uninitialize COM.
	if (SUCCEEDED(comInit))
	{
		CoUninitialize();
	}

	return bRet;
}
