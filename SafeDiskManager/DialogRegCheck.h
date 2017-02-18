#pragma once


// CDialogRegCheck dialog

class CDialogRegCheck : public CDialog
{
	DECLARE_DYNAMIC(CDialogRegCheck)

public:
	CDialogRegCheck(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogRegCheck();

// Dialog Data
	enum { IDD = IDD_DIALOG_REG_CHECK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	CString m_strHostKey;
	CString m_strKey;
};
