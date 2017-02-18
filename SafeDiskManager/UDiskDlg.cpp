// UDiskDlg.cpp : implementation file
//

#include "stdafx.h"
#include <resource.h>
#include "UDiskDlg.h"
#include "../Public/WMIUtils.h"
#include "../Public/DevUtils.h"

// CUDiskDlg dialog

IMPLEMENT_DYNAMIC(CUDiskDlg, CDialog)

CUDiskDlg::CUDiskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUDiskDlg::IDD, pParent)
	, m_strUDiskId(_T(""))
{

}

CUDiskDlg::~CUDiskDlg()
{
}

void CUDiskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUDiskId);
	DDX_Control(pDX, IDC_LIST_AUTH, m_ListAuth);
}


BEGIN_MESSAGE_MAP(CUDiskDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CUDiskDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CUDiskDlg::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDOK, &CUDiskDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CUDiskDlg::OnNMClickList)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_MOVE_WHITE, &CUDiskDlg::OnBnClickedButtonMoveWhite)
	ON_NOTIFY(NM_CLICK, IDC_LIST_AUTH, &CUDiskDlg::OnNMClickListAuth)
	ON_BN_CLICKED(IDC_BUTTON_INPORT, &CUDiskDlg::OnBnClickedButtonInport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CUDiskDlg::OnBnClickedButtonExport)
END_MESSAGE_MAP()


// CUDiskDlg message handlers

BOOL CUDiskDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	m_List.InsertColumn(0, _T("USB存储设备序列号"),	LVCFMT_LEFT, 250);
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_List.SetExtendedStyle(dwStyle);
	
	m_ListAuth.InsertColumn(0, _T("USB存储设备序列号"),	LVCFMT_LEFT, 250);
	dwStyle = m_ListAuth.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_ListAuth.SetExtendedStyle(dwStyle);

	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_MOVE_WHITE)->EnableWindow(FALSE);

	std::vector<CString> vecIds;
	BOOL bRet = CDevUtils::GetUDisk(vecIds);
	for (int j = 0; j < (int)vecIds.size(); j++)
	{
		INT iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
		m_List.SetItemText(iIndex, 0, vecIds[j]);
	}

	for (int i = 0; i < (int)m_vecItems.size(); i++)
	{
		INT iIndex = m_ListAuth.InsertItem(m_ListAuth.GetItemCount(), _T(""));
		m_ListAuth.SetItemText(iIndex, 0, m_vecItems[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUDiskDlg::OnBnClickedButtonAdd()
{
	UpdateData();
	if (m_strUDiskId.IsEmpty())
	{
		AfxMessageBox(_T("输入不能为空！"), MB_ICONERROR);
		return;
	}

	BOOL bMatched = FALSE;
	for (int i = 0; i < m_ListAuth.GetItemCount(); i++)
	{
		CString strId = m_ListAuth.GetItemText(i, 0);
		if (0 == m_strUDiskId.CompareNoCase(strId))
		{
			bMatched = TRUE;
			break;
		}
	}
	if (!bMatched)
	{
		INT iIndex = m_ListAuth.InsertItem(m_ListAuth.GetItemCount(), _T(""));
		m_ListAuth.SetItemText(iIndex, 0, m_strUDiskId);
	}

	m_strUDiskId = _T("");
	UpdateData(FALSE);
}

void CUDiskDlg::OnBnClickedButtonDel()
{
	int iSel = -1;
	std::vector<int> vecIndex;
	while (TRUE)
	{
		iSel = m_ListAuth.GetNextItem(iSel, LVIS_SELECTED);
		if (-1 == iSel)
		{
			break;
		}

		vecIndex.push_back(iSel);
	}

	for (int i = (int)vecIndex.size() - 1; i >= 0; i--)
	{
		m_ListAuth.DeleteItem(i);
	}

	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
}

void CUDiskDlg::OnBnClickedOk()
{
//	OnOK();
	PostMessage(WM_CLOSE);
}

void CUDiskDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iSel = m_List.GetNextItem(-1, LVIS_SELECTED);
	GetDlgItem(IDC_BUTTON_MOVE_WHITE)->EnableWindow(-1 != iSel ? TRUE : FALSE);
	*pResult = 0;
}

void CUDiskDlg::OnClose()
{
	m_vecItems.clear();
	for (int i = 0; i < m_ListAuth.GetItemCount(); i++)
	{
		CString strId = m_ListAuth.GetItemText(i, 0);
		m_vecItems.push_back(strId);
	}
	CDialog::OnClose();
}

void CUDiskDlg::OnBnClickedButtonMoveWhite()
{
	int iSel = -1;
	while (TRUE)
	{
		iSel = m_List.GetNextItem(iSel, LVIS_SELECTED);
		if (-1 == iSel)
		{
			break;
		}
		CString strUDiskId = m_List.GetItemText(iSel, 0);
		BOOL bMatched = FALSE;
		for (int i = 0; i < m_ListAuth.GetItemCount(); i++)
		{
			CString strId = m_ListAuth.GetItemText(i, 0);
			if (0 == strUDiskId.CompareNoCase(strId))
			{
				bMatched = TRUE;
				break;
			}
		}
		if (!bMatched)
		{
			INT iIndex = m_ListAuth.InsertItem(m_ListAuth.GetItemCount(), _T(""));
			m_ListAuth.SetItemText(iIndex, 0, strUDiskId);
		}
	}
	GetDlgItem(IDC_BUTTON_MOVE_WHITE)->EnableWindow(FALSE);
}

void CUDiskDlg::OnNMClickListAuth(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iSel = m_ListAuth.GetNextItem(-1, LVIS_SELECTED);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(-1 != iSel ? TRUE : FALSE);
	*pResult = 0;
}

void CUDiskDlg::OnBnClickedButtonInport()
{
	CFileDialog dlg(TRUE, NULL, _T("UDisk.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text File(*.txt)|*.txt||"));
	if (IDOK == dlg.DoModal())
	{
		FILE *fp = _tfopen(dlg.m_ofn.lpstrFile, _T("r"));
		if (NULL == fp)
		{
			AfxMessageBox(_T("打开文件失败!"), MB_ICONERROR);
			return;
		}
		m_ListAuth.DeleteAllItems();
		USES_CONVERSION;
		while (!feof(fp))
		{
			TCHAR szBuf[MAX_PATH] = {0};
#ifdef UNICODE
			fgetws(szBuf, MAX_PATH, fp);
#else
			fgets(szBuf, MAX_PATH, fp);
#endif
			CString strBuf = szBuf;
			strBuf.Trim();
			if (!strBuf.IsEmpty())
			{
				int iIndex = m_ListAuth.InsertItem(m_ListAuth.GetItemCount(), _T(""));
				m_ListAuth.SetItemText(iIndex, 0, strBuf);
			}
		}
		fclose(fp);
		AfxMessageBox(_T("导入成功！"), MB_ICONINFORMATION);
	}
}

void CUDiskDlg::OnBnClickedButtonExport()
{
	CFileDialog dlg(FALSE, NULL, _T("UDisk.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text File(*.txt)|*.txt||"));
	if (IDOK == dlg.DoModal())
	{
		FILE *fp = _tfopen(dlg.m_ofn.lpstrFile, _T("w"));
		if (NULL == fp)
		{
			AfxMessageBox(_T("打开文件失败!"), MB_ICONERROR);
			return;
		}
		USES_CONVERSION;
		for (int i = 0; i < m_ListAuth.GetItemCount(); i++)
		{
			CString strUrl = m_ListAuth.GetItemText(i, 0);
			std::string strBuf = CT2A(strUrl).m_psz;
			fprintf(fp, "%s\n", strBuf.c_str());
		}
		fclose(fp);
		AfxMessageBox(_T("导出成功！"), MB_ICONINFORMATION);
	}
}
