#pragma once
#include "afxcmn.h"


// CUpgradeDlg dialog

class CUpgradeDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpgradeDlg)

public:
	CUpgradeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpgradeDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_UPGRADE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonUpgrade();
	CListCtrl m_List;
	std::vector<DWORD> m_vecIds;
	afx_msg void OnBnClickedButtonUpgradeService();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonUpgradeConfig();
};
