#pragma once


// CDialogLock dialog

class CDialogLock : public CDialog
{
	DECLARE_DYNAMIC(CDialogLock)

public:
	CDialogLock(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogLock();

// Dialog Data
	enum { IDD = IDD_DIALOG_LOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonLock();
	afx_msg void OnBnClickedButtonUnlock();
	afx_msg void OnBnClickedButtonLockAuto();
	afx_msg void OnPaint();
	afx_msg void OnBnClickedButtonSetPassword();

public:
	void SetLockInfo(CString strPassword, DWORD dwIdleTime);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
