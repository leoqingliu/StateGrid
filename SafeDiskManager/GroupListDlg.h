#pragma once
#include "afxcmn.h"


// CGroupListDlg dialog

class CGroupListDlg : public CDialog
{
	DECLARE_DYNAMIC(CGroupListDlg)

public:
	CGroupListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGroupListDlg();

// Dialog Data
	enum { IDD = IDD_GROUP_LIST_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_List;
	std::vector<CString> m_vecNames;
	CString m_strCurName;
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
};
