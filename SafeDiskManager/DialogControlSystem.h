#pragma once

#include "resource.h"
#include "afxwin.h"
#include "TransparentCheckBox.h"

// CControlSystemDlg dialog

class CDialogControlSystem : public CDialog
{
	DECLARE_DYNAMIC(CDialogControlSystem)

public:
	CDialogControlSystem(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogControlSystem();

// Dialog Data
	enum { IDD = IDD_CONTROL_SYSTEM_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL m_bCheckRegEdit;
	BOOL m_bCheckDevMgr;
	BOOL m_bCheckGpedit;
	BOOL m_bCheckTaskmgr;
	BOOL m_bCheckMsconfig;
	BOOL m_bCheckComputermgr;
	BOOL m_bCheckFormat;
	BOOL m_bCheckDos;
	BOOL m_bCheckIp;
	BOOL m_bCheckClipboard;
	BOOL m_bCheckTelnet;
	BOOL m_bCheckShare;
	BOOL m_bCheckLan;
	BOOL m_bCheckVir;
	BOOL m_bCheckCreateUser;

public:
	CONFIG_INFO m_Config;
	BOOL m_bConfigUpdated;
	afx_msg void OnBnClickedButtonSystemSave();
	VOID UpdateDisplay();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
//	CTransparentCheckBox m_btnCheckRegEdit;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdrawCheckRegedit(NMHDR *pNMHDR, LRESULT *pResult);
};
