#pragma once


// CViewListCtrl

class CViewListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CViewListCtrl)

public:
	CViewListCtrl();
	virtual ~CViewListCtrl();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	DECLARE_MESSAGE_MAP()
public:
	void AutoScroll();
	void AdjustColumnWidth();
	int FindItem(LPCTSTR lpBuffer, int iColumn);
	int FindItem(int iCode, int iColumn);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	afx_msg void OnChildSave();
	afx_msg void OnChildUpdate();
	afx_msg void OnChildKillProcesss();

public:
	enum CONTEXT_MENU
	{
		CONTEXT_MENU_NORMAL = 0,
		CONTEXT_MENU_PROCESS,

		CONTEXT_MENU_MAX
	};
	void SetContextMenuType(CONTEXT_MENU menu);

protected:
	CONTEXT_MENU m_ContextMenuType;
};
