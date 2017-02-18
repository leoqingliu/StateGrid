// ControlSystemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControlSystem.h"
#include "WindowListDlg.h"
#include "ChildFrm.h"

// CControlSystemDlg dialog

IMPLEMENT_DYNAMIC(CDialogControlSystem, CDialog)

CDialogControlSystem::CDialogControlSystem(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogControlSystem::IDD, pParent)
{

}

CDialogControlSystem::~CDialogControlSystem()
{
}

void CDialogControlSystem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_REGEDIT,		m_bCheckRegEdit);
	DDX_Check(pDX, IDC_CHECK_DEVMGR,		m_bCheckDevMgr);
	DDX_Check(pDX, IDC_CHECK_GPEDIT,		m_bCheckGpedit);
	DDX_Check(pDX, IDC_CHECK_TASKMGR,		m_bCheckTaskmgr);
	DDX_Check(pDX, IDC_CHECK_MSCONFIG,		m_bCheckMsconfig);
	DDX_Check(pDX, IDC_CHECK_COMPUTERMGR,	m_bCheckComputermgr);
	DDX_Check(pDX, IDC_CHECK_FORMAT,		m_bCheckFormat);
	DDX_Check(pDX, IDC_CHECK_DOS,			m_bCheckDos);
	DDX_Check(pDX, IDC_CHECK_IP,			m_bCheckIp);
	DDX_Check(pDX, IDC_CHECK_CLIPBOARD,		m_bCheckClipboard);
	DDX_Check(pDX, IDC_CHECK_TELNET,		m_bCheckTelnet);
	DDX_Check(pDX, IDC_CHECK_SHARE,			m_bCheckShare);
	DDX_Check(pDX, IDC_CHECK_LAN,			m_bCheckLan);
	DDX_Check(pDX, IDC_CHECK_VIR,			m_bCheckVir);
	DDX_Check(pDX, IDC_CHECK_CREATEUSER,	m_bCheckCreateUser);
//	DDX_Control(pDX, IDC_CHECK_REGEDIT, m_btnCheckRegEdit);
}


BEGIN_MESSAGE_MAP(CDialogControlSystem, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SYSTEM_SAVE, &CDialogControlSystem::OnBnClickedButtonSystemSave)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_CHECK_REGEDIT, &CDialogControlSystem::OnNMCustomdrawCheckRegedit)
END_MESSAGE_MAP()


// CControlSystemDlg message handlers

BOOL CDialogControlSystem::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_CHECK_CD_BOOT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_U_BOOT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_SAFEMODE)->EnableWindow(FALSE);

	GetDlgItem(IDC_CHECK_KEY_PRTSCN)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_KEY_ESC)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_KEY_WIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_KEY_CTRLALTA)->EnableWindow(FALSE);

	m_bConfigUpdated = FALSE;
	UpdateDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDialogControlSystem::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)lParam;
	INT iCtrlId = LOWORD(wParam);
	INT iCmdId = HIWORD(wParam);

	UINT uIds[] =
	{
		IDC_CHECK_REGEDIT,
		IDC_CHECK_DEVMGR,
		IDC_CHECK_GPEDIT,
		IDC_CHECK_TASKMGR,
		IDC_CHECK_MSCONFIG,
		IDC_CHECK_COMPUTERMGR,
		IDC_CHECK_FORMAT,
		IDC_CHECK_DOS,
		IDC_CHECK_IP,
		IDC_CHECK_CLIPBOARD,
		IDC_CHECK_TELNET,
		IDC_CHECK_SHARE,
		IDC_CHECK_LAN,
		IDC_BUTTON_LAN,
		IDC_CHECK_VIR,
		IDC_CHECK_CREATEUSER
	};
	int i;
	for (i = 0; i < _countof(uIds); i++)
	{
		if (uIds[i] == iCtrlId)
		{
			break;
		}
	}
	if (i != _countof(uIds))
	{
		UpdateData();
		m_Config.m_bCheckRegEdit	= m_bCheckRegEdit;
		m_Config.m_bCheckDevMgr		= m_bCheckDevMgr;
		m_Config.m_bCheckGpedit		= m_bCheckGpedit;
		m_Config.m_bCheckTaskmgr	= m_bCheckTaskmgr;
		m_Config.m_bCheckMsconfig	= m_bCheckMsconfig;
		m_Config.m_bCheckComputermgr= m_bCheckComputermgr;
		m_Config.m_bCheckFormat		= m_bCheckFormat;
		m_Config.m_bCheckDos		= m_bCheckDos;
		m_Config.m_bCheckIp			= m_bCheckIp;
		m_Config.m_bCheckClipboard	= m_bCheckClipboard;
		m_Config.m_bCheckTelnet		= m_bCheckTelnet;
		m_Config.m_bCheckShare		= m_bCheckShare;
		m_Config.m_ItemsLan.bDisable= m_bCheckLan;
		m_Config.m_bCheckVir		= m_bCheckVir;
		m_Config.m_bCheckCreateUser	= m_bCheckCreateUser;
		if (IDC_BUTTON_LAN == iCtrlId)
		{
			std::vector<CONFIG_INFO_ITEM> vecItems = m_Config.m_ItemsLan.vecItems;
			CWindowListDlg dlg;
			dlg.m_vecItems = vecItems;
			if (IDOK == dlg.DoModal())
			{
				m_Config.m_ItemsLan.vecItems = dlg.m_vecItems;
			}
		}
		m_bConfigUpdated = TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDialogControlSystem::OnBnClickedButtonSystemSave()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlSystem(m_Config);
	AfxMessageBox(_T("±£´æ³É¹¦!"), MB_ICONINFORMATION);
}

VOID CDialogControlSystem::UpdateDisplay()
{
	m_bCheckRegEdit		= m_Config.m_bCheckRegEdit;
	m_bCheckDevMgr		= m_Config.m_bCheckDevMgr;
	m_bCheckGpedit		= m_Config.m_bCheckGpedit;
	m_bCheckTaskmgr		= m_Config.m_bCheckTaskmgr;
	m_bCheckMsconfig	= m_Config.m_bCheckMsconfig;
	m_bCheckComputermgr	= m_Config.m_bCheckComputermgr;
	m_bCheckFormat		= m_Config.m_bCheckFormat;
	m_bCheckDos			= m_Config.m_bCheckDos;
	m_bCheckIp			= m_Config.m_bCheckIp;
	m_bCheckClipboard	= m_Config.m_bCheckClipboard;
	m_bCheckTelnet		= m_Config.m_bCheckTelnet;
	m_bCheckShare		= m_Config.m_bCheckShare;
	m_bCheckLan			= m_Config.m_ItemsLan.bDisable;
	m_bCheckVir			= m_Config.m_bCheckVir;
	m_bCheckCreateUser	= m_Config.m_bCheckCreateUser;
	UpdateData(FALSE);
}

void CDialogControlSystem::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

//  	CRect rect;
//  	GetClientRect(rect);
//  	dc.FillSolidRect(rect, RGB(255, 255, 255));
//  	dc.SetBkMode(TRANSPARENT);

// 	if (IsIconic())
// 	{
// 	}
// 	else
// 	{   
// 		CRect rc;
// 		GetClientRect(&rc);
// 		CPaintDC dc(this);
// 		dc.FillSolidRect(&rc, RGB(255, 255, 255));
// 		CDialog::OnPaint();
// 	}
}

HBRUSH CDialogControlSystem::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

#if 0
	//pWnd->GetDlgCtrlID()
	//if (pWnd->IsKindOf(RUNTIME_CLASS(CButton)))
	//if (/*nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_EDIT || */nCtlColor == CTLCOLOR_BTN)
//	{
//		pDC->SetTextColor(RGB(0x41, 0x96, 0x4F));
//		pDC->SetBkMode(TRANSPARENT);
//		pDC->SetBkColor(RGB(255, 0, 255));
//		return (HBRUSH)::GetStockObject(NULL_BRUSH);
//	}

	BOOL bMatched = FALSE;
	bMatched = TRUE;
	/*
	if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_BTN)
	{
		bMatched = TRUE;
	}
	else
	{
		UINT uIds[] =
		{
			IDC_CHECK_REGEDIT,
			IDC_CHECK_DEVMGR,
			IDC_CHECK_GPEDIT,
			IDC_CHECK_TASKMGR,
			IDC_CHECK_MSCONFIG,
			IDC_CHECK_COMPUTERMGR,
			IDC_CHECK_FORMAT,
			IDC_CHECK_DOS,
			IDC_CHECK_IP,
			IDC_CHECK_CLIPBOARD,
			IDC_CHECK_TELNET,
			IDC_CHECK_SHARE,
			IDC_CHECK_LAN,
			IDC_BUTTON_LAN,
			IDC_CHECK_VIR,
			IDC_CHECK_CREATEUSER
		};
		int iCtrlId = pWnd->GetDlgCtrlID();
		for (int i = 0; i < _countof(uIds); i++)
		{
			if (uIds[i] == iCtrlId)
			{
				bMatched = TRUE;
				break;
			}
		}
	}
	*/
	if (bMatched)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
#endif

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CDialogControlSystem::OnEraseBkgnd(CDC* pDC)
{
	return CDialog::OnEraseBkgnd(pDC);
	//return TRUE;
}

void CDialogControlSystem::OnNMCustomdrawCheckRegedit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	CDC *pDC = CDC::FromHandle(pNMCD->hdc);
	pDC->SetBkMode(TRANSPARENT);
	*pResult = 0;
}
