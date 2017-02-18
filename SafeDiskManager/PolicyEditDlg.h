#pragma once
#include "afxcmn.h"


// CPolicyEditDlg dialog

class CPolicyEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CPolicyEditDlg)

public:
	CPolicyEditDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPolicyEditDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_POLICY_EDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_List;
	afx_msg void OnBnClickedButtonDel();
	std::vector<_TIME_RANGE> m_vecRange;
	afx_msg void OnClose();
	afx_msg void OnNMClickListUpgrade(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
};
