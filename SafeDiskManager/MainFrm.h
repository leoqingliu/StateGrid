
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "ConnectView.h"
#include "TopTitleView.h"
#include "TopWeatherView.h"
#include "TopNavView.h"
#include "LogWnd.h"
#include "ChildFrm.h"
#include <map>
#include "Packet.h"

#define INDEX_STATUS_PANE_IP			1
#define INDEX_STATUS_PANE_LAST_LOGIN	2
#define INDEX_STATUS_PANE_PATH			3
#define INDEX_STATUS_PANE_SIZE			4

enum ICON_TYPE
{
	ICON_TYPE_INFO = 0,
	ICON_TYPE_ERROR
};

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 属性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // 控件条嵌入成员
	CMFCMenuBar			m_wndMenuBar;
	CMFCStatusBar		m_wndStatusBar;
	CMFCToolBarImages	m_UserImages;
//	CTopWeatherView		m_wndWeatherView;
	CTopNavView			m_wndNavView;

	CConnectView		m_wndConnectView;
	CTopTitleView		m_wndTopTitleView;
	CTreeCtrlEx& GetConnectTree()
	{
		return m_wndConnectView.GetTree();
	}

	HWND m_hWndCurrentChild;

public:
	CLogWnd				m_wndLog;	
	CImageList			m_LogImageList;
	struct OutputMsg
	{
		ICON_TYPE eType;
		CString strMsg;
	};
	CSafeDeque<OutputMsg> m_vecMsg;
	void OutputMessage(ICON_TYPE eIcon, LPCTSTR lpszFormat, ...);

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnStatusBarCommand();
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);

public:
	afx_msg void OnClose();
	afx_msg void OnChildClose();
	virtual BOOL OnShowMDITabContextMenu (CPoint point, DWORD dwAllowedItems, BOOL bDrop);

	VOID OnConnectCommand(INT iCmdId);
	VOID OnConnectUpdate(INT iCmdId, CCmdUI *pCmdUI);
	VOID OnConnectSelChanged(HTREEITEM hItem);

	// MDI
	HWND GetNextMDIChildWnd();
	DWORD GetCountCMDIChildWnds();
	CChildFrame *CreateChildView(DWORD dwClientId, LPCTSTR pTitle);
	int CloseAllChilds();

	CCriticalSection m_Lock;
	std::map<DWORD, CChildFrame*> m_ChildMap;
	VOID Lock()
	{
		m_Lock.Lock();
	}
	VOID UnLock()
	{
		m_Lock.Unlock();
	}
	afx_msg void OnAppConfig();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL CheckIdExistInTree(DWORD dwId, HTREEITEM& hFound);
	HTREEITEM FindGroupTreeItem(CString strName);
	afx_msg void OnAppUserModify();
	afx_msg void OnAppUserManage();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
//	VOID UpdateTopWeather();
	VOID OnChangeActiveNavTab(int iIndex);
	void onDbClickClient(DWORD dwClientId);
	std::vector<DWORD> m_vecClientIdsToRefreshFile;
	CCriticalSection m_vecClientIdsToRefreshFileLock;
	int m_iCurrentTab;
	HTREEITEM m_hTreeDefault;
	afx_msg void OnUpdateViewDefaultAllow(CCmdUI *pCmdUI);
	afx_msg void OnViewDefaultAllow();
};


