// DialogTip.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenLocker.h"
#include "DialogTip.h"


// CDialogTip dialog

IMPLEMENT_DYNAMIC(CDialogTip, CDialog)

CDialogTip::CDialogTip(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogTip::IDD, pParent)
{

}

CDialogTip::~CDialogTip()
{
}

void CDialogTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogTip, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDialogTip message handlers

BOOL CDialogTip::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	SetTimer(1, 1000 * 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogTip::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent)
	{
		OnOK();
	}

	CDialog::OnTimer(nIDEvent);
}
