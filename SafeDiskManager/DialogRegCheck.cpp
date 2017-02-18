// DialogRegCheck.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogRegCheck.h"
#include "../Public/IdUtils.h"

// CDialogRegCheck dialog

IMPLEMENT_DYNAMIC(CDialogRegCheck, CDialog)

CDialogRegCheck::CDialogRegCheck(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogRegCheck::IDD, pParent)
	, m_strHostKey(_T(""))
	, m_strKey(_T(""))
{

}

CDialogRegCheck::~CDialogRegCheck()
{
}

void CDialogRegCheck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HOST_KEY, m_strHostKey);
	DDX_Text(pDX, IDC_EDIT_KEY, m_strKey);
}


BEGIN_MESSAGE_MAP(CDialogRegCheck, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogRegCheck::OnBnClickedOk)
END_MESSAGE_MAP()


// CDialogRegCheck message handlers

BOOL CDialogRegCheck::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	USES_CONVERSION;
	LPSTR pszMacId = T2A(theApp.m_strMacId);
	CIdUtils::LocalEnc(pszMacId, strlen(pszMacId), m_strHostKey);
	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogRegCheck::OnBnClickedOk()
{
	UpdateData();
	if (m_strKey.IsEmpty())
	{
		AfxMessageBox(_T("×¢²áÂë²»ÄÜÎª¿Õ£¡"), MB_ICONERROR);
		return;
	}

	OnOK();
}
