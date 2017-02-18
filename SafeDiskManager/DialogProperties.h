#pragma once
#include "afxdtctl.h"
#include "afxcmn.h"

// CDialogProperties dialog

class CDialogProperties : public CDialog
{
	DECLARE_DYNAMIC(CDialogProperties)

public:
	CDialogProperties(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogProperties();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwID;
	CString m_strIPOut;
	CString m_strIPIn;
	CString m_strUserIn;
	CString m_strLastLogin;
	CString m_strHostname;
	CString m_strDeviceSerial;
	CString m_strComment;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
