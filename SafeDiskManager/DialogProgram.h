#pragma once


// CDialogProgram dialog

class CDialogProgram : public CDialog
{
	DECLARE_DYNAMIC(CDialogProgram)

public:
	CDialogProgram(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogProgram();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROGRAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPassword;
	afx_msg void OnBnClickedButtonSet();
	virtual BOOL OnInitDialog();
};
