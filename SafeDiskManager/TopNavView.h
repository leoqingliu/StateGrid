#pragma once

#include "DialogControlDev.h"
#include "CustomHeightMFCTabCtrl.h"
// CTopNavView

class CTopNavView : public CDockablePane
{
	DECLARE_DYNAMIC(CTopNavView)

public:
	CTopNavView();
	virtual ~CTopNavView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	CCustomHeightMFCTabCtrl				m_wndTabs;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL CanBeClosed() const;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CListCtrl		m_wndEmpty;
	afx_msg LRESULT OnChangeActiveTab(WPARAM wParam, LPARAM lParam);
};


