
// ClientOpDlg.h : header file
//

#pragma once


// CClientOpDlg dialog
class CClientOpDlg : public CDialog
{
// Construction
public:
	CClientOpDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLIENTOP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonInstall();
	afx_msg void OnBnClickedButtonUninstall();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	void UpdateDisplay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	
	afx_msg void OnBnClickedButtonGen();
};
