#pragma once


// CDialogModify dialog

class CDialogUserModify : public CDialog
{
	DECLARE_DYNAMIC(CDialogUserModify)

public:
	CDialogUserModify(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogUserModify();

// Dialog Data
	enum { IDD = IDD_DIALOG_USER_MODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CString m_strPassword;
	CString m_strPassword2;
};
