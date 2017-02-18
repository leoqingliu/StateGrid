// UrlDialog.cpp : implementation file
//

#include "stdafx.h"
#include <resource.h>
#include "UrlDialog.h"


// CUrlDialog dialog

IMPLEMENT_DYNAMIC(CUrlDialog, CDialog)

CUrlDialog::CUrlDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUrlDialog::IDD, pParent)
	, m_strUrl(_T(""))
	, m_bCheckAutoAddHttp(FALSE)
{

}

CUrlDialog::~CUrlDialog()
{
}

void CUrlDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_URL, m_strUrl);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Check(pDX, IDC_CHECK_ADD_HTTP, m_bCheckAutoAddHttp);
}


BEGIN_MESSAGE_MAP(CUrlDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CUrlDialog::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &CUrlDialog::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CUrlDialog::OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CUrlDialog::OnBnClickedButtonClean)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CUrlDialog::OnBnClickedButtonExport)
	ON_EN_CHANGE(IDC_EDIT_URL, &CUrlDialog::OnEnChangeEditUrl)
	ON_BN_CLICKED(IDOK, &CUrlDialog::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CUrlDialog::OnNMClickList)
END_MESSAGE_MAP()


// CUrlDialog message handlers

BOOL CUrlDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	m_List.InsertColumn(0, _T("URL"),	LVCFMT_LEFT, 500);
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_List.SetExtendedStyle(dwStyle);
	for (int i = 0; i < (int)m_vecUrls.size(); i++)
	{
		INT iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
		m_List.SetItemText(iIndex, 0, m_vecUrls[i]);
	}
	
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUrlDialog::OnBnClickedButtonAdd()
{
	UpdateData();
	if (m_strUrl.IsEmpty())
	{
		AfxMessageBox(_T("输入不能为空!"), MB_ICONERROR);
		return;
	}

	if (m_bCheckAutoAddHttp)
	{
		if (m_strUrl.Left(7) != _T("http://") && m_strUrl.Left(8) != _T("https://"))
		{
			m_strUrl = _T("http://") + m_strUrl;
		}
	}
	else
	{
		if (m_strUrl.Left(7) == _T("http://"))
		{
			m_strUrl = m_strUrl.Right(m_strUrl.GetLength() - 7);
		}
	}

	int iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
	m_List.SetItemText(iIndex, 0, m_strUrl);

	SetDlgItemText(IDC_EDIT_URL, _T(""));
}

void CUrlDialog::OnBnClickedButtonDel()
{
	int iSel = m_List.GetNextItem(-1, LVIS_SELECTED);
	if (-1 != iSel)
	{
		m_List.DeleteItem(iSel);
		GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(FALSE);
	}
}

void CUrlDialog::OnBnClickedButtonClean()
{
	m_List.DeleteAllItems();
}

void CUrlDialog::OnBnClickedButtonImport()
{
	CFileDialog dlg(TRUE, NULL, _T("Url.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text File(*.txt)|*.txt||"));
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
			/*
			TCHAR szBuf[MAX_PATH] = {0};
#ifdef UNICODE
			fgetws(szBuf, MAX_PATH, fp);
#else
			fgets(szBuf, MAX_PATH, fp);
#endif
			CString strBuf = szBuf;
			*/
			CHAR szBuf[MAX_PATH] = {0};
			fgets(szBuf, MAX_PATH, fp);
			CString strBuf = CA2T(szBuf).m_psz;
			strBuf.Trim();
			if (!strBuf.IsEmpty())
			{
				int iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
				m_List.SetItemText(iIndex, 0, strBuf);
			}
		}
		fclose(fp);
		AfxMessageBox(_T("导入成功！"), MB_ICONINFORMATION);
	}
}

void CUrlDialog::OnBnClickedButtonExport()
{
	CFileDialog dlg(FALSE, NULL, _T("Url.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Text File(*.txt)|*.txt||"));
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
			CString strUrl = m_List.GetItemText(i, 0);
			std::string strBuf = CT2A(strUrl).m_psz;
			fprintf(fp, "%s\n", strBuf.c_str());
		}
		fclose(fp);
		AfxMessageBox(_T("导出成功！"), MB_ICONINFORMATION);
	}
}

void CUrlDialog::OnEnChangeEditUrl()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	UpdateData();
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(m_strUrl.IsEmpty() ? FALSE : TRUE);
}

void CUrlDialog::OnBnClickedOk()
{
	m_vecUrls.clear();
	for (int i = 0; i < m_List.GetItemCount(); i++)
	{
		CString strUrl = m_List.GetItemText(i, 0);
		m_vecUrls.push_back(strUrl);
	}

	OnOK();
}

void CUrlDialog::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int iSel = m_List.GetNextItem(-1, LVIS_SELECTED);
	GetDlgItem(IDC_BUTTON_DEL)->EnableWindow(-1 != iSel ? TRUE : FALSE);
	*pResult = 0;
}
