#pragma once


// CLoginModifyDlg dialog

class CLoginModifyDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginModifyDlg)

public:
	CLoginModifyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoginModifyDlg();

// Dialog Data
	enum { IDD = IDD_LOGIN_MODIFY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnOk();
	CString m_strPassword;
	CString m_strPassword2;
};
