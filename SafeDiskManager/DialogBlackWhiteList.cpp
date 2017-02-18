// DialogBlackWhiteList.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogBlackWhiteList.h"
#include "WindowDlg.h"
#include "ChildFrm.h"

// CDialogBlackWhiteList dialog

IMPLEMENT_DYNAMIC(CDialogBlackWhiteList, CDialog)

CDialogBlackWhiteList::CDialogBlackWhiteList(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogBlackWhiteList::IDD, pParent)
{

}

CDialogBlackWhiteList::~CDialogBlackWhiteList()
{
}

void CDialogBlackWhiteList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WIN, m_WinList);
	DDX_Control(pDX, IDC_LIST_WHITE_WIN, m_WhiteWinList);
}


BEGIN_MESSAGE_MAP(CDialogBlackWhiteList, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WIN_ADD, &CDialogBlackWhiteList::OnBnClickedButtonWinAdd)
	ON_BN_CLICKED(IDC_BUTTON_WIN_DEL, &CDialogBlackWhiteList::OnBnClickedButtonWinDel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_WIN, &CDialogBlackWhiteList::OnNMClickListWin)
	ON_BN_CLICKED(IDOK, &CDialogBlackWhiteList::OnBnClickedOk)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON_WHITE_WIN_ADD, &CDialogBlackWhiteList::OnBnClickedButtonWhiteWinAdd)
	ON_BN_CLICKED(IDC_BUTTON_WHITE_WIN_DEL, &CDialogBlackWhiteList::OnBnClickedButtonWhiteWinDel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_WHITE_WIN, &CDialogBlackWhiteList::OnNMClickListWhiteWin)
	ON_BN_CLICKED(IDC_RADIO_LIST, &CDialogBlackWhiteList::OnBnClickedRadioList)
	ON_BN_CLICKED(IDC_RADIO_WHITE_LIST, &CDialogBlackWhiteList::OnBnClickedRadioWhiteList)
END_MESSAGE_MAP()


// CDialogBlackWhiteList message handlers

BOOL CDialogBlackWhiteList::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	m_WinList.InsertColumn(0, _T("窗口名"),	LVCFMT_LEFT, 130);
	m_WinList.InsertColumn(1, _T("类名"),	LVCFMT_LEFT, 130);
	DWORD dwStyle = m_WinList.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_WinList.SetExtendedStyle(dwStyle);
	
	m_WhiteWinList.InsertColumn(0, _T("窗口名"),	LVCFMT_LEFT, 130);
	m_WhiteWinList.InsertColumn(1, _T("类名"),	LVCFMT_LEFT, 130);
	dwStyle = m_WhiteWinList.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_WhiteWinList.SetExtendedStyle(dwStyle);

	m_bWhiteMode = FALSE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

VOID CDialogBlackWhiteList::UpdateList()
{
	for (int i = 0; i < (int)m_vecWin.size(); i++)
	{
		CString strWin = m_vecWin[i];
		CString strClass = m_vecClass[i];
		int iIndex = m_WinList.InsertItem(m_WinList.GetItemCount(), _T(""));
		m_WinList.SetItemText(iIndex, 0, strWin);
		m_WinList.SetItemText(iIndex, 1, strClass);		
	}

	for (int i = 0; i < (int)m_vecWhiteClass.size(); i++)
	{
		CString strWin = m_vecWhiteWin[i];
		CString strClass = m_vecWhiteClass[i];
		int iIndex = m_WhiteWinList.InsertItem(m_WhiteWinList.GetItemCount(), _T(""));
		m_WhiteWinList.SetItemText(iIndex, 0, strWin);
		m_WhiteWinList.SetItemText(iIndex, 1, strClass);		
	}
	EnableList(m_bWhiteMode);
	CheckDlgButton(m_bWhiteMode ? IDC_RADIO_WHITE_LIST : IDC_RADIO_LIST, BST_CHECKED);
}

void CDialogBlackWhiteList::OnBnClickedButtonWinAdd()
{
	CWindowDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		int iCount = m_WinList.GetItemCount();
		BOOL bMatched = FALSE;
		for (int i = 0; i < iCount; i++)
		{
			CString strWin = m_WinList.GetItemText(i, 0);
			CString strClass = m_WinList.GetItemText(i, 1);
			if (strWin == dlg.m_strWindow && strClass == dlg.m_strClass)
			{
				bMatched = TRUE;
				break;
			}
		}
		if (!bMatched)
		{
			int iIndex = m_WinList.InsertItem(m_WinList.GetItemCount(), _T(""));
			m_WinList.SetItemText(iIndex, 0, dlg.m_strWindow);
			m_WinList.SetItemText(iIndex, 1, dlg.m_strClass);
		}
		else
		{
			AfxMessageBox(_T("请不要重复添加！"), MB_ICONERROR);
		}
	}
}

void CDialogBlackWhiteList::OnBnClickedButtonWinDel()
{
	int iClickIndex = m_WinList.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iClickIndex)
	{
		m_WinList.DeleteItem(iClickIndex);
		GetDlgItem(IDC_BUTTON_WIN_DEL)->EnableWindow(FALSE);
	}
}

void CDialogBlackWhiteList::OnNMClickListWin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iClickIndex = m_WinList.GetNextItem(-1, LVIS_SELECTED);
	if (-1 == iClickIndex)
	{
		GetDlgItem(IDC_BUTTON_WIN_DEL)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_WIN_DEL)->EnableWindow(TRUE);
	}

	*pResult = 0;
}

void CDialogBlackWhiteList::OnBnClickedOk()
{
	UpdateData();

	m_vecWin.clear();
	m_vecClass.clear();
	for (int i = 0; i < m_WinList.GetItemCount(); i++)
	{
		CString strWin = m_WinList.GetItemText(i, 0);
		CString strClass = m_WinList.GetItemText(i, 1);
		m_vecWin.push_back(strWin);
		m_vecClass.push_back(strClass);
	}

	m_vecWhiteWin.clear();
	m_vecWhiteClass.clear();
	for (int i = 0; i < m_WhiteWinList.GetItemCount(); i++)
	{
		CString strWin = m_WhiteWinList.GetItemText(i, 0);
		CString strClass = m_WhiteWinList.GetItemText(i, 1);
		m_vecWhiteWin.push_back(strWin);
		m_vecWhiteClass.push_back(strClass);
	}

	m_bWhiteMode = IsDlgButtonChecked(IDC_RADIO_WHITE_LIST) == BST_CHECKED ? TRUE : FALSE;

	//OnOK();
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlSetBlackWhiteList(m_vecClass, m_vecWin, m_vecWhiteClass, m_vecWhiteWin, m_bWhiteMode);
}

void CDialogBlackWhiteList::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CRect rect;
	GetClientRect(rect);
	dc.FillSolidRect(rect, RGB(255, 255, 255));
	dc.SetBkMode(TRANSPARENT);
}

void CDialogBlackWhiteList::OnBnClickedButtonWhiteWinAdd()
{
	CWindowDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		int iCount = m_WhiteWinList.GetItemCount();
		BOOL bMatched = FALSE;
		for (int i = 0; i < iCount; i++)
		{
			CString strWin = m_WhiteWinList.GetItemText(i, 0);
			CString strClass = m_WhiteWinList.GetItemText(i, 1);
			if (strWin == dlg.m_strWindow && strClass == dlg.m_strClass)
			{
				bMatched = TRUE;
				break;
			}
		}
		if (!bMatched)
		{
			int iIndex = m_WhiteWinList.InsertItem(m_WhiteWinList.GetItemCount(), _T(""));
			m_WhiteWinList.SetItemText(iIndex, 0, dlg.m_strWindow);
			m_WhiteWinList.SetItemText(iIndex, 1, dlg.m_strClass);
		}
		else
		{
			AfxMessageBox(_T("请不要重复添加！"), MB_ICONERROR);
		}
	}
}

void CDialogBlackWhiteList::OnBnClickedButtonWhiteWinDel()
{
	int iClickIndex = m_WhiteWinList.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iClickIndex)
	{
		m_WhiteWinList.DeleteItem(iClickIndex);
		GetDlgItem(IDC_BUTTON_WHITE_WIN_DEL)->EnableWindow(FALSE);
	}
}

void CDialogBlackWhiteList::OnNMClickListWhiteWin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iClickIndex = m_WhiteWinList.GetNextItem(-1, LVIS_SELECTED);
	if (-1 == iClickIndex)
	{
		GetDlgItem(IDC_BUTTON_WHITE_WIN_DEL)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_WHITE_WIN_DEL)->EnableWindow(TRUE);
	}
	*pResult = 0;
}

void CDialogBlackWhiteList::OnBnClickedRadioList()
{
	EnableList(FALSE);
}

void CDialogBlackWhiteList::OnBnClickedRadioWhiteList()
{
	EnableList(TRUE);
}

VOID CDialogBlackWhiteList::EnableList(BOOL bWhite)
{
	GetDlgItem(IDC_LIST_WIN)->EnableWindow(!bWhite);
	GetDlgItem(IDC_BUTTON_WIN_ADD)->EnableWindow(!bWhite);
	GetDlgItem(IDC_BUTTON_WIN_DEL)->EnableWindow(!bWhite);

	GetDlgItem(IDC_LIST_WHITE_WIN)->EnableWindow(bWhite);
	GetDlgItem(IDC_BUTTON_WHITE_WIN_ADD)->EnableWindow(bWhite);
	GetDlgItem(IDC_BUTTON_WHITE_WIN_DEL)->EnableWindow(bWhite);
}
