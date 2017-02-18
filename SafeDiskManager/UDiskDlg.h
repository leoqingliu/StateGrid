#pragma once
#include "afxcmn.h"

#include <vector>
// CUDiskDlg dialog

class CUDiskDlg : public CDialog
{
	DECLARE_DYNAMIC(CUDiskDlg)

public:
	CUDiskDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUDiskDlg();

// Dialog Data
	enum { IDD = IDD_UDISK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_List;
	CString m_strUDiskId;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	std::vector<CString> m_vecItems;
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonMoveWhite();
	CListCtrl m_ListAuth;
	afx_msg void OnNMClickListAuth(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonInport();
	afx_msg void OnBnClickedButtonExport();
};
