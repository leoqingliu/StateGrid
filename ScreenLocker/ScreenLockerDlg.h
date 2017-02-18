
// ScreenLockerDlg.h : header file
//

#pragma once
#include "../Public/ResizableLib/ResizableDialog.h"
#include "../ConnConf.h"
#include "MFCDesktopAlertNoCloseWnd.h"
#include "DialogTip.h"
#include <vector>
#include "../MacConfig.h"
#include "../Public/DevUtils.h"

// CScreenLockerDlg dialog
class CScreenLockerDlg : public CResizableDialog
{
// Construction
public:
	CScreenLockerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SCREENLOCKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bFullScreen;
	VOID ChangeFullScreen();
	BOOL SetAlpha(BYTE dwAlpha);
	VOID ControlKey(BOOL bEnable);
public:
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString m_strPassword;
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg LRESULT onAlertMessage(WPARAM wparam, LPARAM lparam);

	//CMFCDesktopAlertNoCloseWnd *m_pPopUp;
	CDialogTip *m_pPopUp;

public:
	CString m_strCurrentUser;
	struct ENUM_PROC_INFO
	{
		CString strProcessName;
		CString strFileDesc;
		CString strProductDesc;
	};
	struct ENUM_WIN_INFO
	{
		ENUM_WIN_INFO()
		{
			hWnd = NULL;
		}
		HWND hWnd;
		CString strWinName;
		CString strClassName;
		DWORD dwProcessId;
		CString strProcessName;
		CString strFileDesc;
		CString strProductDesc;
	};
	VOID FilterWnds(const std::vector<ENUM_WIN_INFO>& vecInfo);

public:
	CDevUtils m_DeviceUtils;
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	void UpdateExplorerId();
	DWORD m_dwExplorerId;
	std::vector<CString> m_vecUDiskLetter;
	CCriticalSection m_DataLock;
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	HACCEL m_hAccel;
	afx_msg void OnBnClickedButtonShow();
};
