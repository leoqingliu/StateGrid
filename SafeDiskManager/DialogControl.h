#pragma once


// CDialogControl dialog

class CDialogControl : public CDialog
{
	DECLARE_DYNAMIC(CDialogControl)

public:
	CDialogControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogControl();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRestart();
	afx_msg void OnBnClickedButtonShutdown();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonLogoff();
};
