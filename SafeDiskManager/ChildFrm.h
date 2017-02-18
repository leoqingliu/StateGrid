
// ChildFrm.h : CChildFrame 类的接口
//

#pragma once

#include "ChildWnd.h"
#include "../Public/View/ViewEdit.h"
#include <vector>
using namespace std;

class CMainFrame;
class CSafeDiskManagerView;
struct Packet_t;
class CChildFrame : public CMDIChildWndEx, public CViewRichEditCallBack
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// 属性
public:

// 操作
public:

// 重写
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL CanShowOnMDITabs();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();

protected:
	CChildWnd m_wndChild;

protected:
	DWORD m_dwClientId;

public:
	DWORD GetId()
	{
		return m_dwClientId;
	}
	VOID Initialize(DWORD dwId, CString strPassword, UINT uTime, DWORD dwIdleTime, BOOL bWhiteMode);
	CMainFrame *GetMainFrame()
	{
		return (CMainFrame*)AfxGetApp()->GetMainWnd();
	}

public:
	
	// Dir
	struct ITEMINFO
	{
		BOOL bDirectory;
		DWORD nFileSizeLow;
		DWORD nFileSizeHigh;
		FILETIME ftCreationTime;
		CString strItem;
	};
	vector<ITEMINFO>	m_vecItemInfo;
	//CSafeDiskManagerView* GetFileView();
	//CListCtrl& GetFileList();
	VOID OnDir(Packet_t &Packet);
	void OnFileViewDblClick(INT iClickIndex);
	void OnFileViewSelChanged(int iClickIndex);
	INT				m_iRemoteSelLevel;
	CString			m_szCurDir;

	// Cmd
	LONG m_dwCursor;
	VOID OnCmd(Packet_t &Packet);
	virtual void OnEnMsgfilter(NMHDR *pNMHDR, LRESULT *pResult);

	// Version
	VOID OnVersion(Packet_t &Packet);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnUpdateChildRefresh(CCmdUI *pCmdUI);
	afx_msg void OnChildRefresh();

	// Password
	VOID OnPassword(Packet_t &Packet);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnChildDownload();
	afx_msg void OnUpdateChildDownload(CCmdUI *pCmdUI);
	afx_msg void OnChildUpload();
	afx_msg void OnUpdateChildUpload(CCmdUI *pCmdUI);

	// Screen
	VOID CaptureScreen();
	VOID OnControlSetScreenTime(UINT uTime);
	VOID ScreenRealtimeView();
	VOID ScreenRealtimeControl();
	BOOL m_bAutoScreenTimerStarted;

	BOOL SaveListCtrl(CMFCListCtrl &List, CString strFileName);
	VOID OnSave(DWORD dwCtrlId);
	VOID OnUpdate(DWORD dwCtrlId);
	VOID OnKillProcess(DWORD dwCtrlId);

	// Process
	VOID OnProcess(Packet_t &Packet);

	// Service
	VOID OnService(Packet_t &Packet);

	// Keyboard
	VOID OnKeyboard(Packet_t &Packet);

	// Network
	VOID OnNetwork(Packet_t &Packet);
	
	VOID OnKillProcessResponse(Packet_t &Packet);

	// Shutdown
	VOID OnControlShutdown();

	// Restart
	VOID OnControlRestart();

	// Logoff
	VOID OnControlLogoff();

	// Lock
	VOID OnControlLock();
	VOID OnControlUnLock();
	VOID OnControlLockAuto();
	void OnControlSetPassword(CString strPassword, DWORD dwIdleTime);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	VOID OnControlSetBlackWhiteList(
		std::vector<CString> vecClass, std::vector<CString> vecWin,
		std::vector<CString> vecWhiteClass, std::vector<CString> vecWhiteWin,
		BOOL bWhiteMode);

	VOID OnControlDev(const CONFIG_INFO& config);
	VOID OnControlStorage(const CONFIG_INFO& config);
	VOID OnControlSystem(const CONFIG_INFO& config);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	void ActiveTab(int iIndex);

	void OnOnlineChanged();
	void OnLockChanged();
};
