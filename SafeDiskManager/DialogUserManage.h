#pragma once
#include "afxcmn.h"


// CDialogUserManage dialog

class CDialogUserManage : public CDialog
{
	DECLARE_DYNAMIC(CDialogUserManage)

public:
	CDialogUserManage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogUserManage();

// Dialog Data
	enum { IDD = IDD_DIALOG_USER_MANAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListUser(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ListUser;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
};
