#pragma once


// CUDiskLoginDlg dialog

class CUDiskLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CUDiskLoginDlg)

public:
	CUDiskLoginDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUDiskLoginDlg();

// Dialog Data
	enum { IDD = IDD_UDISK_LOGIN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strPassword;
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
