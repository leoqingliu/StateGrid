// DialogAppConfig.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogAppConfig.h"


// CDialogAppConfig dialog

IMPLEMENT_DYNAMIC(CDialogAppConfig, CDialog)

CDialogAppConfig::CDialogAppConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogAppConfig::IDD, pParent)
	, m_strHost(_T(""))
	, m_dwPort(0)
	, m_strKey(_T(""))
	, m_strPage(_T(""))
	, m_dwTimeout(0)
	, m_strLocalDir(_T(""))
{

}

CDialogAppConfig::~CDialogAppConfig()
{
}

void CDialogAppConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HOST, m_strHost);
	DDX_Text(pDX, IDC_EDIT_PORT, m_dwPort);
	DDX_Text(pDX, IDC_EDIT_KEY, m_strKey);
	DDX_Text(pDX, IDC_EDIT_PAGE, m_strPage);
	DDX_Text(pDX, IDC_EDIT_TIMEOUT, m_dwTimeout);
	DDX_Text(pDX, IDC_EDIT_LOCAL_DIR, m_strLocalDir);
}


BEGIN_MESSAGE_MAP(CDialogAppConfig, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REREG, &CDialogAppConfig::OnBnClickedButtonRereg)
END_MESSAGE_MAP()


// CDialogAppConfig message handlers

void CDialogAppConfig::OnBnClickedButtonRereg()
{
	theApp.WriteProfileString(_T("REG"), _T("KEY"), _T(""));
	AfxMessageBox(_T("重启程序后生效!"), MB_ICONINFORMATION);
	OnOK();
}
