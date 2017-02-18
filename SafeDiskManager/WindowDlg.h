#pragma once
#include "afxwin.h"


// CWindowDlg dialog

class CWindowDlg : public CDialog
{
	DECLARE_DYNAMIC(CWindowDlg)

public:
	CWindowDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWindowDlg();

// Dialog Data
	enum { IDD = IDD_WINDOW_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strWindow;
	CString m_strClass;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CStatic m_ctrlLook;
	HICON m_hIconBlank;
	HICON m_hIconScan;
	HCURSOR m_hCursorScan;
	HCURSOR m_hCursorPrev;
	HWND m_hWndPrev;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void StartLooking(void);
	void StopLooking(void);
	void Scan(CPoint point);
	HWND SmallestWindowFromPoint(const POINT point);
	void InvertBorder(const HWND hWnd);
	BOOL m_bIsLooking;
	afx_msg void OnBnClickedOk();
	CString m_strProcess;
	CString m_strDesc;
	int m_iType;
	BOOL m_bDisableExcept;
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
