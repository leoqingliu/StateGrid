#pragma once

// CViewCmd

class CViewRichEditCallBack
{
public:
	virtual void OnEnMsgfilter(NMHDR *pNMHDR, LRESULT *pResult) = 0;
};

class CViewRichEdit : public CRichEditCtrl
{
	DECLARE_DYNAMIC(CViewRichEdit)

public:
	CViewRichEdit();
	virtual ~CViewRichEdit();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnMsgfilter(NMHDR *pNMHDR, LRESULT *pResult);

private:
	//
	LONG			m_dwCursor;
	CViewRichEditCallBack* m_pCallBack;
public:
	void SetEditCallBack(CViewRichEditCallBack* pCallBack);
	CViewRichEditCallBack* GetEditCallBack();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
