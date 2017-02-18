// ControlStorageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControlStorage.h"
#include "UDiskDlg.h"
#include "WindowListDlg.h"
#include "LoginModifyDlg.h"
#include "ChildFrm.h"

// CControlStorageDlg dialog

IMPLEMENT_DYNAMIC(CDialogControlStorage, CDialog)

CDialogControlStorage::CDialogControlStorage(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogControlStorage::IDD, pParent)
{

}

CDialogControlStorage::~CDialogControlStorage()
{
}

void CDialogControlStorage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_UDISK_DISABLE, m_uUDiskType);
	DDX_Check(pDX, IDC_CHECK_CD_DVD, m_bCheckCD);
	DDX_Check(pDX, IDC_CHECK_FLOPPY, m_bCheckFloppy);
	DDX_Check(pDX, IDC_CHECK_CD_BURN, m_bCheckCDBurn);
}


BEGIN_MESSAGE_MAP(CDialogControlStorage, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_STORAGE_SAVE, &CDialogControlStorage::OnBnClickedButtonStorageSave)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CControlStorageDlg message handlers

BOOL CDialogControlStorage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bConfigUpdated = FALSE;
	UpdateDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDialogControlStorage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)lParam;
	INT iCtrlId = LOWORD(wParam);
	INT iCmdId = HIWORD(wParam);

	UINT uIds[] =
	{
		IDC_RADIO_UDISK_DISABLE,
		IDC_RADIO_UDISK_SPEC,
		IDC_RADIO_UDISK_WRITEPROTECT,
		IDC_RADIO_UDISK_COPYTOCOMPUTER,
		IDC_RADIO_UDISK_PASSWD,
		IDC_RADIO_UDISK_ENABLE,
		IDC_CHECK_CD_DVD,
		IDC_CHECK_FLOPPY,
		IDC_CHECK_CD_BURN,
		IDC_BUTTON_ADD_U_ID,
		IDC_BUTTON_UDISK_PASSWD,
		IDC_BUTTON_CD_BURN_OP
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
		m_Config.m_uUDiskType			= (UDISK_TYPE)m_uUDiskType;
		m_Config.m_bCheckCD				= m_bCheckCD;
		m_Config.m_bCheckFloppy			= m_bCheckFloppy;
		m_Config.m_ItemsCDBurn.bDisable	= m_bCheckCDBurn;
		if (IDC_BUTTON_ADD_U_ID == iCtrlId)
		{
			CUDiskDlg dlg;
			dlg.m_vecItems = m_Config.m_ItemsUDiskIds.vecItems;
			dlg.DoModal();
			m_Config.m_ItemsUDiskIds.vecItems = dlg.m_vecItems;
		}
		else if (IDC_BUTTON_UDISK_PASSWD == iCtrlId)
		{
			CLoginModifyDlg dlg;
			if (IDOK == dlg.DoModal())
			{
				lstrcpy(m_Config.m_szUDiskPasswd, dlg.m_strPassword);
			}
		}
		else if (IDC_BUTTON_CD_BURN_OP == iCtrlId)
		{
			std::vector<CONFIG_INFO_ITEM> vecItems = m_Config.m_ItemsCDBurn.vecItems;
			CWindowListDlg dlg;
			dlg.m_vecItems = vecItems;
			if (IDOK == dlg.DoModal())
			{
				m_Config.m_ItemsCDBurn.vecItems = dlg.m_vecItems;
			}
		}
			
		m_bConfigUpdated = TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CDialogControlStorage::OnBnClickedButtonStorageSave()
{
	CChildFrame *pChildFrame = (CChildFrame *)GetParentFrame();
	pChildFrame->OnControlStorage(m_Config);
	AfxMessageBox(_T("±£´æ³É¹¦!"), MB_ICONINFORMATION);
}

VOID CDialogControlStorage::UpdateDisplay()
{
	m_uUDiskType = m_Config.m_uUDiskType;
	m_bCheckCD = m_Config.m_bCheckCD;
	m_bCheckFloppy = m_Config.m_bCheckFloppy;
	m_bCheckCDBurn = m_Config.m_ItemsCDBurn.bDisable;
	UpdateData(FALSE);
}

void CDialogControlStorage::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

// 	CRect rect;
// 	GetClientRect(rect);
// 	dc.FillSolidRect(rect, RGB(255, 255, 255));
// 	dc.SetBkMode(TRANSPARENT);
}
