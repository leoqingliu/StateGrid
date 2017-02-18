#pragma once
#include "afxcmn.h"
#include <vector>

// CUrlDialog dialog

class CUrlDialog : public CDialog
{
	DECLARE_DYNAMIC(CUrlDialog)

public:
	CUrlDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUrlDialog();

// Dialog Data
	enum { IDD = IDD_URL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strUrl;
	CListCtrl m_List;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonImport();
	afx_msg void OnBnClickedButtonClean();
	afx_msg void OnBnClickedButtonExport();
	afx_msg void OnEnChangeEditUrl();
	std::vector<CString> m_vecUrls;
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	BOOL m_bCheckAutoAddHttp;
};
