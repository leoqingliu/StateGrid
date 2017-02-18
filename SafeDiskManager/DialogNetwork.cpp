// DialogNetwork.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogNetwork.h"


// CDialogNetwork dialog

IMPLEMENT_DYNAMIC(CDialogNetwork, CDialog)

CDialogNetwork::CDialogNetwork(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogNetwork::IDD, pParent)
{

}

CDialogNetwork::~CDialogNetwork()
{
}

void CDialogNetwork::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogNetwork, CDialog)
END_MESSAGE_MAP()


// CDialogNetwork message handlers

BOOL CDialogNetwork::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
