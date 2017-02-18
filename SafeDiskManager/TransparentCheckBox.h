#pragma once


// CTransparentCheckBox

class CTransparentCheckBox : public CButton
{
	DECLARE_DYNAMIC(CTransparentCheckBox)

public:
	CTransparentCheckBox();
	virtual ~CTransparentCheckBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


