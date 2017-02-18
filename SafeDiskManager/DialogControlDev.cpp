// ControlDevDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControlDev.h"
#include "WindowListDlg.h"
#include "ChildFrm.h"

// CControlDevDlg dialog

IMPLEMENT_DYNAMIC(CDialogControlDev, CDialog)

CDialogControlDev::CDialogControlDev(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogControlDev::IDD, pParent)
{

}

CDialogControlDev::~CDialogControlDev()
{
}

void CDialogControlDev::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_DEVICE_CELL,		m_bCheckDeviceCell);
	DDX_Check(pDX, IDC_CHECK_PHONE_ASSIST,		m_bCheckPhoneAssist);
	DDX_Check(pDX, IDC_CHECK_NETWORK_WIRED,		m_bCheckNetworkWired);
	DDX_Check(pDX, IDC_CHECK_NETWORK_WIRELESS,	m_bCheckNetworkWireless);
	DDX_Check(pDX, IDC_CHECK_NETWORK_MINI,		m_bCheckNetworkMini);
	DDX_Check(pDX, IDC_CHECK_BLUETOOTH,			m_bCheckBlueTooth);
	DDX_Check(pDX, IDC_CHECK_SERIAL,			m_bCheckSerial);
	DDX_Check(pDX, IDC_CHECK_1394,				m_bCheck1394);
	DDX_Check(pDX, IDC_CHECK_IRDA,				m_bCheckIrda);
	DDX_Check(pDX, IDC_CHECK_PCMCIA,			m_bCheckPcmcia);
	DDX_Check(pDX, IDC_CHECK_MODEM,				m_bCheckModem);
}


BEGIN_MESSAGE_MAP(CDialogControlDev, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDialogControlDev::OnBnClickedButtonSave)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CControlDevDlg message handlers

BOOL CDialogControlDev::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);		// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// Set small icon

	m_bConfigUpdated = FALSE;
	UpdateDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDialogControlDev::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)lParam;
	INT iCtrlId = LOWORD(wParam);
	INT iCmdId = HIWORD(wParam);

	if (IDC_CHECK_DEVICE_CELL == iCtrlId		||
		IDC_CHECK_PHONE_ASSIST == iCtrlId		||
		IDC_CHECK_NETWORK_WIRED == iCtrlId		||
		IDC_CHECK_NETWORK_WIRELESS == iCtrlId	||
		IDC_CHECK_NETWORK_MINI == iCtrlId		||
		IDC_CHECK_BLUETOOTH == iCtrlId			||
		IDC_CHECK_SERIAL == iCtrlId				||
		IDC_CHECK_1394 == iCtrlId				||
		IDC_CHECK_IRDA == iCtrlId				||
		IDC_CHECK_PCMCIA == iCtrlId				||
		IDC_CHECK_MODEM == iCtrlId
		)
	{
		UpdateData();
		m_Config.m_bCheckDeviceCell					= m_bCheckDeviceCell;
		m_Config.m_ItemsPhoneAssist.bDisable		= m_bCheckPhoneAssist;
		m_Config.m_ItemsNetworkWired.bDisable		= m_bCheckNetworkWired;
		m_Config.m_ItemsNetworkWireless.bDisable	= m_bCheckNetworkWireless;
		m_Config.m_ItemsNetworkMini.bDisable		= m_bCheckNetworkMini;
		m_Config.m_bCheckBlueTooth					= m_bCheckBlueTooth;
		m_Config.m_bCheckSerial						= m_bCheckSerial;
		m_Config.m_bCheck1394						= m_bCheck1394;
		m_Config.m_bCheckIrda						= m_bCheckIrda;
		m_Config.m_bCheckPcmcia						= m_bCheckPcmcia;
		m_Config.m_bCheckModem						= m_bCheckModem;
		m_bConfigUpdated = TRUE;

	}
	else if (
		IDC_BUTTON_PHONE_ASSIST_OP == iCtrlId	||
		IDC_BUTTON_NETWORK_WIRED == iCtrlId		||
		IDC_BUTTON_NETWORK_WIRELESS == iCtrlId	||
		IDC_BUTTON_NETWORK_MINI == iCtrlId
		)
	{
		CONFIG_INFO_ITEMS *pItems = NULL;
		switch (iCtrlId)
		{
		case IDC_BUTTON_PHONE_ASSIST_OP:
			pItems = &m_Config.m_ItemsPhoneAssist;
			break;
		case IDC_BUTTON_NETWORK_WIRED:
			pItems = &m_Config.m_ItemsNetworkWired;
			break;
		case IDC_BUTTON_NETWORK_WIRELESS:
			pItems = &m_Config.m_ItemsNetworkWireless;
			break;
		case IDC_BUTTON_NETWORK_MINI:
			pItems = &m_Config.m_ItemsNetworkMini;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		std::vector<CONFIG_INFO_ITEM> vecItems = pItems->vecItems;
		CWindowListDlg dlg;
		dlg.m_vecItems = vecItems;
		if (IDOK == dlg.DoModal())
		{
			pItems->vecItems = dlg.m_vecItems;
		}
		m_bConfigUpdated = TRUE;
	}
	
	return CDialog::OnCommand(wParam, lParam);
}

void CDialogControlDev::OnBnClickedButtonSave()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlDev(m_Config);
	AfxMessageBox(_T("±£´æ³É¹¦!"), MB_ICONINFORMATION);
}

VOID CDialogControlDev::UpdateDisplay()
{
	m_bCheckDeviceCell		= m_Config.m_bCheckDeviceCell;
	m_bCheckPhoneAssist		= m_Config.m_ItemsPhoneAssist.bDisable;
	m_bCheckNetworkWired	= m_Config.m_ItemsNetworkWired.bDisable;
	m_bCheckNetworkWireless	= m_Config.m_ItemsNetworkWireless.bDisable;
	m_bCheckNetworkMini		= m_Config.m_ItemsNetworkMini.bDisable;
	m_bCheckBlueTooth		= m_Config.m_bCheckBlueTooth;
	m_bCheckSerial			= m_Config.m_bCheckSerial;
	m_bCheck1394			= m_Config.m_bCheck1394;
	m_bCheckIrda			= m_Config.m_bCheckIrda;
	m_bCheckPcmcia			= m_Config.m_bCheckPcmcia;
	m_bCheckModem			= m_Config.m_bCheckModem;
	UpdateData(FALSE);
}

void CDialogControlDev::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

// 	CRect rect;
// 	GetClientRect(rect);
// 	dc.FillSolidRect(rect, RGB(255, 255, 255));
// 	dc.SetBkMode(TRANSPARENT);
}
