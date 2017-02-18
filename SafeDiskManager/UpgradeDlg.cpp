// UpgradeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "UpgradeDlg.h"
#include "../Defines.h"
#include "ProtoManagerChild.h"

#define TIMER_CHECK_VERSION 100

// CUpgradeDlg dialog

IMPLEMENT_DYNAMIC(CUpgradeDlg, CDialog)

CUpgradeDlg::CUpgradeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpgradeDlg::IDD, pParent)
{

}

CUpgradeDlg::~CUpgradeDlg()
{
}

void CUpgradeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_UPGRADE, m_List);
}


BEGIN_MESSAGE_MAP(CUpgradeDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UPGRADE, &CUpgradeDlg::OnBnClickedButtonUpgrade)
	ON_BN_CLICKED(IDC_BUTTON_UPGRADE_SERVICE, &CUpgradeDlg::OnBnClickedButtonUpgradeService)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_UPGRADE_CONFIG, &CUpgradeDlg::OnBnClickedButtonUpgradeConfig)
END_MESSAGE_MAP()


// CUpgradeDlg message handlers

BOOL CUpgradeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	m_List.InsertColumn(0, _T("IP"),		LVCFMT_LEFT, 100);
	m_List.InsertColumn(1, _T("机器名"),	LVCFMT_LEFT, 100);
	m_List.InsertColumn(2, _T("备注"),		LVCFMT_LEFT, 100);
	m_List.InsertColumn(3, _T("版本"),		LVCFMT_LEFT, 100);
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_SHAREIMAGELISTS|LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP;
	m_List.SetExtendedStyle(dwStyle);

	std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient(TRUE);
	ClientInfo_t *pInfo = NULL;
	for (std::vector<ClientInfo_t>::iterator pIter = vecInfo.begin(); pIter != vecInfo.end(); pIter++)
	{
		int iIndex = m_List.InsertItem(m_List.GetItemCount(), _T(""));
		m_List.SetItemText(iIndex, 0, pIter->szIPOut);
		m_List.SetItemText(iIndex, 1, pIter->szHostname);
		m_List.SetItemText(iIndex, 2, theApp.GenDisplayName(&(*pIter)));
		CString strTmp;
		strTmp.Format(_T("%d.%d"), pIter->dwVersion / 100, pIter->dwVersion % 100);
		m_List.SetItemText(iIndex, 3, strTmp);
		m_List.SetItemData(iIndex, pIter->dwClientId);
	}

	SetTimer(TIMER_CHECK_VERSION, 500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUpgradeDlg::OnBnClickedButtonUpgrade()
{
	CFileDialog dlg(TRUE, NULL, SERVICE_DLL_FILE_NAME, OFN_HIDEREADONLY, _T("Dll File(*.dll)|*.dll||"));
	if (IDOK == dlg.DoModal())
	{
		CString strPath = dlg.m_ofn.lpstrFile;
		int iCount = m_List.GetItemCount();
		for (int i = 0; i < iCount; i++)
		{
			DWORD dwClientId = m_List.GetItemData(i);
			CProtoManager::IDS_UPGRADE_INFO info;
			CString strFileName;
			strFileName.Format(_T("%s.UP"), SERVICE_DLL_FILE_NAME);
			info.strName = strFileName;
			info.strPath = strPath;
			info.dwClientId = dwClientId;
			theProtoManager->m_vecIdsToUpgrade.push_back(info);
		}
	}

	for (std::vector<CProtoManager::IDS_UPGRADE_INFO>::iterator pIter = theProtoManager->m_vecIdsToUpgrade.begin(); pIter != theProtoManager->m_vecIdsToUpgrade.end();)
	{
		File_Transfer_t file;
		file.strLocalPath = pIter->strPath;
		file.strLocalName = pIter->strName;
		file.strRemoteDir = _T("*");
		file.dwClientId = pIter->dwClientId;
		file.bToRemote = TRUE;
		CProtoManagerChild *pChild = theProtoManager->GetChild(pIter->dwClientId);
		if (pChild)
		{
			pChild->InvokeFile(file);
		}

		theProtoManager->m_vecIdsToUpgrade.erase(pIter);
		break;
	}
}

void CUpgradeDlg::OnBnClickedButtonUpgradeService()
{
	int iCount = m_List.GetItemCount();
	for (int i = 0; i < iCount; i++)
	{
		DWORD dwClientId = m_List.GetItemData(i);
		CProtoManagerChild *pChild = theProtoManager->GetChild(dwClientId);
		if (pChild)
		{
			pChild->InvokeReq(BOT_TYPE_UPGRADE);
		}
	}
}

void CUpgradeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_CHECK_VERSION == nIDEvent)
	{
		int iCount = m_List.GetItemCount();
		for (int i = 0; i < iCount; i++)
		{
			DWORD dwClientId = m_List.GetItemData(i);

			std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient(TRUE);
			for (std::vector<ClientInfo_t>::iterator pIter = vecInfo.begin(); pIter != vecInfo.end(); pIter++)
			{
				if (pIter->dwClientId == dwClientId)
				{
					CString strNewVersion;
					strNewVersion.Format(_T("%d.%d"), pIter->dwVersion / 100, pIter->dwVersion % 100);
					CString strOldVersion = m_List.GetItemText(i, 3);
					if (strNewVersion != strOldVersion)
					{
						m_List.SetItemText(i, 3, strNewVersion);
					}

					break;
				}
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CUpgradeDlg::OnBnClickedButtonUpgradeConfig()
{
	int iCount = m_List.GetItemCount();
	for (int i = 0; i < iCount; i++)
	{
		DWORD dwClientId = m_List.GetItemData(i);
		std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(dwClientId);
		if (pIter != theConfig.m_mapClient.end())
		{
			std::string strJsonConfig;
			pIter->second.SaveConfig(strJsonConfig);
			CString strConfig = CA2T(strJsonConfig.c_str()).m_psz;
			theProtoManager->InvokeConfigInfo(dwClientId, strConfig);
		}
	}
	AfxMessageBox(_T("成功!"), MB_ICONINFORMATION);
}
