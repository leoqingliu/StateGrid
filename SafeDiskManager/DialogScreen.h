#pragma once

#include "resource.h"

// CDialogScreen dialog
class CDialogScreen : public CDialog
{
	DECLARE_DYNAMIC(CDialogScreen)

public:
	CDialogScreen(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogScreen();

// Dialog Data
	enum { IDD = IDD_DIALOG_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonScreen();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonVncView();
	afx_msg void OnBnClickedButtonVncControl();
	afx_msg void OnBnClickedButtonScreenAuto();
	void SetAutoTime(UINT uTime);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
