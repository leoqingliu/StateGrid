#pragma once


// CDialogLogin dialog

class CDialogUserLogin : public CDialog
{
	DECLARE_DYNAMIC(CDialogUserLogin)

public:
	CDialogUserLogin(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogUserLogin();

// Dialog Data
	enum { IDD = IDD_DIALOG_USER_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strUser;
	CString m_strPassword;
	afx_msg void OnBnClickedOk();
};
