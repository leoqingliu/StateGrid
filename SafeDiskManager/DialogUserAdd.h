#pragma once


// CDialogAddUser dialog

class CDialogUserAdd : public CDialog
{
	DECLARE_DYNAMIC(CDialogUserAdd)

public:
	CDialogUserAdd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogUserAdd();

// Dialog Data
	enum { IDD = IDD_DIALOG_USER_ADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strUser;
	CString m_strPassword;
	CString m_strPassword2;
	afx_msg void OnBnClickedOk();
};
