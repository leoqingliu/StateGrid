// CustomMFCTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "CustomHeightMFCTabCtrl.h"


// CCustomMFCTabCtrl

IMPLEMENT_DYNAMIC(CCustomHeightMFCTabCtrl, CMFCTabCtrl)

CCustomHeightMFCTabCtrl::CCustomHeightMFCTabCtrl(int iHeight)
{
	m_iHeight = iHeight;
}

CCustomHeightMFCTabCtrl::~CCustomHeightMFCTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CCustomHeightMFCTabCtrl, CMFCTabCtrl)
END_MESSAGE_MAP()

void CCustomHeightMFCTabCtrl::SetTabsHeight()
{
	//__super::SetTabsHeight();
	m_nTabsHeight = m_iHeight;
	m_nTabBorderSize = 0;
}

void CCustomHeightMFCTabCtrl::ResetHeight()
{
	SetTabsHeight();
}

// CCustomMFCTabCtrl message handlers
