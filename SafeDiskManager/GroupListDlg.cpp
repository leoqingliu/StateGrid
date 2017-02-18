// GroupListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "GroupListDlg.h"


// CGroupListDlg dialog

IMPLEMENT_DYNAMIC(CGroupListDlg, CDialog)

CGroupListDlg::CGroupListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGroupListDlg::IDD, pParent)
{
}

CGroupListDlg::~CGroupListDlg()
{
}

void CGroupListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CGroupListDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CGroupListDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CGroupListDlg::OnNMDblclkList)
END_MESSAGE_MAP()


// CGroupListDlg message handlers

BOOL CGroupListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	m_List.InsertColumn(0, _T("名称"),	LVCFMT_LEFT, 200);
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_List.SetExtendedStyle(dwStyle);

	for (int i = 0; i < (int)m_vecNames.size(); i++)
	{
		int iPos = m_List.InsertItem(m_List.GetItemCount(), _T(""));
		m_List.SetItemText(iPos, 0, m_vecNames[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGroupListDlg::OnBnClickedOk()
{
	int iClickIndex = m_List.GetNextItem(-1, LVIS_SELECTED);
	if (-1 == iClickIndex)
	{
		AfxMessageBox(_T("请选择分组!"), MB_ICONERROR);
		return;
	}

	m_strCurName = m_List.GetItemText(iClickIndex, 0);
	OnOK();
}

void CGroupListDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iClickIndex = m_List.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iClickIndex)
	{
		m_strCurName = m_List.GetItemText(iClickIndex, 0);
		OnOK();
	}

	*pResult = 0;
}
