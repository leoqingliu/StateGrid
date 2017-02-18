// ConnectConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientOp.h"
#include "ConnectConfigDlg.h"
#include "../SafeExt/resource.h"
#include "../Public/crypt.h"
#include "../Defines.h"
#define DEFAULT_PORT 80

// CConnectConfigDlg dialog

IMPLEMENT_DYNAMIC(CConnectConfigDlg, CDialog)

CConnectConfigDlg::CConnectConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConnectConfigDlg::IDD, pParent)
{

}

CConnectConfigDlg::~CConnectConfigDlg()
{
}

void CConnectConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConnectConfigDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_PORT_DEFAULT, &CConnectConfigDlg::OnBnClickedRadioPortDefault)
	ON_BN_CLICKED(IDC_RADIO_PORT_USER, &CConnectConfigDlg::OnBnClickedRadioPortUser)
	ON_BN_CLICKED(IDOK, &CConnectConfigDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDCANCEL, &CConnectConfigDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CConnectConfigDlg message handlers

BOOL CConnectConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	if(!ReadConfig())
	{
		AfxMessageBox(_T("读取文件失败，程序将退出"), MB_ICONERROR);
		PostQuitMessage(0);
	}

	//
	if(m_ConnectInfo.uPort == DEFAULT_PORT)
	{
		OnBnClickedRadioPortDefault();
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_PORT, m_ConnectInfo.uPort);
		OnBnClickedRadioPortUser();
	}

//	SetDlgItemInt(IDC_EDIT_ID, m_ConnectInfo.dwId);
	USES_CONVERSION;
	SetDlgItemText(IDC_EDIT_PASSWD, A2T(m_ConnectInfo.szKey));
	SetDlgItemText(IDC_EDIT_PAGE, A2T(m_ConnectInfo.szPage));
	SetDlgItemText(IDC_EDIT_DYNAMIC, A2T(m_ConnectInfo.szHost));
	SetDlgItemText(IDC_EDIT_COMMENT, A2T(m_ConnectInfo.szComment));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConnectConfigDlg::ReadConfig()
{
	BOOL bOK = FALSE;
	if(m_PEHeader.InitiateHeader((LPTSTR)(LPCTSTR)m_strTagerPath, TRUE, L"\"BIN\"", IDR_BIN_CONFIG))
	{
		if(m_PEHeader.m_bFind)
		{
			if(m_PEHeader.m_dwLen == sizeof(CommunicationInfo))
			{
				LPVOID	pStart = LPVOID((LPSTR)m_PEHeader.pBasePointer + m_PEHeader.m_dwStartAddress);
				DWORD dwRead = 0;
				LPBYTE pBuf = new BYTE[sizeof(CommunicationInfo)];
				memcpy(pBuf, pStart, m_PEHeader.m_dwLen);
				Crypt::_rc4Full(CONFIG_KEY, strlen(CONFIG_KEY), pBuf, sizeof(CommunicationInfo));
				memcpy(&m_ConnectInfo, pBuf, m_PEHeader.m_dwLen);
				//
				bOK = TRUE;
			}
			
// 			LPSTR	pSelData = new CHAR[m_PEHeader.m_dwLen + 2];
// 			LPVOID	pStart = LPVOID((LPSTR)m_PEHeader.pBasePointer + m_PEHeader.m_dwStartAddress);
// 			memcpy(pSelData, pStart, m_PEHeader.m_dwLen);
// 
// 			m_bIsDynamic = pSelData[0] == 0;
// 			memcpy(m_szBuffer, pSelData + 1, m_PEHeader.m_dwLen - 1);
// 
// 			delete[] pSelData;
//			bOK = TRUE;
		}
	}
	else
	{
		DWORD dwError = GetLastError();

		int i = 0;
	}
	return bOK;
}

BOOL CConnectConfigDlg::WriteConfig()
{
	LPSTR pBuffer = (LPSTR)m_PEHeader.pBasePointer + m_PEHeader.m_dwStartAddress;
	
	LPBYTE pBuf = new BYTE[sizeof(CommunicationInfo)];
	memcpy(pBuf, &m_ConnectInfo, sizeof(CommunicationInfo));
	Crypt::_rc4Full(CONFIG_KEY, strlen(CONFIG_KEY), pBuf, sizeof(CommunicationInfo));
	memcpy(pBuffer, pBuf, sizeof(CommunicationInfo));
	delete[] pBuf;

	//memcpy(pBuffer, &m_ConnectInfo, sizeof(CommunicationInfo));

	/*
	LPSTR pBuffer = (LPSTR)m_PEHeader.pBasePointer + m_PEHeader.m_dwStartAddress;
	for(DWORD i = 0; i < m_PEHeader.m_dwLen; i++)
	{
		*pBuffer++ = m_szBuffer[i];
	}
	*/
	return TRUE;
}

void CConnectConfigDlg::OnBnClickedRadioPortDefault()
{
	CheckDlgButton(IDC_RADIO_PORT_DEFAULT, BST_CHECKED);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
}

void CConnectConfigDlg::OnBnClickedRadioPortUser()
{
	CheckDlgButton(IDC_RADIO_PORT_USER, BST_CHECKED);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
}


void CConnectConfigDlg::OnBnClickedBtnOk()
{
	// Dynamic
	BOOL bTrans = FALSE;
	TCHAR szBuffer[128] = {0};
	::GetDlgItemText(m_hWnd, IDC_EDIT_DYNAMIC, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入动态域名!"), MB_ICONERROR);
		return;
	}
	USES_CONVERSION;
	strcpy(m_ConnectInfo.szHost, T2A(szBuffer));

	// Page
	::GetDlgItemText(m_hWnd, IDC_EDIT_PAGE, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入页面!"), MB_ICONERROR);
		return;
	}
	strcpy(m_ConnectInfo.szPage, T2A(szBuffer));

	// Comment
	::GetDlgItemText(m_hWnd, IDC_EDIT_COMMENT, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入备注!"), MB_ICONERROR);
		return;
	}
	strcpy(m_ConnectInfo.szComment, T2A(szBuffer));

	// Port
	if(BST_CHECKED == IsDlgButtonChecked(IDC_RADIO_PORT_DEFAULT))
	{
		m_ConnectInfo.uPort = DEFAULT_PORT;
	}
	else
	{
		m_ConnectInfo.uPort = GetDlgItemInt(IDC_EDIT_PORT, &bTrans, FALSE);
	}

	// Id
	//	m_ConnectInfo.dwId = GetDlgItemInt(IDC_EDIT_ID, &bTrans, FALSE);
	m_ConnectInfo.dwId = 0;
	// Password
	szBuffer[0] = 0;
	::GetDlgItemText(m_hWnd, IDC_EDIT_PASSWD, szBuffer, 128);
	if(lstrlen(szBuffer) == 0)
	{
		AfxMessageBox(_T("请输入密码!"), MB_ICONERROR);
		return;
	}
	strcpy(m_ConnectInfo.szKey, T2A(szBuffer));

	//
	if(WriteConfig())
	{
		m_PEHeader.UnInit();
		OnOK();
		return;
	}

	AfxMessageBox(_T("保存配置失败！"), MB_ICONINFORMATION);
	m_PEHeader.UnInit();
	OnCancel();
}

void CConnectConfigDlg::OnBnClickedCancel()
{
	OnCancel();
}
