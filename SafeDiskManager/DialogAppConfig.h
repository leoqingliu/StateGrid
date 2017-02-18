#pragma once


// CDialogAppConfig dialog

class CDialogAppConfig : public CDialog
{
	DECLARE_DYNAMIC(CDialogAppConfig)

public:
	CDialogAppConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogAppConfig();

// Dialog Data
	enum { IDD = IDD_DIALOG_APP_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

public:
	CString m_strHost;
	DWORD m_dwPort;
	CString m_strKey;
	CString m_strPage;
	DWORD m_dwTimeout;
	CString m_strLocalDir;
	afx_msg void OnBnClickedButtonRereg();
};
