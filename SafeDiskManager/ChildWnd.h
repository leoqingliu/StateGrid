#pragma once

#include "../Public/View/ViewEdit.h"
#include "../Public/View/ViewListCtrl.h"
#include "DialogScreen.h"
#include "DialogControl.h"
#include "DialogLock.h"
#include "DialogBlackWhiteList.h"
#include "DialogControlDev.h"
#include "DialogControlStorage.h"
#include "DialogControlSystem.h"
#include "FileManCtrl.h"
#include "CustomHeightMFCTabCtrl.h"
#include "ControlPanelCtrl.h"
#include "HostInfoView.h"
#include "DialogNetwork.h"
#include "DialogProgram.h"

enum ID_TAB_ITEM
{
	ID_ITEM_FILE = 2,
	ID_ITEM_CMD,
	ID_ITEM_VERSION,
	ID_ITEM_PASSWORD,
	ID_ITEM_SCREEN,
	ID_ITEM_PROCESS,
	ID_ITEM_SERVICE,
//	ID_ITEM_KEYBOARD,
	ID_ITEM_NETWORK,
	ID_ITEM_CONTROL,
	ID_ITEM_CONTROL_PANEL,
	ID_ITEM_HOSTINFO
};

class CChildWnd : public CDockablePane
{
public:
	CChildWnd(void);
	~CChildWnd(void);

protected:
	// Attributes
	CCustomHeightMFCTabCtrl	m_wndTabs;

public:
	CFileManCtrl			m_wndFile;
	CViewRichEdit			m_wndCmd;
	CViewListCtrl			m_wndVersion;
	//Utils::CViewListCtrl	m_wndPassword;
	CDialogScreen			m_wndScreen;
	CDialogBlackWhiteList	m_wndBlackWhiteList;
	CViewListCtrl			m_wndProcess;
	CViewListCtrl			m_wndService;
	CViewListCtrl			m_wndKeyboard;
	CViewListCtrl			m_wndNetwork;
	CDialogControl			m_wndControl;
	CDialogLock				m_wndLock;
	CDialogControlDev		m_wndControlDev;
	CDialogControlStorage	m_wndControlStorage;
	CDialogControlSystem	m_wndControlSystem;
	CControlPanelCtrl		m_wndControlPanel;
	CHostInfoView*			m_wndHostInfo;
	CDialogNetwork			m_wndNetworkRate;
	CDialogProgram			m_wndProgram;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//		
	virtual BOOL CanBeClosed() const;
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	virtual CTabbedPane* CreateTabbedPane();

public:
	void ActiveTab(int iIndex);
	void Close();
	afx_msg void OnClose();
};
