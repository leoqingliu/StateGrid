#include "stdafx.h"
#include "SafeDiskManager.h"
#include "AboutDlg.h"

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

#if !defined(FULL_VERSION)
	CString strTmp;
	strTmp.Format(_T("ÊÚÈ¨Êý: %d"), theApp.m_uHostCount);
	SetDlgItemText(IDC_STATIC_LIMIT, strTmp);
#endif

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
