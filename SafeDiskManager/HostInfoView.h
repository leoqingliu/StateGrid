#pragma once

#include "../Public/Grid/GridCtrl_src/GridCtrl.h"

// CHostInfoView view

class CHostInfoView : public CView
{
	DECLARE_DYNCREATE(CHostInfoView)

public:
	CHostInfoView();           // protected constructor used by dynamic creation
	virtual ~CHostInfoView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
	CGridCtrl* m_pGridCtrl;
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	VOID Close();
	afx_msg void OnClose();
	//void ParseSpecTime(DWORD dwTime, DWORD &dwHour, DWORD &dwMin, DWORD &dwSec);
	void UpdateItems();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnHostRefresh();
	afx_msg void OnUpdateHostRefresh(CCmdUI *pCmdUI);
};


