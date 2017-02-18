
#pragma once

#include "ViewTree.h"
#include "../Public/View/TreeCtrlEx.h"

#define IMAGE_ROOT				0
#define IMAGE_CONNECTED			1
#define IMAGE_DISCONNECTED		2
#define IMAGE_CONNECTED_LOCKED	3

#if 0
class CConnectViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};
#endif

class CConnectView : public CDockablePane
{
// 构造
public:
	CConnectView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// 属性
protected:

	//CViewTree m_wndConnectView;
	CTreeCtrlEx m_wndConnectView;
	CImageList m_FileConnectImages;
//	CConnectViewToolBar m_wndToolBar;

protected:
	//void FillFileView();

public:
	CTreeCtrlEx& GetTree()
	{
		return m_wndConnectView;
	}

// 实现
public:
	virtual ~CConnectView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnPolicy();
	afx_msg void OnFileOpen();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnUpdateOpen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProperties(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePolicy(CCmdUI *pCmdUI);
	afx_msg void OnRemoteOpen();
	afx_msg void OnUpdateRemoteOpen(CCmdUI *pCmdUI);
	afx_msg void OnBatchLock();
	afx_msg void OnUpdateBatchLock(CCmdUI *pCmdUI);
	afx_msg void OnComment();
	afx_msg void OnUpdateComment(CCmdUI *pCmdUI);
	afx_msg void OnBatchUnlock();
	afx_msg void OnUpdateBatchUnlock(CCmdUI *pCmdUI);
	afx_msg void OnBatchUpgrade();
	afx_msg void OnUpdateBatchUpgrade(CCmdUI *pCmdUI);
	CMFCToolTipCtrl m_ToolTip;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DWORD m_dwLastClientId;
	afx_msg void OnPolicyEdit();
	afx_msg void OnUpdatePolicyEdit(CCmdUI *pCmdUI);
	afx_msg void OnGroupCreate();
	afx_msg void OnUpdateGroupCreate(CCmdUI *pCmdUI);
	afx_msg void OnGroupEdit();
	afx_msg void OnUpdateGroupEdit(CCmdUI *pCmdUI);
	afx_msg void OnGroupDel();
	afx_msg void OnUpdateGroupDel(CCmdUI *pCmdUI);
	afx_msg void OnGroupAddto();
	afx_msg void OnUpdateGroupAddto(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnBatchAuto();
	afx_msg void OnUpdateBatchAuto(CCmdUI *pCmdUI);
};

