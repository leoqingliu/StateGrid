#pragma once


// CFileManCtrl

class CFileManCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CFileManCtrl)

public:
	CFileManCtrl();
	virtual ~CFileManCtrl();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	CImageList m_ImageList;
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnUpdateChildDownload(CCmdUI *pCmdUI);
	afx_msg void OnChildDownload();
	afx_msg void OnUpdateChildUpload(CCmdUI *pCmdUI);
	afx_msg void OnChildUpload();
	afx_msg void OnUpdateChildRefresh(CCmdUI *pCmdUI);
	afx_msg void OnChildRefresh();
};


