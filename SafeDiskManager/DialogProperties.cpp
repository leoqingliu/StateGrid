// DialogProperties.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogProperties.h"

// CDialogProperties dialog

IMPLEMENT_DYNAMIC(CDialogProperties, CDialog)

CDialogProperties::CDialogProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogProperties::IDD, pParent)
	, m_dwID(0)
	, m_strIPOut(_T(""))
	, m_strIPIn(_T(""))
	, m_strUserIn(_T(""))
	, m_strLastLogin(_T(""))
	, m_strHostname(_T(""))
	, m_strDeviceSerial(_T(""))
	, m_strComment(_T(""))
{
}

CDialogProperties::~CDialogProperties()
{
}

void CDialogProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ID, m_dwID);
	DDX_Text(pDX, IDC_STATIC_IP_OUT, m_strIPOut);
	DDX_Text(pDX, IDC_STATIC_IP_IN, m_strIPIn);
	DDX_Text(pDX, IDC_STATIC_USER_IN, m_strUserIn);
	DDX_Text(pDX, IDC_STATIC_LAST_LOGIN, m_strLastLogin);
	DDX_Text(pDX, IDC_STATIC_HOSTNAME, m_strHostname);
	DDX_Text(pDX, IDC_STATIC_DEVICE_SERIAL, m_strDeviceSerial);
	DDX_Text(pDX, IDC_STATIC_COMMENT, m_strComment);
}


BEGIN_MESSAGE_MAP(CDialogProperties, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogProperties::OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDialogProperties message handlers

BOOL CDialogProperties::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogProperties::OnBnClickedOk()
{
	UpdateData();

	OnOK();
}

HBRUSH CDialogProperties::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

