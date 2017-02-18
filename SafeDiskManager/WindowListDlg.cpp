// WindowListDlg.cpp : implementation file
//

#include "stdafx.h"
#include <resource.h>
#include "WindowListDlg.h"
#include "WindowDlg.h"
#include "../Public/Utils.h"

// CWindowListDlg dialog

IMPLEMENT_DYNAMIC(CWindowListDlg, CDialog)

CWindowListDlg::CWindowListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWindowListDlg::IDD, pParent)
{
	m_bDisableExcept = FALSE;
}

CWindowListDlg::~CWindowListDlg()
{
}

void CWindowListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
}


BEGIN_MESSAGE_MAP(CWindowListDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CWindowListDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CWindowListDlg::OnBnClickedButtonDel)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CWindowListDlg::OnNMClickList)
	ON_BN_CLICKED(IDOK, &CWindowListDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CWindowListDlg::OnNMDblclkList)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CWindowListDlg::OnBnClickedButtonClean)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CWindowListDlg::OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CWindowListDlg::OnBnClickedButtonExport)
END_MESSAGE_MAP()


// CWindowListDlg message handlers

BOOL CWindowListDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	m_List.InsertColumn(0, _T("类型"),		LVCFMT_LEFT, 50);
	m_List.InsertColumn(1, _T("窗口名称"),	LVCFMT_LEFT, 130);
	m_List.InsertColumn(2, _T("窗口类名"),		LVCFMT_LEFT, 130);
	m_List.InsertColumn(3, _T("进程名称"),	LVCFMT_LEFT, 130);
	m_List.InsertColumn(4, _T("文件描述"),	LVCFMT_LEFT, 130);
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_List.SetExtendedStyle(dwStyle);
	for (int i = 0; i < (int)m_vecItems.size(); i++)
	{
		INT iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
		m_List.SetItemData(iIndex, m_vecItems[i].iType);
		m_List.SetItemText(iIndex, 0, m_vecItems[i].GetTypeName());
		m_List.SetItemText(iIndex, 1, m_vecItems[i].szWindow);
		m_List.SetItemText(iIndex, 2, m_vecItems[i].szClass);
		m_List.SetItemText(iIndex, 3, m_vecItems[i].szProcess);
		m_List.SetItemText(iIndex, 4, m_vecItems[i].szDesc);
	}

	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWindowListDlg::OnBnClickedButtonAdd()
{
	CWindowDlg dlg;
	dlg.m_bDisableExcept = m_bDisableExcept;
	if (IDOK == dlg.DoModal())
	{
		int iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
		m_List.SetItemData(iIndex, dlg.m_iType);
		m_List.SetItemText(iIndex, 0, CONFIG_INFO_ITEM::GetTypeName(dlg.m_iType));
		m_List.SetItemText(iIndex, 1, dlg.m_strWindow);
		m_List.SetItemText(iIndex, 2, dlg.m_strClass);
		m_List.SetItemText(iIndex, 3, dlg.m_strProcess);
		m_List.SetItemText(iIndex, 4, dlg.m_strDesc);
	}
}

void CWindowListDlg::OnBnClickedButtonDel()
{
	int iSel = m_List.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iSel)
	{
		m_List.DeleteItem(iSel);
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	}
}

void CWindowListDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iSel = m_List.GetNextItem(-1, LVIS_SELECTED);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(-1 != iSel ? TRUE : FALSE);
	*pResult = 0;
}

void CWindowListDlg::OnBnClickedOk()
{
	m_vecItems.clear();
	for (int i = 0; i < m_List.GetItemCount(); i++)
	{
		CONFIG_INFO_ITEM item;
		DWORD iType = (DWORD)m_List.GetItemData(i);
		item.iType = iType;
		m_List.GetItemText(i, 1, item.szWindow, MAX_PATH);
		m_List.GetItemText(i, 2, item.szClass, MAX_PATH);
		m_List.GetItemText(i, 3, item.szProcess, MAX_PATH);
		m_List.GetItemText(i, 4, item.szDesc, MAX_PATH);
		m_vecItems.push_back(item);
	}

	OnOK();
}

void CWindowListDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iSel = m_List.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iSel)
	{
		CWindowDlg dlg;
		dlg.m_bDisableExcept = m_bDisableExcept;
		dlg.m_iType = (INT)m_List.GetItemData(iSel);
		dlg.m_strWindow = m_List.GetItemText(iSel, 1);
		dlg.m_strClass = m_List.GetItemText(iSel, 2);
		dlg.m_strProcess = m_List.GetItemText(iSel, 3);
		dlg.m_strDesc = m_List.GetItemText(iSel, 4);
		if (IDOK == dlg.DoModal())
		{
			m_List.SetItemData(iSel, dlg.m_iType);
			m_List.SetItemText(iSel, 0, CONFIG_INFO_ITEM::GetTypeName(dlg.m_iType));
			m_List.SetItemText(iSel, 1, dlg.m_strWindow);
			m_List.SetItemText(iSel, 2, dlg.m_strClass);
			m_List.SetItemText(iSel, 3, dlg.m_strProcess);
			m_List.SetItemText(iSel, 4, dlg.m_strDesc);
		}
	}
	*pResult = 0;
}

void CWindowListDlg::OnBnClickedButtonClean()
{
	m_List.DeleteAllItems();
}

void CWindowListDlg::OnBnClickedButtonImport()
{
	CFileDialog dlg(TRUE, NULL, _T("Win.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text File(*.txt)|*.txt||"));
	if (IDOK == dlg.DoModal())
	{
		FILE *fp = _tfopen(dlg.m_ofn.lpstrFile, _T("r"));
		if (NULL == fp)
		{
			AfxMessageBox(_T("打开文件失败!"), MB_ICONERROR);
			return;
		}
		m_List.DeleteAllItems();
		USES_CONVERSION;
		while (!feof(fp))
		{
			CHAR szBuf[MAX_PATH] = {0};
			fgets(szBuf, MAX_PATH, fp);
			CString strBuf = CA2T(szBuf).m_psz;
			std::vector<CString> vecFields = Utils::SplitFull(strBuf, _T("\t"));
			if (5 == vecFields.size())
			{
				CONFIG_INFO_ITEM item;
				item.iType = _tstoi(vecFields[0]);
				lstrcpy(item.szWindow, vecFields[1]);
				lstrcpy(item.szClass, vecFields[2]);
				lstrcpy(item.szProcess, vecFields[3]);
				lstrcpy(item.szDesc, vecFields[4]);

				int iSel = m_List.InsertItem(m_List.GetItemCount(), _T(""));
				CString strType = item.GetTypeName();
				m_List.SetItemData(iSel, item.iType);
				m_List.SetItemText(iSel, 0, strType);
				m_List.SetItemText(iSel, 1, item.szWindow);
				m_List.SetItemText(iSel, 2, item.szClass);
				m_List.SetItemText(iSel, 3, item.szProcess);
				m_List.SetItemText(iSel, 4, item.szDesc);
			}
		}
		fclose(fp);
		AfxMessageBox(_T("导入成功！"), MB_ICONINFORMATION);
	}
}

void CWindowListDlg::OnBnClickedButtonExport()
{
	CFileDialog dlg(FALSE, NULL, _T("Win.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text File(*.txt)|*.txt||"));
	if (IDOK == dlg.DoModal())
	{
		FILE *fp = _tfopen(dlg.m_ofn.lpstrFile, _T("w"));
		if (NULL == fp)
		{
			AfxMessageBox(_T("打开文件失败!"), MB_ICONERROR);
			return;
		}
		USES_CONVERSION;
		for (int i = 0; i < m_List.GetItemCount(); i++)
		{
			CONFIG_INFO_ITEM item;
			item.iType = (DWORD)m_List.GetItemData(i);
			m_List.GetItemText(i, 1, item.szWindow, MAX_PATH);
			m_List.GetItemText(i, 2, item.szClass, MAX_PATH);
			m_List.GetItemText(i, 3, item.szProcess, MAX_PATH);
			m_List.GetItemText(i, 4, item.szDesc, MAX_PATH);
			CString strBuf;
			strBuf.Format(_T("%d\t%s\t%s\t%s\t%s"), item.iType, item.szWindow, item.szClass, item.szProcess, item.szDesc);
			std::string strBufField = CT2A(strBuf).m_psz;
			fprintf(fp, "%s\n", strBufField.c_str());
		}
		fclose(fp);
		AfxMessageBox(_T("导出成功！"), MB_ICONINFORMATION);
	}
}
