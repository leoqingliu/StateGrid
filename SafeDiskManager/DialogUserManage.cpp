// DialogUserManage.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogUserManage.h"
#include "DialogUserAdd.h"
#include "DialogUserModify.h"

// CDialogUserManage dialog

IMPLEMENT_DYNAMIC(CDialogUserManage, CDialog)

CDialogUserManage::CDialogUserManage(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogUserManage::IDD, pParent)
{

}

CDialogUserManage::~CDialogUserManage()
{
}

void CDialogUserManage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_USER, m_ListUser);
}


BEGIN_MESSAGE_MAP(CDialogUserManage, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_USER, &CDialogUserManage::OnNMClickListUser)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDialogUserManage::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CDialogUserManage::OnBnClickedButtonDel)
END_MESSAGE_MAP()


// CDialogUserManage message handlers

BOOL CDialogUserManage::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	//
	m_ListUser.InsertColumn(0, _T("用户名"),	LVCFMT_LEFT, 200);
	DWORD dwStyle = m_ListUser.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_ListUser.SetExtendedStyle(dwStyle);

	for (int i = 0; i < (int)theApp.m_userConfig.m_vecClient.size(); i++)
	{
		int iIndex = m_ListUser.InsertItem(m_ListUser.GetItemCount(), _T(""));
		m_ListUser.SetItemText(iIndex, 0, theApp.m_userConfig.m_vecClient[i].strUser);	
	}

	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogUserManage::OnNMClickListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iClickIndex = m_ListUser.GetNextItem(-1, LVIS_SELECTED);
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

void CDialogUserManage::OnBnClickedButtonAdd()
{
	CDialogUserAdd dlg;
	if (IDOK == dlg.DoModal())
	{
		BOOL bFound = FALSE;
		for (int i = 0; i < (int)theApp.m_userConfig.m_vecClient.size(); i++)
		{
			if (theApp.m_userConfig.m_vecClient[i].strUser == dlg.m_strUser)
			{
				bFound = TRUE;
				break;
			}
		}
		if (!bFound)
		{
			CUserConfig::_USER_INFO userInfo;
			userInfo.strUser = dlg.m_strUser;
			userInfo.strPassword = dlg.m_strPassword;
			theApp.m_userConfig.m_vecClient.push_back(userInfo);
			theApp.m_userConfig.Save();
			int iIndex = m_ListUser.InsertItem(m_ListUser.GetItemCount(), _T(""));
			m_ListUser.SetItemText(iIndex, 0, userInfo.strUser);	
		}
		else
		{
			AfxMessageBox(_T("用户名重复！"), MB_ICONERROR);
		}
	}
}

void CDialogUserManage::OnBnClickedButtonDel()
{
	int iClickIndex = m_ListUser.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iClickIndex)
	{
		CString strUser = m_ListUser.GetItemText(iClickIndex, 0);
		std::vector<CUserConfig::USER_INFO>::iterator pIter = theApp.m_userConfig.m_vecClient.begin();
		for (; pIter != theApp.m_userConfig.m_vecClient.end(); pIter++)
		{
			if (pIter->strUser == strUser)
			{
				break;
			}
		}
		if (pIter == theApp.m_userConfig.m_vecClient.end())
		{
			AfxMessageBox(_T("没有找到用户!"), MB_ICONERROR);
		}
		else
		{
			theApp.m_userConfig.m_vecClient.erase(pIter);
			theApp.m_userConfig.Save();
			m_ListUser.DeleteItem(iClickIndex);
			GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
		}
	}
}

