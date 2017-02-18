#pragma once

#include "resource.h"

// CControlDevDlg dialog

class CDialogControlDev : public CDialog
{
	DECLARE_DYNAMIC(CDialogControlDev)

public:
	CDialogControlDev(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogControlDev();

// Dialog Data
	enum { IDD = IDD_CONTROL_DEV_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	BOOL m_bCheckDeviceCell;
	BOOL m_bCheckPhoneAssist;
	BOOL m_bCheckNetworkWired;
	BOOL m_bCheckNetworkWireless;
	BOOL m_bCheckNetworkMini;
	BOOL m_bCheckBlueTooth;
	BOOL m_bCheckSerial;
	BOOL m_bCheck1394;
	BOOL m_bCheckIrda;
	BOOL m_bCheckPcmcia;
	BOOL m_bCheckModem;

public:
	CONFIG_INFO m_Config;
	BOOL m_bConfigUpdated;
	afx_msg void OnBnClickedButtonSave();
	VOID UpdateDisplay();
	afx_msg void OnPaint();
};
