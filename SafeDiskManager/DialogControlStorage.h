#pragma once

#include "resource.h"

// CControlStorageDlg dialog

class CDialogControlStorage : public CDialog
{
	DECLARE_DYNAMIC(CDialogControlStorage)

public:
	CDialogControlStorage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogControlStorage();

// Dialog Data
	enum { IDD = IDD_CONTROL_STORAGE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	int m_uUDiskType;
	BOOL m_bCheckCD;
	BOOL m_bCheckFloppy;
	BOOL m_bCheckCDBurn;

public:
	CONFIG_INFO m_Config;
	BOOL m_bConfigUpdated;
	afx_msg void OnBnClickedButtonStorageSave();
	VOID UpdateDisplay();
	afx_msg void OnPaint();
};
