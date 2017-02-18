// ResEditDlg.h : header file
//

#if !defined(AFX_RESEDITDLG_H__1801E1C8_1AC2_4BDB_8412_0DCF19AD4ABE__INCLUDED_)
#define AFX_RESEDITDLG_H__1801E1C8_1AC2_4BDB_8412_0DCF19AD4ABE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
//
#include "../ConnConf.h"
using namespace NInformation;

/////////////////////////////////////////////////////////////////////////////
// CResEditDlg dialog

class CResEditDlg : public CDialog
{
// Construction
public:
	CResEditDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CResEditDlg)
	enum { IDD = IDD_RESEDIT_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CResEditDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnOk();
	afx_msg void OnRadioDynamic();
	afx_msg void OnRadioPortDefault();
	afx_msg void OnRadioPortUser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	//
	CommunicationInfo	m_ConnectInfo;
	TCHAR			m_szFile[MAX_PATH];
private:
	//
	DWORD			ConvertAddr(DWORD Addr);
	BOOL			ReadConfig();
	BOOL			WriteConfig();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESEDITDLG_H__1801E1C8_1AC2_4BDB_8412_0DCF19AD4ABE__INCLUDED_)
