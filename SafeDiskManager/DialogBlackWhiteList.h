#pragma once
#include "afxcmn.h"


// CDialogBlackWhiteList dialog

class CDialogBlackWhiteList : public CDialog
{
	DECLARE_DYNAMIC(CDialogBlackWhiteList)

public:
	CDialogBlackWhiteList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogBlackWhiteList();

// Dialog Data
	enum { IDD = IDD_DIALOG_BLACKWHITE_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_WinList;
	afx_msg void OnBnClickedButtonWinAdd();
	afx_msg void OnBnClickedButtonWinDel();
	afx_msg void OnNMClickListWin(NMHDR *pNMHDR, LRESULT *pResult);
	std::vector<CString> m_vecWin;
	std::vector<CString> m_vecClass;
	std::vector<CString> m_vecWhiteWin;
	std::vector<CString> m_vecWhiteClass;
	BOOL m_bWhiteMode;
	afx_msg void OnBnClickedOk();
	afx_msg void OnPaint();
	CListCtrl m_WhiteWinList;
	afx_msg void OnBnClickedButtonWhiteWinAdd();
	afx_msg void OnBnClickedButtonWhiteWinDel();
	afx_msg void OnNMClickListWhiteWin(NMHDR *pNMHDR, LRESULT *pResult);
	VOID UpdateList();
	afx_msg void OnBnClickedRadioList();
	afx_msg void OnBnClickedRadioWhiteList();
	VOID EnableList(BOOL bWhite);
};
