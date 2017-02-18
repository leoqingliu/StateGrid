// TaskKeyHook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TaskKeyHook.h"

extern HINSTANCE g_hInstance;
HHOOK g_hHookKbdLL = NULL;

LRESULT CALLBACK MyTaskKeyHookLL(int nCode, WPARAM wp, LPARAM lp)
{
	KBDLLHOOKSTRUCT *pkh = (KBDLLHOOKSTRUCT *)lp;
	if (nCode == HC_ACTION)
	{
		BOOL bCtrlKeyDown = GetAsyncKeyState(VK_CONTROL)>>((sizeof(SHORT) * 8) - 1);
		if ((pkh->vkCode == VK_ESCAPE && bCtrlKeyDown) ||				// Ctrl+Esc
			(pkh->vkCode == VK_TAB && pkh->flags & LLKHF_ALTDOWN) ||	// Alt+TAB
			(pkh->vkCode == VK_ESCAPE && pkh->flags & LLKHF_ALTDOWN)||	// Alt+Esc
			(pkh->vkCode == VK_LWIN || pkh->vkCode == VK_RWIN)			// Start Menu
			)				
			{
				// gobble it: go directly to jail, do not pass go
				return 1;
			}
	}
	return CallNextHookEx(g_hHookKbdLL, nCode, wp, lp);
}

TASKKEYHOOK_API BOOL IsTaskKeysDisabled()
{
	return NULL != g_hHookKbdLL;
}

TASKKEYHOOK_API BOOL DisableTaskKeys(BOOL bDisable)
{
	if (bDisable)
	{
		if (NULL == g_hHookKbdLL)
		{
			g_hHookKbdLL = SetWindowsHookEx(WH_KEYBOARD_LL, MyTaskKeyHookLL, g_hInstance, 0);
		}

		return IsTaskKeysDisabled();
	}
	else if (NULL != g_hHookKbdLL)
	{
		BOOL bRet = UnhookWindowsHookEx(g_hHookKbdLL);
		if (bRet)
		{
			g_hHookKbdLL = NULL;
		}
		
		return bRet;
	}
	else
	{
		return TRUE;
	}
}
