// PolicyEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "PolicyEditDlg.h"
#include "../Public/Utils.h"

// CPolicyEditDlg dialog

IMPLEMENT_DYNAMIC(CPolicyEditDlg, CDialog)

CPolicyEditDlg::CPolicyEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPolicyEditDlg::IDD, pParent)
{

}

CPolicyEditDlg::~CPolicyEditDlg()
{
}

void CPolicyEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_UPGRADE, m_List);
}


BEGIN_MESSAGE_MAP(CPolicyEditDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CPolicyEditDlg::OnBnClickedButtonDel)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_UPGRADE, &CPolicyEditDlg::OnNMClickListUpgrade)
	ON_BN_CLICKED(IDOK, &CPolicyEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPolicyEditDlg message handlers

BOOL CPolicyEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_List.InsertColumn(0, _T("开始时间"),	LVCFMT_LEFT, 150);
	m_List.InsertColumn(1, _T("结束时间"),	LVCFMT_LEFT, 150);
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_List.SetExtendedStyle(dwStyle);

	for (int i = 0; i < (int)m_vecRange.size(); i++)
	{
		int iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
		_TIME_RANGE range = m_vecRange[i];
		COleDateTime oleStart((__time32_t)range.uTimeStart);
		COleDateTime oleEnd((__time32_t)range.uTimeEnd);

		CString strTmp;
		strTmp.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
			oleStart.GetYear(), oleStart.GetMonth(), oleStart.GetDay(), oleStart.GetHour(), oleStart.GetMinute(), oleStart.GetSecond());
		m_List.SetItemText(iIndex, 0, strTmp);
		strTmp.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
			oleEnd.GetYear(), oleEnd.GetMonth(), oleEnd.GetDay(), oleEnd.GetHour(), oleEnd.GetMinute(), oleEnd.GetSecond());
		m_List.SetItemText(iIndex, 1, strTmp);
	}

	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPolicyEditDlg::OnBnClickedButtonDel()
{
	int iClickIndex = m_List.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iClickIndex)
	{
		m_List.DeleteItem(iClickIndex);
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	}
}

void CPolicyEditDlg::OnClose()
{
	CDialog::OnClose();
}

void CPolicyEditDlg::OnNMClickListUpgrade(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iClickIndex = m_List.GetNextItem(-1, LVIS_SELECTED);
	if (-1 == iClickIndex)
	{
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(TRUE);
	}
	*pResult = 0;
}

void CPolicyEditDlg::OnBnClickedOk()
{
	m_vecRange.clear();
	for (int i = 0; i < m_List.GetItemCount(); i++)
	{
		CString strStart = m_List.GetItemText(i, 0);
		CString strEnd = m_List.GetItemText(i, 1);
		COleDateTime oleStart;
		BOOL bParse = oleStart.ParseDateTime(strStart);
		COleDateTime oleEnd;
		bParse = bParse && oleEnd.ParseDateTime(strEnd);
		if (bParse)
		{
			_TIME_RANGE range;
			range.uTimeStart = Utils::GetCurrentTimestamp(oleStart);
			range.uTimeEnd = Utils::GetCurrentTimestamp(oleEnd);
			m_vecRange.push_back(range);
		}
	}

	OnOK();
}
