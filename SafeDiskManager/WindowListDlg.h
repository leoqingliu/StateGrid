#pragma once
#include "afxcmn.h"
#include <vector>
#include "../MacConfig.h"

// CWindowListDlg dialog

class CWindowListDlg : public CDialog
{
	DECLARE_DYNAMIC(CWindowListDlg)

public:
	CWindowListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWindowListDlg();

// Dialog Data
	enum { IDD = IDD_WINDOW_RULE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	CListCtrl m_List;
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	std::vector<CONFIG_INFO_ITEM> m_vecItems;
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonClean();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonExport();
	BOOL m_bDisableExcept;
};
