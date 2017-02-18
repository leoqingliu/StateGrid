
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "MainFrm.h"
#include "ProtoManager.h"
#include "DialogAppConfig.h"
#include "DialogProperties.h"
#include "DialogPolicy.h"
#include "DialogUserLogin.h"
#include "DialogUserManage.h"
#include "DialogUserModify.h"
#include "../Public/crypt.h"
#include "DialogControlDev.h"
#include "DialogControlStorage.h"
#include "DialogControlSystem.h"
#include "ProtoManagerChild.h"
#include "ModifyCommentDlg.h"
#include "UpgradeDlg.h"
#include "PolicyEditDlg.h"
#include "../Public/Conv.h"
#include "../Public/Utils.h"
#include "../Public/GlobalUtils.h"
#include "GroupDialog.h"
#include "GroupListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

#define TIMER_ID_INVOKE_CLIENT		1
#define TIMER_ID_RESPONSE			2
#define TIMER_ID_CLIENT				3
#define TIMER_ID_OUTPUT_MSG			4
#define TIMER_CHECK_HOSTINFO_UPDATE	5
#define TIMER_CHECK_ONLINE			6

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CHILD_CLOSE, &CMainFrame::OnChildClose)
	ON_COMMAND(ID_APP_CONFIG, &CMainFrame::OnAppConfig)
	ON_COMMAND(ID_INDICATOR_IP, &CMainFrame::OnStatusBarCommand)
	ON_COMMAND(ID_INDICATOR_LAST_LOGIN, &CMainFrame::OnStatusBarCommand)
	ON_COMMAND(ID_INDICATOR_PATH, &CMainFrame::OnStatusBarCommand)
	ON_COMMAND(ID_INDICATOR_SIZE, &CMainFrame::OnStatusBarCommand)
	ON_WM_TIMER()
	ON_COMMAND(ID_APP_USER_MODIFY, &CMainFrame::OnAppUserModify)
	ON_COMMAND(ID_APP_USER_MANAGE, &CMainFrame::OnAppUserManage)
	ON_WM_INITMENUPOPUP()
	ON_UPDATE_COMMAND_UI(ID_VIEW_DEFAULT_ALLOW, &CMainFrame::OnUpdateViewDefaultAllow)
	ON_COMMAND(ID_VIEW_DEFAULT_ALLOW, &CMainFrame::OnViewDefaultAllow)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器

	ID_INDICATOR_IP,
	ID_INDICATOR_LAST_LOGIN,
	ID_INDICATOR_PATH,
	ID_INDICATOR_SIZE,

	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);
	theApp.m_bDefaultAllow = theApp.GetInt(_T("DefaultAllow"), FALSE);

	m_hWndCurrentChild = NULL;
	m_iCurrentTab = 0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);
	
//	EnableLoadDockState(FALSE);

	/*
	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE;	// 其他可用样式...
	mdiTabParams.m_tabLocation = CMFCTabCtrl::LOCATION_BOTTOM;
	mdiTabParams.m_bActiveTabCloseButton = TRUE;			// 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;						// 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = FALSE;						// 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = FALSE;					// 在选项卡区域的右边缘启用文档菜单
	mdiTabParams.m_nTabBorderSize = 0;
	EnableMDITabbedGroups(TRUE, mdiTabParams);
	*/

	if (!m_wndMenuBar.Create(this))
	{
		AfxMessageBox(_T("未能创建菜单栏"), MB_ICONERROR);
		return -1;
	}

	//m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI);

	//if (!m_WeatherToolBar.Create(this))
	//{
	//	AfxMessageBox(_T("未能创建工具栏"), MB_ICONERROR);
	//	return -1;
	//}
	//m_WeatherToolBar.SetPaneStyle(m_WeatherToolBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_FLOAT_MULTI);


	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	// 允许用户定义的工具栏操作:
	InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneWidth(0, 100);
	m_wndStatusBar.SetPaneWidth(INDEX_STATUS_PANE_IP,			200);
	m_wndStatusBar.SetPaneWidth(INDEX_STATUS_PANE_LAST_LOGIN,	200);
	m_wndStatusBar.SetPaneWidth(INDEX_STATUS_PANE_PATH,			350);
	m_wndStatusBar.SetPaneWidth(INDEX_STATUS_PANE_SIZE,			100);
//	m_wndStatusBar.SetFont(&afxGlobalData.fontRegular);
	m_wndStatusBar.SetFont(&theApp.m_Font);

	EnableDocking(CBRS_ALIGN_ANY);

	// 创建停靠窗口
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}

	m_wndTopTitleView.EnableDocking(CBRS_ALIGN_TOP);
	GetDockingManager()->AddPane(&m_wndTopTitleView);
	//DockPane(&m_wndTopTitleView);
	//CDockingManager::AddPane(&m_wndTopTitleView);
	//CDockingManager* pDockManager = afxGlobalUtils.GetDockingManager(this);
	//ASSERT_VALID(pDockManager);
	//pDockManager->AddPane(&m_wndTopTitleView);

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_TOP);
	DockPane(&m_wndMenuBar);

	m_wndNavView.EnableDocking(CBRS_ALIGN_TOP);
	GetDockingManager()->AddPane(&m_wndNavView);

	/*
	m_wndWeatherView.EnableDocking(CBRS_ALIGN_TOP);
	//DockPane(&m_WeatherToolBar);
	GetDockingManager()->AddPane(&m_wndWeatherView);
	UpdateTopWeather();
	*/


	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_TOP);

	// 加载菜单项图像(不在任何标准工具栏上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	m_wndConnectView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndConnectView);
	m_wndLog.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndLog);
	
	//GetDockingManager()->DisableRestoreDockState(TRUE);

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	/*
	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			m_UserImages.SetImageSize(CSize(16, 16), FALSE);
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}
	*/

	/*
	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_APP_CONFIG);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_SORTING_SORTALPHABETIC);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYTYPE);
	lstBasicCommands.AddTail(ID_SORTING_SORTBYACCESS);
	lstBasicCommands.AddTail(ID_SORTING_GROUPBYTYPE);
	CMFCToolBar::SetBasicCommands(lstBasicCommands);
	*/

	m_LogImageList.Create(16, 16, ILC_COLOR4|ILC_MASK, 0, 0);
	m_LogImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_INFO));
	m_LogImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_ERROR));
	
	m_wndLog.m_List.InsertColumn(0, _T(""),		LVCFMT_LEFT, 24);
	m_wndLog.m_List.InsertColumn(1, _T("行数"),	LVCFMT_LEFT, 50);
	m_wndLog.m_List.InsertColumn(2, _T("时间"),	LVCFMT_LEFT, 150);
	m_wndLog.m_List.InsertColumn(3, _T("信息"),	LVCFMT_LEFT, 600);
	m_wndLog.m_List.SetImageList(&m_LogImageList, LVSIL_SMALL);

	// Set Find Flag
	::SetProp(m_hWnd, FIND_FLAG, (HANDLE)1);

	//
	OutputMessage(ICON_TYPE_INFO, _T("程序启动..."));

	// Init Root Item
	HTREEITEM hRoot = GetConnectTree().InsertItem(_T("主机列表"), IMAGE_ROOT, IMAGE_ROOT);
	GetConnectTree().SetItemData(hRoot, 0);
	// Init Group
	m_hTreeDefault = GetConnectTree().InsertItem(_T("默认"), hRoot);
	for (std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.m_vecGroup.begin(); pIter != theConfig.m_vecGroup.end(); pIter++)
	{
		GetConnectTree().InsertItem(pIter->strName, hRoot);
	}
	GetConnectTree().Expand(hRoot, TVE_EXPAND);

	SetTimer(TIMER_ID_INVOKE_CLIENT, 1000, NULL);
	SetTimer(TIMER_ID_RESPONSE, 50, NULL);
	SetTimer(TIMER_ID_OUTPUT_MSG, 50, NULL);
	SetTimer(TIMER_ID_CLIENT, 1000 * 3, NULL);
	SetTimer(TIMER_CHECK_HOSTINFO_UPDATE, 200, NULL);
	SetTimer(TIMER_CHECK_ONLINE, 1000 * 2, NULL);

#if 0
	std::vector<ClientInfo> vecInfo;
	theProtoManager->GetClient(vecInfo);

	// Init Client
	for(size_t i = 0; i < vecInfo.size(); i++)
	{
		ClientInfo& Info = vecInfo[i];
		TCHAR szBuffer[128] = {0};
		wsprintf(szBuffer, _T("%d"), Info.dwClientId);
		HTREEITEM hAfter = GetConnectTree().InsertItem(szBuffer, IMAGE_DISCONNECTED, IMAGE_DISCONNECTED, hRoot);
		GetConnectTree().SetItemData(hAfter, (DWORD_PTR)Info.dwClientId);
	}
	GetConnectTree().Expand(hRoot, TVE_EXPAND);
#else
	//theProtoManager->InvokeClient();
	//theProtoManager->InvokeFileUpDownInfo();
	theRelayUpDownInfo.bUpdated = TRUE;
	theRelayUpDownInfo.bFileTrans = TRUE;
	lstrcpy(theRelayUpDownInfo.szUpalodScript, _T("/safe/upload.php"));
	lstrcpy(theRelayUpDownInfo.szUpalodDir, _T("/safe/data/"));
#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// un-comment below will don't show "a-b" format
	cs.style &= ~FWS_ADDTOTITLE;

	BOOL bNameValid;
	bNameValid = m_strTitle.LoadString(IDS_TITLE);
	ASSERT(bNameValid);

	if (!CMDIFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	// Set Title
	/*
	BOOL bNameValid;
	CString strVersion;
	bNameValid = strVersion.LoadString(IDS_VERSION);
	ASSERT(bNameValid);
	m_strTitle = _T("SafeDisk Manager, Version: ") + strVersion;
	*/
	//m_strTitle = _T("SafeDisk Manager");
	//m_strTitle = _T("大势至-国家电网内网管理系统");
	//m_strTitle = _T("国家电网内网管理系统");
	//m_strTitle = _T("国家电网公司远程运维专区管理系统 - V3.0");

	return TRUE;
}

int CMainFrame::CloseAllChilds()
{
	m_hWndCurrentChild = NULL;
	HWND pChild = GetNextMDIChildWnd();
	while (pChild) 
	{
		::SendMessage(pChild, WM_CLOSE, NULL, NULL);
		pChild = GetNextMDIChildWnd();
	}
	return 0;
}
void CMainFrame::OnClose()
{
	// Remove Find Flag
	::RemoveProp(m_hWnd, FIND_FLAG);

	// Close Childs
	CloseAllChilds();

	// Close
	CMDIFrameWndEx::OnClose();
}

void CMainFrame::OnStatusBarCommand()
{

}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndConnectView.Create(strFileView, this, CRect(0, 0, 200, 200),
		/*TRUE*/ FALSE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	//
	if (!m_wndTopTitleView.Create(_T("图标"), this, CRect(0, 0, 100, 85),
		FALSE, ID_VIEW_TOPWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP, AFX_CBRS_REGULAR_TABS, AFX_CBRS_FLOAT))
	{
		TRACE0("Can't Create Top Title View\n");
		return FALSE; // 未能创建
	}

	/*
	if (!m_wndWeatherView.Create(_T("天气"), this, CRect(0, 0, 100, 30),
		FALSE, ID_VIEW_TOPWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP, AFX_CBRS_REGULAR_TABS, AFX_CBRS_FLOAT))
	{
		TRACE0("Can't Create Top Title View\n");
		return FALSE; // 未能创建
	}
	*/

	if (!m_wndNavView.Create(_T("导航"), this, CRect(0, 0, 100, 30),
		FALSE, ID_VIEW_TOPWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOP, AFX_CBRS_REGULAR_TABS, AFX_CBRS_FLOAT))
	{
		TRACE0("Can't Create Top Title View\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndLog.Create(strOutputWnd, this, CRect(0, 0, 100, 300),
		TRUE, ID_VIEW_OUTPUTWND, WS_CHILD /*| WS_VISIBLE*/ | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndConnectView.SetIcon(hFileViewIcon, FALSE);

	HICON hTopViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndConnectView.SetIcon(hTopViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndLog.SetIcon(hOutputBarIcon, FALSE);

//	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CMDIFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);

//	afxGlobalData.clrBtnText = RGB(255, 0, 0);
//	afxGlobalData.clrBtnFace = RGB(255, 0, 0);
//	afxGlobalData.brBarFace.DeleteObject();
//	afxGlobalData.brBarFace.CreateSolidBrush(afxGlobalData.clrBarFace);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	// 基类将执行真正的工作

	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

BOOL CMainFrame::OnShowMDITabContextMenu (CPoint point, DWORD dwAllowedItems, BOOL bDrop)
{
	if (bDrop)
	{
		return FALSE;
	}

	CMenu menu;
	//menu.CreatePopupMenu();
	//menu.AppendMenu(MF_BYCOMMAND|MF_STRING, ID_FILE_CLOSE, _T("关闭此视图"));
	menu.LoadMenu(IDR_MENU_CHILD_TAB);
	
	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
	if (pPopupMenu)
	{
		pPopupMenu->SetAutoDestroy (FALSE);
		//pPopupMenu->Create(this, point.x, point.y, menu.GetSafeHmenu());
		pPopupMenu->Create(this, point.x, point.y, menu.GetSubMenu(0)->GetSafeHmenu());
	}

	return TRUE;
}

void CMainFrame::OnChildClose()
{
	CChildFrame *pChild = (CChildFrame*)MDIGetActive();
	if (pChild)
	{
		pChild->SendMessage(WM_CLOSE, NULL, NULL);
	}
}

//////////////////////////////////////////////////////////////////////////

#pragma warning(push)
#pragma warning(disable:4996)
void CMainFrame::OutputMessage(ICON_TYPE eIcon, LPCTSTR lpszFormat, ...)
{
	TCHAR szDebug[4096];
	memset(szDebug, 0, sizeof(szDebug));
	//
	va_list args;
	va_start(args, lpszFormat);
	_vstprintf_s(szDebug, _countof(szDebug), lpszFormat, args); 

	/*
#ifdef UNICODE
	nBuf = vswprintf_s(m_szDebug, lpszFormat, args);
#else
	nBuf = vsprintf(m_szDebug, lpszFormat, args);
#endif
	*/

	//
	va_end(args);
	OutputMsg Msg;
	Msg.eType = eIcon;
	Msg.strMsg = szDebug;
	m_vecMsg.Add(Msg);
}

#pragma warning(pop)

HWND CMainFrame::GetNextMDIChildWnd()
{
	if (!m_hWndCurrentChild) 
	{ 
		// Get the first child window. 
		m_hWndCurrentChild = ::GetWindow(m_hWndMDIClient, GW_CHILD); 
	} 
	else 
	{ 
		// Get the next child window in the list. 
		m_hWndCurrentChild = ::GetWindow(m_hWndCurrentChild, GW_HWNDNEXT);
	} 

	if (!m_hWndCurrentChild) 
	{ 
		// No child windows exist in the MDIClient, 
		// or you are at the end of the list. This check 
		// will terminate any recursion. 
		return NULL; 
	} 

	// Check the kind of window 
	if (!::GetWindow(m_hWndCurrentChild, GW_OWNER)) 
	{ 
		CWnd* pChild = CWnd::FromHandle(m_hWndCurrentChild);
		if (pChild->IsKindOf(RUNTIME_CLASS(CChildFrame))) 
		{ 
			// CMDIChildWnd or a derived class. 
			return m_hWndCurrentChild; 
		}
		else 
		{ 
			// Window is foreign to the MFC framework. 
			// Check the next window in the list recursively. 
			return GetNextMDIChildWnd(); 
		} 
	} 
	else 
	{ 
		// Title window associated with an iconized child window. 
		// Recurse over the window manager's list of windows. 
		return GetNextMDIChildWnd(); 
	}
	return NULL;
}

DWORD CMainFrame::GetCountCMDIChildWnds()
{
	m_hWndCurrentChild = NULL;
	DWORD count = 0; 
	HWND pChild = GetNextMDIChildWnd(); 
	while (pChild) 
	{ 
		count++; 
		pChild = GetNextMDIChildWnd(); 
	}
	return count; 
}

//////////////////////////////////////////////////////////////////////////

void CMainFrame::OnAppConfig()
{
	USES_CONVERSION;
	CDialogAppConfig dlg;
	dlg.m_strHost = theConfig.m_Info.szHost;
	dlg.m_dwPort  = theConfig.m_Info.uPort;
	dlg.m_strKey  = A2T(theConfig.m_Info.szKey);
	dlg.m_strPage = theConfig.m_Info.szPage;
	dlg.m_dwTimeout = theConfig.m_Info.uTimeout;
	dlg.m_strLocalDir = theConfig.m_Info.szLocalDir;
	if (IDOK == dlg.DoModal())
	{
		lstrcpy(theConfig.m_Info.szHost, dlg.m_strHost);
		theConfig.m_Info.uPort = (USHORT)dlg.m_dwPort;
		StrCpyA(theConfig.m_Info.szKey, CT2A(dlg.m_strKey));
		lstrcpy(theConfig.m_Info.szPage, dlg.m_strPage);
		theConfig.m_Info.uTimeout = dlg.m_dwTimeout;
		lstrcpy(theConfig.m_Info.szLocalDir, dlg.m_strLocalDir);
		theConfig.Save();
	}
}

void CMainFrame::OnAppUserModify()
{
	CDialogUserModify dlg;
	if (IDOK == dlg.DoModal())
	{
		int i;
		for (i = 0; i < (int)theApp.m_userConfig.m_vecClient.size(); i++)
		{
			if (theApp.m_userConfig.m_vecClient[i].strUser == theApp.m_CurUserInfo.strUser)
			{
				theApp.m_userConfig.m_vecClient[i].strPassword = dlg.m_strPassword;
				theApp.m_CurUserInfo.strPassword = dlg.m_strPassword;
				theApp.m_userConfig.Save();
				AfxMessageBox(_T("修改成功!"), MB_ICONINFORMATION);
				break;
			}
		}
	}
}

void CMainFrame::OnAppUserManage()
{
	CDialogUserManage dlg;
	dlg.DoModal();
}

CChildFrame *CMainFrame::CreateChildView(DWORD dwClientId, LPCTSTR pTitle)
{
	// Find MDI
	DWORD dwCount = GetCountCMDIChildWnds();
	DWORD i;
	CChildFrame* pActiveChild = NULL;
	for(i = 0; i < dwCount; i++)
	{
		HWND hMdi = GetNextMDIChildWnd();
		CChildFrame* pChild = (CChildFrame*)CWnd::FromHandle(hMdi);
		if (pChild->GetId() == dwClientId)
		{
			pActiveChild = pChild;
			break;
		}
	}

	if(i != dwCount)
	{
		pActiveChild->ActiveTab(m_iCurrentTab);
		MDIActivate(pActiveChild);

		/*
		std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient(FALSE);
		for (int i = 0; i < (int)vecInfo.size(); i++)
		{
			if (vecInfo[i].dwClientId == dwClientId)
			{
				ClientInfo_t *pInfo = &vecInfo[i];
				CString strInfo = theApp.GenDisplayName(pInfo);
				CString strTmp;
				BOOL bNameValid = strTmp.LoadString(IDS_TITLE);
				CString strTitle;
				strTitle.Format(_T("%s - %s"), strTmp, strInfo);
				SetWindowText(strTitle);
				break;
			}
		}
		*/
		
		return NULL;
	}
	else
	{
		// New Document
		if(::SendMessage(GetSafeHwnd(), WM_COMMAND, ID_FILE_NEW, NULL) == FALSE)
		{
			OutputMessage(ICON_TYPE_ERROR, _T("Create New View Failed.."));
			return FALSE;
		}
		
		pActiveChild = (CChildFrame*)MDIGetActive();
		pActiveChild->SetWindowText(pTitle);

		CString strPassword = _T("685768op");
		UINT uTime = 0;
		BOOL bWhiteMode = FALSE;
		DWORD dwIdleTime = 0;
		std::map<DWORD, _CLIENT_INFO>::iterator pIterFound = theConfig.m_mapClient.find(dwClientId);
		if (pIterFound != theConfig.m_mapClient.end())
		{
			if (0 != lstrlen(pIterFound->second.strUnlockPassword))
			{
				strPassword = pIterFound->second.strUnlockPassword;
			}
			uTime = pIterFound->second.uScreenTime;
			bWhiteMode = pIterFound->second.bWhiteMode;
		}

		pActiveChild->Initialize(dwClientId, strPassword, uTime, dwIdleTime, bWhiteMode);

		/*
		std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient(FALSE);
		for (int i = 0; i < (int)vecInfo.size(); i++)
		{
			if (vecInfo[i].dwClientId == dwClientId)
			{
				ClientInfo_t *pInfo = &vecInfo[i];
				CString strInfo = theApp.GenDisplayName(pInfo);
				CString strTmp;
				BOOL bNameValid = strTmp.LoadString(IDS_TITLE);
				CString strTitle;
				strTitle.Format(_T("%s - %s"), strTmp, strInfo);
				SetWindowText(strTitle);
				break;
			}
		}
		*/
	}
	return pActiveChild;
}

VOID CMainFrame::OnConnectCommand(INT iCmdId)
{
	HTREEITEM hSel = GetConnectTree().GetSelectedItem();
	DWORD dwClientId = GetConnectTree().GetItemData(hSel);
	HTREEITEM hRoot = GetConnectTree().GetRootItem();
	if (NULL == hSel)
	{
		return;
	}

	if (ID_GROUP_CREATE == iCmdId)
	{
		CGroupDialog dlg;
		if (IDOK == dlg.DoModal())
		{
			CString strName = dlg.m_strName;
			std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.FindGroup(strName);
			if (pIter != theConfig.m_vecGroup.end())
			{
				AfxMessageBox(_T("分组已经存在!"), MB_ICONERROR);
			}
			else
			{
				HTREEITEM hInserted = GetConnectTree().InsertItem(strName, GetConnectTree().GetRootItem());
				GetConnectTree().SetItemData(hInserted, 0);
				DWORD dwGroupId = 0;
				for (int i = 0; i < (int)theConfig.m_vecGroup.size(); i++)
				{
					dwGroupId = max(dwGroupId, theConfig.m_vecGroup[i].dwGroupId);
				}
				dwGroupId += 1;

				std::vector<DWORD> vecIds;
				CAppConfig::GROUP_INFO groupInfo;
				groupInfo.strName = strName;
				groupInfo.dwGroupId = dwGroupId;
				groupInfo.vecIds = vecIds;
				theConfig.m_vecGroup.push_back(groupInfo);
				theConfig.Save();
			}
		}
		return;
	}
	else if (ID_GROUP_EDIT == iCmdId)
	{
		CString strName = GetConnectTree().GetItemText(hSel);
		CGroupDialog dlg;
		dlg.m_strName = strName;
		if (IDOK == dlg.DoModal())
		{
			CString strNewName = dlg.m_strName;
			std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.FindGroup(strNewName);
			if (pIter != theConfig.m_vecGroup.end())
			{
				AfxMessageBox(_T("分组已经存在!"), MB_ICONERROR);
			}
			else
			{
				pIter = theConfig.FindGroup(strName);
				if (pIter == theConfig.m_vecGroup.end())
				{
					AfxMessageBox(_T("无法找到分组!"), MB_ICONERROR);
				}
				else
				{
					pIter->strName = strNewName;
					theConfig.Save();
					GetConnectTree().SetItemText(hSel, strNewName);
					theApp.m_bLockInfoFromServiceUpdated = TRUE;
				}
			}
		}
		return;
	}
	else if (ID_GROUP_DEL == iCmdId)
	{
		CString strName = GetConnectTree().GetItemText(hSel);
		std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.FindGroup(strName);
		if (pIter != theConfig.m_vecGroup.end())
		{
			theConfig.m_vecGroup.erase(pIter);
			theConfig.Save();
			GetConnectTree().DeleteItem(hSel);
			theApp.m_bLockInfoFromServiceUpdated = TRUE;
		}
		return;
	}
	else if (ID_GROUP_ADDTO == iCmdId)
	{
		CGroupListDlg dlg;
		dlg.m_vecNames.push_back(_T("默认"));
		for (int i = 0; i < (int)theConfig.m_vecGroup.size(); i++)
		{
			dlg.m_vecNames.push_back(theConfig.m_vecGroup[i].strName);
		}
		if (IDOK == dlg.DoModal())
		{
			CString strName = dlg.m_strCurName;
			HTREEITEM hMultiSel = GetConnectTree().GetFirstSelectedItem();
			while (NULL != hMultiSel)
			{
				dwClientId = GetConnectTree().GetItemData(hMultiSel);
				if (0 != dwClientId)
				{
					// Delete Info
					std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.FindGroup(dwClientId);
					if (pIter != theConfig.m_vecGroup.end())
					{
						std::vector<DWORD>::iterator pIdIter = pIter->FindId(dwClientId);
						if (pIdIter != pIter->vecIds.end())
						{
							pIter->vecIds.erase(pIdIter);
						}
					}
					// Add Info
					pIter = theConfig.FindGroup(strName);
					if (pIter != theConfig.m_vecGroup.end())
					{
						std::vector<DWORD>::iterator pIdIter = pIter->FindId(dwClientId);
						if (pIdIter == pIter->vecIds.end())
						{
							pIter->vecIds.push_back(dwClientId);
						}
					}
				}

				HTREEITEM hPre = hMultiSel;
				hMultiSel = GetConnectTree().GetNextSelectedItem(hMultiSel);
				if (0 != dwClientId)
				{
					// Update UI
					GetConnectTree().DeleteItem(hPre);
				}
			}
			theApp.m_bLockInfoFromServiceUpdated = TRUE;
		}

		return;
	}

	if (hSel == hRoot)
	{
		return;
	}

	BOOL bFound = FALSE;
	CString strHostName = _T("");
	std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient(FALSE);
	ClientInfo_t *pInfo = NULL;
	for (int i = 0; i < (int)vecInfo.size(); i++)
	{
		if (vecInfo[i].dwClientId == dwClientId)
		{
			pInfo = &vecInfo[i];
			bFound = TRUE;
		}
	}
	if (FALSE == bFound)
	{
		AfxMessageBox(_T("无法获得信息！"), MB_ICONERROR);
		return;
	}

	if (ID_OPEN == iCmdId)
	{
		CString strId;
		//strId.Format(_T("%s(%d, %s)"), pInfo->szHostname, dwClientId, pInfo->szComment);
		//strId.Format(_T("%s(%s)"), pInfo->szHostname, pInfo->szIPOut);
		strId = theApp.GenDisplayName(pInfo);
		CreateChildView(dwClientId, strId);
	}
	else if (ID_PROPERTIES == iCmdId)
	{
		CDialogProperties dlg;
		dlg.m_dwID				= pInfo->dwClientId;
		dlg.m_strHostname		= pInfo->szHostname;
		dlg.m_strIPOut			= pInfo->szIPOut;
		dlg.m_strIPIn			= pInfo->szIPIn;
		dlg.m_strLastLogin		= pInfo->szTime;
		dlg.m_strDeviceSerial	= pInfo->szDeviceSerial;
		dlg.m_strComment		= pInfo->szComment;
		CString strUserIn;
		strUserIn += StrStr(pInfo->szIPIn, pInfo->szIPOut) ? _T("外") : _T("内");
		strUserIn += _T("网用户");
		dlg.m_strUserIn		= strUserIn;
		int iRet = dlg.DoModal();
		if (iRet == IDOK)
		{
		}
	}
	else if (ID_POLICY == iCmdId)
	{
		CDialogPolicy dlg;
		std::map<DWORD, _CLIENT_INFO>::iterator pIterFound = theConfig.m_mapClient.find(pInfo->dwClientId);
		if (pIterFound != theConfig.m_mapClient.end())
		{
			dlg.m_strUserName = pIterFound->second.strUserName;
			dlg.m_strCompany = pIterFound->second.strCompany;
			dlg.m_strNumber = pIterFound->second.strNumber;
			dlg.m_strId = pIterFound->second.strId;
			//dlg.m_dwStart = pIterFound->second.uTimeStart;
			//dlg.m_dwEnd = pIterFound->second.uTimeEnd;
		}
		int iRet = dlg.DoModal();
		if (iRet == IDOK)
		{
			std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(pInfo->dwClientId);
			if (pIter == theConfig.m_mapClient.end())
			{
				_CLIENT_INFO info;
				lstrcpy(info.strUserName, dlg.m_strUserName);
				lstrcpy(info.strCompany, dlg.m_strCompany);
				lstrcpy(info.strNumber, dlg.m_strNumber);
				lstrcpy(info.strId, dlg.m_strId);
				//info.uTimeStart = dlg.m_dwStart;
				//info.uTimeEnd = dlg.m_dwEnd;
				_TIME_RANGE range;
				range.uTimeStart = dlg.m_dwStart;
				range.uTimeEnd = dlg.m_dwEnd;
				info.vecTimeRange.push_back(range);
				info.iWinCount = 0;
				info.configInfo.LoadIni(theApp.m_strConfigPath);
				theConfig.m_mapClient.insert(std::map<DWORD, _CLIENT_INFO>::value_type(pInfo->dwClientId, info));
				theConfig.Save();
				pIter = theConfig.m_mapClient.find(pInfo->dwClientId);
			}
			else
			{
// 				memcpy(info.vecClass, pIter->second.vecClass, sizeof(info.vecClass));
// 				memcpy(info.vecWin, pIter->second.vecWin, sizeof(info.vecWin));
// 				info.iWinCount = pIter->second.iWinCount;
// 				pIter->second = info;
				lstrcpy(pIter->second.strUserName, dlg.m_strUserName);
				lstrcpy(pIter->second.strCompany, dlg.m_strCompany);
				lstrcpy(pIter->second.strNumber, dlg.m_strNumber);
				lstrcpy(pIter->second.strId, dlg.m_strId);
				//pIter->second.uTimeStart = dlg.m_dwStart;
				//pIter->second.uTimeEnd = dlg.m_dwEnd;
				_TIME_RANGE range;
				range.uTimeStart = dlg.m_dwStart;
				range.uTimeEnd = dlg.m_dwEnd;
				pIter->second.vecTimeRange.push_back(range);
				pIter->second.iWinCount = 0;
				theConfig.Save();
			}

		//	LPSTR pBuf = Crypt::_base64Encode((LPBYTE)&info, sizeof(info), NULL);
		//	USES_CONVERSION;
		//	CString strBuf = A2T(pBuf);
		//	free(pBuf);
		//	theProtoManager->InvokeClientLockInfo(pInfo->dwClientId, strBuf);

		//	std::string strJsonConfig;
		//	pIter->second.SaveConfig(strJsonConfig);
		//	CString strConfig = CA2T(strJsonConfig.c_str()).m_psz;
		//	theProtoManager->InvokeClientLockInfo(pInfo->dwClientId, strConfig);

			theApp.m_bLockInfoFromServiceUpdated = TRUE;
			OutputMessage(ICON_TYPE_INFO, _T("成功!"));
		}
	}
	else if (ID_POLICY_EDIT == iCmdId)
	{
		CPolicyEditDlg dlg;
		std::vector<_TIME_RANGE> vecRange;
		std::map<DWORD, _CLIENT_INFO>::iterator pIterFound = theConfig.m_mapClient.find(pInfo->dwClientId);
		if (pIterFound != theConfig.m_mapClient.end())
		{
			vecRange = pIterFound->second.vecTimeRange;
		}
		dlg.m_vecRange = vecRange;
		if (IDOK == dlg.DoModal())
		{
			std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(pInfo->dwClientId);
			if (pIter != theConfig.m_mapClient.end())
			{
				pIter->second.vecTimeRange = dlg.m_vecRange;
				theConfig.Save();

			//	std::string strJsonConfig;
			//	pIter->second.SaveConfig(strJsonConfig);
			//	CString strConfig = CA2T(strJsonConfig.c_str()).m_psz;
			//	theProtoManager->InvokeClientLockInfo(pInfo->dwClientId, strConfig);

				theApp.m_bLockInfoFromServiceUpdated = TRUE;
				OutputMessage(ICON_TYPE_INFO, _T("修改成功!"));
			}
			else
			{
				OutputMessage(ICON_TYPE_ERROR, _T("修改失败: 无法找到相关配置，可能已经下线"));
			}
		}
	}
	else if (ID_BATCH_LOCK == iCmdId || ID_BATCH_UNLOCK == iCmdId || ID_BATCH_AUTO == iCmdId || ID_BATCH_UPGRADE == iCmdId)
	{
		DWORD dwType = (ID_BATCH_LOCK == iCmdId ? BOT_TYPE_LOCK : (ID_BATCH_UNLOCK == iCmdId ? BOT_TYPE_UNLOCK : BOT_TYPE_SET_LOCK_AUTO));
		std::vector<DWORD> vecIds;
		HTREEITEM hMultiSel = GetConnectTree().GetFirstSelectedItem();
		while (NULL != hMultiSel)
		{
			if (hSel != GetConnectTree().GetRootItem())
			{
				DWORD dwClientId = GetConnectTree().GetItemData(hMultiSel);
				vecIds.push_back(dwClientId);
			}
			hMultiSel = GetConnectTree().GetNextSelectedItem(hMultiSel);
		}
		if (0 != vecIds.size())
		{
			if (ID_BATCH_LOCK == iCmdId || ID_BATCH_UNLOCK == iCmdId || ID_BATCH_AUTO == iCmdId)
			{
				for (std::vector<DWORD>::iterator pIter = vecIds.begin(); pIter != vecIds.end(); pIter++)
				{
					DWORD dwClientId = *pIter;
					if (0 == dwClientId)
						continue;
					for (int i = 0; i < (int)vecInfo.size(); i++)
					{
						if (vecInfo[i].dwClientId == dwClientId)
						{
							CProtoManagerChild *pChild = theProtoManager->GetChild(dwClientId);
							if (NULL != pChild)
							{
								pChild->InvokeReq(dwType);
							}
							
							break;
						}
					}
				}
			}
			else
			{
				CUpgradeDlg dlg;
				dlg.m_vecIds = vecIds;
				dlg.DoModal();
			}
		}
	}
	else if (ID_COMMENT == iCmdId)
	{
		std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(pInfo->dwClientId);
		if (pIter != theConfig.m_mapClient.end())
		{
			CModifyCommentDlg dlg;
			dlg.m_strComment = pIter->second.szCustomComment;
			if (IDOK == dlg.DoModal())
			{
				lstrcpy(pIter->second.szCustomComment, dlg.m_strComment);
				theConfig.Save();
				GetConnectTree().SetItemText(hSel, theApp.GenDisplayName(pInfo));
				theApp.m_bLockInfoFromServiceUpdated = TRUE;
			}
		}
		else
		{
			AfxMessageBox(_T("无法找到配置，请重试!"), MB_ICONERROR);
		}
	}
}

VOID CMainFrame::OnConnectUpdate(INT iCmdId, CCmdUI *pCmdUI)
{
	HTREEITEM hSel = GetConnectTree().GetSelectedItem();
	HTREEITEM hRoot = GetConnectTree().GetRootItem();
	if (NULL == hSel)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	if (ID_OPEN == iCmdId		
		|| ID_PROPERTIES == iCmdId
		|| ID_POLICY == iCmdId
		|| ID_POLICY_EDIT == iCmdId
		|| ID_COMMENT == iCmdId
		)
	{
		if (hSel == hRoot)
		{
			pCmdUI->Enable(FALSE);
			return;
		}

		std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
		DWORD dwId = GetConnectTree().GetItemData(hSel);
		size_t i;
		for(i = 0; i < vecInfo.size(); i++)
		{
			if (vecInfo[i].dwClientId == dwId)
			{
				break;
			}
		}
		if (i != vecInfo.size())
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
// 	else if (ID_POLICY == iCmdId)
// 	{
// 		pCmdUI->Enable(FALSE);
// 	}
	else if (ID_REMOTE_OPEN == iCmdId)
	{
		if (hSel == hRoot)
		{
			pCmdUI->Enable(FALSE);
			return;
		}

		pCmdUI->Enable(FALSE);
	}
	else if (ID_BATCH_LOCK == iCmdId || ID_BATCH_UNLOCK == iCmdId || ID_BATCH_AUTO == iCmdId || ID_BATCH_UPGRADE == iCmdId)
	{
		if (hSel == hRoot)
		{
			pCmdUI->Enable(FALSE);
			return;
		}

		INT iSelCount = 0;
		HTREEITEM hSel = GetConnectTree().GetFirstSelectedItem();
		while (NULL != hSel)
		{
			DWORD dwClientId = GetConnectTree().GetItemData(hSel);
			if (0 == dwClientId)
			{
				pCmdUI->Enable(FALSE);
				return;
			}

			if (hSel != GetConnectTree().GetRootItem())
			{
				iSelCount++;
			}
			hSel = GetConnectTree().GetNextSelectedItem(hSel);
		}

		BOOL bEnable = (0 != iSelCount ? TRUE : FALSE);
		pCmdUI->Enable(bEnable);
	}
	/*
	else if (ID_COMMENT == iCmdId)
	{
		if (hSel == hRoot)
		{
			pCmdUI->Enable(FALSE);
			return;
		}

		pCmdUI->Enable(TRUE);
	}
	*/
	else if (ID_GROUP_CREATE == iCmdId)
	{
		if (hSel != hRoot)
		{
			pCmdUI->Enable(FALSE);
			return;
		}

		pCmdUI->Enable(TRUE);
	}
	else if (ID_GROUP_EDIT == iCmdId || ID_GROUP_DEL == iCmdId)
	{
		if (GetConnectTree().GetParentItem(hSel) == hRoot)
		{
			CString strName = GetConnectTree().GetItemText(hSel);
			if (strName == _T("默认"))
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else if (ID_GROUP_ADDTO == iCmdId)
	{
		if (hSel == hRoot || GetConnectTree().GetParentItem(hSel) == hRoot)
		{
			pCmdUI->Enable(FALSE);
			return;
		}
		
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

VOID CMainFrame::OnConnectSelChanged(HTREEITEM hItem)
{
	CString strLastLogin;
	strLastLogin.LoadString(ID_INDICATOR_LAST_LOGIN);
	CString strIP;
	strIP.LoadString(ID_INDICATOR_IP);
	if(hItem == NULL || hItem == GetConnectTree().GetRootItem())
	{
	}
	else
	{
		DWORD dwId = GetConnectTree().GetItemData(hItem);
		std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
		for (int i = 0; i < (int)vecInfo.size(); i++)
		{
			if (vecInfo[i].dwClientId == dwId)
			{
				strLastLogin += vecInfo[i].szTime;
				strIP += vecInfo[i].szIPOut;
				strIP += _T("(");
				strIP += StrStr(vecInfo[i].szIPIn, vecInfo[i].szIPOut) ? _T("外") : _T("内");
				strIP += _T("网用户)");
				break;
			}
		}
	}
	m_wndStatusBar.SetPaneText(INDEX_STATUS_PANE_LAST_LOGIN, strLastLogin);
	m_wndStatusBar.SetPaneText(INDEX_STATUS_PANE_IP, strIP);
}

BOOL CMainFrame::CheckIdExistInTree(DWORD dwId, HTREEITEM& hFound)
{
	BOOL bExist = FALSE;
	HTREEITEM hRoot = GetConnectTree().GetRootItem();
	HTREEITEM hChild = GetConnectTree().GetChildItem(hRoot);
	while(hChild)
	{
		HTREEITEM hSubChild = GetConnectTree().GetChildItem(hChild);
		while (hSubChild)
		{
			DWORD_PTR dwData = GetConnectTree().GetItemData(hSubChild);
			if (dwData == dwId)
			{
				hFound = hSubChild;
				bExist = TRUE;
				break;
			}

			hSubChild = GetConnectTree().GetNextSiblingItem(hSubChild);
		}

		hChild = GetConnectTree().GetNextSiblingItem(hChild);
	}
	return bExist;
}

HTREEITEM CMainFrame::FindGroupTreeItem(CString strName)
{
	HTREEITEM hRoot = GetConnectTree().GetRootItem();
	HTREEITEM hChild = GetConnectTree().GetChildItem(hRoot);
	while(hChild)
	{
		CString strText = GetConnectTree().GetItemText(hChild);
		if (strText == strName)
		{
			return hChild;
		}

		hChild = GetConnectTree().GetNextSiblingItem(hChild);
	}
	return NULL;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	Lock();
	if (TIMER_ID_INVOKE_CLIENT == nIDEvent)
	{
		//OutputLog(_T("%d|===GetClient\n"), Utils::GetCurrentTimestamp());
		//theProtoManager->InvokeClient();
	}
	else if (TIMER_ID_RESPONSE == nIDEvent)
	{
		// Update Packet
		Packet_t Packet;
		thePacketManagerLock.Lock();
		BOOL bGet = thePacketManager.GetRes(Packet);
		thePacketManagerLock.Unlock();
		if (TRUE == bGet)
		{
			BOOL bShouldProcessed = FALSE;
			CChildFrame *pChildFrame = m_ChildMap[Packet.dwClientId];
			if (pChildFrame)
			{
				if (BOT_TYPE_DIR == Packet.dwType)
				{
					pChildFrame->OnDir(Packet);
				}
				else if (BOT_TYPE_CMD == Packet.dwType)
				{
					pChildFrame->OnCmd(Packet);
				}
				else if (BOT_TYPE_VERSION == Packet.dwType)
				{
					pChildFrame->OnVersion(Packet);
				}
				else if (BOT_TYPE_PASSWORD == Packet.dwType)
				{
					//pChildFrame->OnPassword(Packet);
				}
				else if (BOT_TYPE_FILE_TO_REMOTE == Packet.dwType)
				{
					LPSTR pJsonText = CConv::CharToUtf(Packet.strResponse);
					Json::Value jsonItem;
					Json::Reader reader;
					Json::Value root;
					if (reader.parse(pJsonText, root, false))
					{
						std::string strLog = GET_JSON_STRING(root, "log", "");
						std::string strRefresh = GET_JSON_STRING(root, "REFRESH", "");
						LPTSTR pLog = CConv::UtfToChar(strLog.c_str());
						OutputMessage(ICON_TYPE_INFO, pLog);
						delete[] pLog;

						// Update
						if (strRefresh.size() != 0)
						{
							BOOL bShoudRefresh = FALSE;
							m_vecClientIdsToRefreshFileLock.Lock();
							for (std::vector<DWORD>::iterator pIter = m_vecClientIdsToRefreshFile.begin(); pIter != m_vecClientIdsToRefreshFile.end();)
							{
								if (*pIter == Packet.dwClientId)
								{
									bShoudRefresh = TRUE;
									pIter = m_vecClientIdsToRefreshFile.erase(pIter);
								}
								else
								{
									pIter++;
								}
							}
							m_vecClientIdsToRefreshFileLock.Unlock();
							if (bShoudRefresh)
							{
								pChildFrame->OnChildRefresh();
							}
						}
					}
					else
					{
						OutputMessage(ICON_TYPE_ERROR, _T("下载文件协议错误"));
					}
					delete[] pJsonText;

					// Check Download
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
				else if (BOT_TYPE_FILE_FROM_REMOTE == Packet.dwType)
				{
					/*
					LPCTSTR pszResponse = (LPCTSTR)Packet.strResponse;
					if (Packet.strResponse.GetLength() != 0)
					{
						tstring strResponse = pszResponse + 1;
						if (*pszResponse == _T('0'))
						{
							OutputMessage(ICON_TYPE_INFO, strResponse.c_str());
						}
						else
						{
							size_t iCur = 0;
							size_t iNext = 0;
							iNext = strResponse.find(_T('&'), iCur);
							tstring strRemoteFile = strResponse.substr(iCur, iNext - iCur);
							iCur = iNext + 1;
							iNext = strResponse.find(_T('&'), iCur);
							tstring strRelayFile = strResponse.substr(iCur, iNext - iCur);
							iCur = iNext + 1;
							tstring strContent = strResponse.substr(iCur, strResponse.size() - iCur);

							OutputMessage(ICON_TYPE_INFO, strContent.c_str());
							theProtoManager->DownloadFile(Packet.dwClientId, strRelayFile.c_str(), strRemoteFile.c_str());
						}
					}
					else
					{
						OutputMessage(ICON_TYPE_INFO, _T("Download File Param Error"));
					}
					*/
					LPSTR pJsonText = CConv::CharToUtf(Packet.strResponse);
					Json::Value jsonItem;
					Json::Reader reader;
					Json::Value root;
					if (reader.parse(pJsonText, root, false))
					{
						std::string strLogA = GET_JSON_STRING(root, "log", "");
						LPTSTR pLog = CConv::UtfToChar(strLogA.c_str());
						int iStatus = GetJsonInt(root, "status", 999);
						std::string strPathA = GET_JSON_STRING(root, "PATH", "");
						LPTSTR pPath = CConv::UtfToChar(strPathA.c_str());
						std::string strRelayA = GET_JSON_STRING(root, "FILE_ID", "");
						LPTSTR pRelay = CConv::UtfToChar(strRelayA.c_str());
						std::string strTypeA = GET_JSON_STRING(root, "TYPE", "");
						if (strTypeA == "LOG")
						{
							OutputMessage(ICON_TYPE_INFO, pLog);
						}
						else
						{
							if (0 != iStatus)
							{
								OutputMessage(ICON_TYPE_ERROR, pLog);
							}
							else
							{
								OutputMessage(ICON_TYPE_INFO, pLog);
								if (0 != lstrlen(pRelay))
								{
									//theProtoManager->DownloadFile(Packet.dwClientId, pRelay, pPath);

									CString strPath = pPath;
									CString strFileName = strPath.Right(strPath.GetLength() - strPath.ReverseFind(_T('\\')) - 1);

									CString strHostName;
									std::vector<ClientInfo_t> vecClient = theProtoManager->GetResClient(FALSE);
									for (int i = 0; i < (int)vecClient.size(); i++)
									{
										if (vecClient[i].dwClientId == Packet.dwClientId)
										{
											strHostName = vecClient[i].szHostname;
											break;
										}
									}

									CString strLocalPath = theConfig.m_Info.szLocalDir;
									if (strLocalPath.Right(1) != _T('\\'))
									{
										strLocalPath += _T("\\");
									}
									CString strMidDir;
									strMidDir.Format(_T("%s.%d\\"), strHostName, Packet.dwClientId);
									strLocalPath += strMidDir;

									BOOL bCreateDir = CGlobalUtility::CreateDir(strLocalPath);
									if (!bCreateDir)
									{
										Packet_t Packet;
										Packet.dwType = BOT_TYPE_FILE_FROM_REMOTE;
										Packet.dwClientId = Packet.dwClientId;

										CString strTmp;
										strTmp.Format(_T("创建目录(%s)失败"), strLocalPath);
										std::map<CString, CString> mapPair;
										mapPair.insert(std::map<CString, CString>::value_type(_T("TYPE"), _T("LOG")));
										Packet.strResponse = theApp.GenResponse(strTmp, 1, mapPair);

										thePacketManagerLock.Lock();
										thePacketManager.AddRes(Packet);
										thePacketManagerLock.Unlock();
										return;
									}

									CString strLocalSrcPath;
									strLocalSrcPath.Format(_T("%sUpload\\%s"), theApp.m_strTmpPath, pRelay);
									
									strLocalPath += strFileName;
									DeleteFile(strLocalPath);
									BOOL bMove = MoveFile(strLocalSrcPath, strLocalPath);
									CString strTmp;
									if (bMove)
									{
										strTmp.Format(_T("下载文件(%s)到本地(%s)成功"), pRelay, strLocalPath);
										OutputMessage(ICON_TYPE_INFO, strTmp);
									}
									else
									{
										strTmp.Format(_T("下载文件(%s)到本地(%s)失败"), pRelay, strLocalPath);
										OutputMessage(ICON_TYPE_ERROR, strTmp);
									}
								}
							}
						}
						delete[] pLog;
						delete[] pPath;
						delete[] pRelay;
					}
					else
					{
						OutputMessage(ICON_TYPE_ERROR, _T("上传文件协议错误"));
					}
					delete[] pJsonText;
				}
				else if (BOT_TYPE_PROCESS == Packet.dwType)
				{
					pChildFrame->OnProcess(Packet);
				}
				else if (BOT_TYPE_SERVICE == Packet.dwType)
				{
					pChildFrame->OnService(Packet);
				}
				else if (BOT_TYPE_KEYBOARD == Packet.dwType)
				{
					pChildFrame->OnKeyboard(Packet);
				}
				else if (BOT_TYPE_NETWORK == Packet.dwType)
				{
					pChildFrame->OnNetwork(Packet);
				}
				else if (BOT_TYPE_KILL_PROCESS == Packet.dwType)
				{
					OutputMessage(ICON_TYPE_INFO, Packet.strResponse);
					pChildFrame->OnKillProcessResponse(Packet);
				}
				else
				{
					bShouldProcessed = TRUE;
				}
			}
			else
			{
				bShouldProcessed = TRUE;
			}

			if (bShouldProcessed)
			{
				if (
					BOT_TYPE_SCREEN == Packet.dwType		||
					BOT_TYPE_SHUTDOWN == Packet.dwType		||
					BOT_TYPE_RESTART == Packet.dwType		||
					BOT_TYPE_LOGOFF == Packet.dwType		||
					BOT_TYPE_LOCK == Packet.dwType			||
					BOT_TYPE_UNLOCK == Packet.dwType		||
					BOT_TYPE_SET_LOCK_AUTO == Packet.dwType ||
					BOT_TYPE_UPDATE_CLIENT_LOCK_INFO == Packet.dwType ||
					BOT_TYPE_USB_CONFIG == Packet.dwType	||
					BOT_TYPE_UPGRADE == Packet.dwType
					)
				{
					OutputMessage(ICON_TYPE_INFO, Packet.strResponse);
				}
				else
				{
					CString strBuf;
					strBuf.Format(_T("Unsupport Type: %d"), Packet.dwType);
					OutputMessage(ICON_TYPE_ERROR, strBuf);
				}
			}
		}
	}
	else if (TIMER_ID_CLIENT == nIDEvent)
	{
		// Update Client
		HTREEITEM hRoot = GetConnectTree().GetRootItem();
		//OutputLog(_T("%d|=======================\n"), Utils::GetCurrentTimestamp());
#if !defined(FULL_VERSION)
		std::vector<ClientInfo_t> vecInfo;
		std::vector<ClientInfo_t> vecInfoOrg = theProtoManager->GetResClient();
		for (int i = 0; i < (int)theApp.m_uHostCount && i < (int)vecInfoOrg.size(); i++)
		{
		//	OutputLog(_T("%d|IP: %s\n"), Utils::GetCurrentTimestamp(), vecInfoOrg[i].szIPOut);
			vecInfo.push_back(vecInfoOrg[i]);
		}
#else
		std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
#endif
		for(size_t i = 0; i < vecInfo.size(); i++)
		{
			ClientInfo_t& Info = vecInfo[i];
			HTREEITEM hFound = NULL;
			TCHAR szBuffer[1024] = {0};
			//wsprintf(szBuffer, _T("%s(%d, %s)"), Info.szHostname, Info.dwClientId, Info.szComment);
			//wsprintf(szBuffer, _T("%s(%s)"), Info.szHostname, Info.szIPOut);
			wsprintf(szBuffer, _T("%s"), theApp.GenDisplayName(&Info));
			if (FALSE == CheckIdExistInTree(Info.dwClientId, hFound))
			{
				HTREEITEM hTreeItemParent = m_hTreeDefault;
				std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.FindGroup(Info.dwClientId);
				if (pIter != theConfig.m_vecGroup.end())
				{
					HTREEITEM hTreeFind = FindGroupTreeItem(pIter->strName);
					if (NULL != hTreeFind)
					{
						hTreeItemParent = hTreeFind;
					}
				}
				
				HTREEITEM hAfter = GetConnectTree().InsertItem(szBuffer, IMAGE_DISCONNECTED, IMAGE_DISCONNECTED, hTreeItemParent);
				GetConnectTree().SetItemData(hAfter, (DWORD_PTR)Info.dwClientId);
				GetConnectTree().Expand(hTreeItemParent, TVE_EXPAND);
			}
			else
			{
				CString strExist = GetConnectTree().GetItemText(hFound);
				if (strExist != szBuffer)
				{
					GetConnectTree().SetItemText(hFound, szBuffer);
				}
			}
		}
		OnConnectSelChanged(GetConnectTree().GetSelectedItem());

		// Update Time
		HTREEITEM hChild = GetConnectTree().GetChildItem(hRoot);
		while (hChild)
		{
			HTREEITEM hSubChild = GetConnectTree().GetChildItem(hChild);
			while (hSubChild)
			{
				BOOL bLocked = FALSE;
				DWORD dwId = GetConnectTree().GetItemData(hSubChild);
				size_t i;
				for(i = 0; i < vecInfo.size(); i++)
				{
					if (vecInfo[i].dwClientId == dwId)
					{
						bLocked = vecInfo[i].bLocked;
						break;
					}
				}
				INT iImage;
				if (i != vecInfo.size())
				{
#if 0
					try
					{
						COleDateTime NowTime(COleDateTime::GetCurrentTime());
						COleVariant vtime(vecInfo[i].szTime);
						vtime.ChangeType(VT_DATE);
						COleDateTime LastLoginTime = vtime;
						COleDateTimeSpan tSecond = NowTime - LastLoginTime;
						double lfSecondSpan = tSecond.GetTotalSeconds();
						BOOL bTimeout = (lfSecondSpan > theConfig.m_Info.uTimeout);
						INT iImage = bTimeout ? IMAGE_DISCONNECTED : IMAGE_CONNECTED;
						int nCurImage;
						int nCurSelectedImage;
						GetConnectTree().GetItemImage(hSubChild, nCurImage, nCurSelectedImage);
						if (nCurImage != iImage || nCurSelectedImage != iImage)
						{
							GetConnectTree().SetItemImage(hSubChild, iImage, iImage);
						}
					}
					catch (CException *e)
					{
						TCHAR szError[256];
						e->GetErrorMessage(szError, 256);
						OutputLog(szError);
					}
#endif
					if (bLocked)
					{
						iImage = IMAGE_CONNECTED_LOCKED;
					}
					else
					{
						iImage = IMAGE_CONNECTED;
					}
				}
				else
				{
					iImage = IMAGE_DISCONNECTED;
				}
				int nCurImage;
				int nCurSelectedImage;
				GetConnectTree().GetItemImage(hSubChild, nCurImage, nCurSelectedImage);
				if (nCurImage != iImage || nCurSelectedImage != iImage)
				{
					GetConnectTree().SetItemImage(hSubChild, iImage, iImage);
					//theProtoManager->SetClientOnline(dwId, IMAGE_CONNECTED == iImage ? TRUE : FALSE);
					// Update Online
					for (std::map<DWORD, CChildFrame*>::iterator pIter = m_ChildMap.begin(); pIter != m_ChildMap.end(); pIter++)
					{
						if (pIter->second)
						{
							pIter->second->OnOnlineChanged();
						}
					}
					if (iImage == IMAGE_CONNECTED)
					{
						std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(dwId);
						if (pIter != theConfig.m_mapClient.end())
						{
							//LPSTR pBuf = Crypt::_base64Encode((LPBYTE)&info, sizeof(info), NULL);
							//USES_CONVERSION;
							//CString strBuf = A2T(pBuf);
							//free(pBuf);
							
						//	std::string strJsonConfig;
						//	pIter->second.SaveConfig(strJsonConfig);
						//	CString strBuf = CA2T(strJsonConfig.c_str()).m_psz;
						//	theProtoManager->InvokeClientLockInfo(dwId, strBuf);

							/*
							CProtoManagerChild *pChild = theProtoManager->GetChild(dwId);
							if (NULL != pChild)
							{
								pChild->InvokeReq(BOT_TYPE_SET_LOCK_AUTO);
							}
							*/
						}

						if (i != vecInfo.size())
						{
							if (0 == m_ChildMap.size())
							{
								CString strId;
								//strId.Format(_T("%s(%d, %s)"), pInfo->szHostname, dwClientId, pInfo->szComment);
								//strId.Format(_T("%s(%s)"), vecInfo[i].szHostname, vecInfo[i].szIPOut);
								strId = theApp.GenDisplayName(&vecInfo[i]);
								CreateChildView(dwId, strId);
							}

							// Update Online
							for (std::map<DWORD, CChildFrame*>::iterator pIter = m_ChildMap.begin(); pIter != m_ChildMap.end(); pIter++)
							{
								if (pIter->second)
								{
									pIter->second->OnOnlineChanged();
								}
							}
						}
					}
				}
				hSubChild = GetConnectTree().GetNextSiblingItem(hSubChild);
			}

			hChild = GetConnectTree().GetNextSiblingItem(hChild);
		}
	}
	else if (TIMER_ID_OUTPUT_MSG == nIDEvent)
	{
		OutputMsg Msg;
		BOOL bProcess = m_vecMsg.Get(Msg);
		if (bProcess)
		{
			CViewListCtrl &List = m_wndLog.m_List;
			int iCount = List.GetItemCount();
			LVITEM	lvi;
			lvi.mask	= LVIF_IMAGE|LVIF_TEXT;
			lvi.iItem	= iCount;
			lvi.iSubItem= 0;
			lvi.iImage	= (INT)Msg.eType;
			lvi.pszText	= _T("");
			int iIndex = List.InsertItem(&lvi);

			TCHAR szIndex[MAX_PATH] = {0};
			wsprintf(szIndex, _T("%d"), iIndex + 1);

			TCHAR szTime[MAX_PATH];
			COleDateTime DateTime = COleDateTime::GetCurrentTime();
			wsprintf(szTime, _T("%04d-%02d-%02d %02d:%02d:%02d"),
				DateTime.GetYear(), DateTime.GetMonth(), DateTime.GetDay(),
				DateTime.GetHour(), DateTime.GetMinute(), DateTime.GetSecond()
				);

			List.SetItemText(iIndex, 1, szIndex);
			List.SetItemText(iIndex, 2, szTime);
			List.SetItemText(iIndex, 3, Msg.strMsg);
			List.AutoScroll();
		}
	}
	else if (TIMER_CHECK_HOSTINFO_UPDATE == nIDEvent)
	{
		if (theApp.m_bLockInfoFromServiceUpdated)
		{
			theApp.m_bLockInfoFromServiceUpdated = FALSE;
			DWORD dwCount = GetCountCMDIChildWnds();
			DWORD i;
			for(i = 0; i < dwCount; i++)
			{
				HWND hMdi = GetNextMDIChildWnd();
				CChildFrame* pChild = (CChildFrame*)CWnd::FromHandle(hMdi);
				pChild->OnLockChanged();
				pChild->OnOnlineChanged();
			}
		}
	}
	else if (TIMER_CHECK_ONLINE == nIDEvent)
	{
		theProtoManager->CheckOnline();
		DWORD dwCount = 0;
		theProtoManager->CheckConfig(dwCount);
		if (0 != dwCount)
		{
			OutputMessage(ICON_TYPE_INFO, _T("更新配置成功!"));
		}
	}
	UnLock();
	CMDIFrameWndEx::OnTimer(nIDEvent);
}

void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CMDIFrameWndEx::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	if (!theApp.m_CurUserInfo.bAdmin)
	{
		pPopupMenu->DeleteMenu(ID_APP_USER_MANAGE, MF_BYCOMMAND);
	}
}

/*
VOID CMainFrame::UpdateTopWeather()
{
	COleDateTime oleDateTime = COleDateTime::GetCurrentTime();
	LPTSTR pszWeak = _T("日一二三四五六");
	CString strTmp;
	strTmp.Format(_T("  今天是：%04d年%02d月%02d日  星期%c"), oleDateTime.GetYear(), oleDateTime.GetMonth(), oleDateTime.GetDay(), pszWeak[oleDateTime.GetDayOfWeek() - 1]);
	m_wndWeatherView.SetText(strTmp);
}
*/

VOID CMainFrame::OnChangeActiveNavTab(int iIndex)
{
	m_iCurrentTab = iIndex;
	CChildFrame *pChild = (CChildFrame*)MDIGetActive();
	if (pChild)
	{
		pChild->ActiveTab(iIndex);
	}
}

void CMainFrame::onDbClickClient(DWORD dwClientId)
{
	std::vector<ClientInfo_t> vecInfo = theProtoManager->GetResClient();
	ClientInfo_t *pInfo = NULL;
	for (int i = 0; i < (int)vecInfo.size(); i++)
	{
		if (vecInfo[i].dwClientId == dwClientId)
		{
			pInfo = &vecInfo[i];
		}
	}
	if (NULL == pInfo)
	{
		AfxMessageBox(_T("无法找到配置,请重试"), MB_ICONERROR);
		return;
	}

	CString strId = theApp.GenDisplayName(pInfo);
	CreateChildView(dwClientId, strId);

	CChildFrame *pChild = (CChildFrame*)MDIGetActive();
	if (pChild)
	{
		pChild->ActiveTab(m_iCurrentTab);
	}
}


void CMainFrame::OnUpdateViewDefaultAllow(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(theApp.m_bDefaultAllow);
}

void CMainFrame::OnViewDefaultAllow()
{
	theApp.m_bDefaultAllow = !theApp.m_bDefaultAllow;
	theApp.WriteInt(_T("DefaultAllow"), theApp.m_bDefaultAllow);
}
