// MFCDesktopAlertNoCloseWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenLocker.h"
#include "MFCDesktopAlertNoCloseWnd.h"


// CMFCDesktopAlertNoCloseWnd

IMPLEMENT_DYNAMIC(CMFCDesktopAlertNoCloseWnd, CMFCDesktopAlertWnd)

CMFCDesktopAlertNoCloseWnd::CMFCDesktopAlertNoCloseWnd()
:CMFCDesktopAlertWnd()
{
	m_bHasCloseButton = FALSE;
}

CMFCDesktopAlertNoCloseWnd::~CMFCDesktopAlertNoCloseWnd()
{
}


BEGIN_MESSAGE_MAP(CMFCDesktopAlertNoCloseWnd, CMFCDesktopAlertWnd)
END_MESSAGE_MAP()


// CMFCDesktopAlertNoCloseWnd message handlers


