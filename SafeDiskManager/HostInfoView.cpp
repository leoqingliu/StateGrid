// HostInfoView.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "HostInfoView.h"
#include "../Public/Utils.h"

// CHostInfoView

IMPLEMENT_DYNCREATE(CHostInfoView, CView)

CHostInfoView::CHostInfoView()
{
	m_pGridCtrl = NULL;
}

CHostInfoView::~CHostInfoView()
{
	Close();
}

BEGIN_MESSAGE_MAP(CHostInfoView, CView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, 100, OnGridRClick)
	ON_COMMAND(ID_HOST_REFRESH, &CHostInfoView::OnHostRefresh)
	ON_UPDATE_COMMAND_UI(ID_HOST_REFRESH, &CHostInfoView::OnUpdateHostRefresh)
END_MESSAGE_MAP()


// CHostInfoView drawing

void CHostInfoView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CHostInfoView diagnostics

#ifdef _DEBUG
void CHostInfoView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CHostInfoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CHostInfoView message handlers

void CHostInfoView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
 	if (m_pGridCtrl)
 		m_pGridCtrl->OnBeginPrinting(pDC, pInfo);

//	CView::OnBeginPrinting(pDC, pInfo);
}

void CHostInfoView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
 	if (m_pGridCtrl)
 		m_pGridCtrl->OnEndPrinting(pDC, pInfo);

//	CView::OnEndPrinting(pDC, pInfo);
}

void CHostInfoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	if (NULL == m_pGridCtrl)
	{
		// Create the Gridctrl Object
		m_pGridCtrl = new CGridCtrl;
		if (!m_pGridCtrl)
		{
			return;
		}

		// Create Gridctrl Window
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->Create(rect, this, 100);
		m_pGridCtrl->SetEditable(FALSE);
		m_pGridCtrl->EnableDragAndDrop(TRUE);

		try
		{
		//	m_pGridCtrl->SetRowCount(18);
			m_pGridCtrl->SetColumnCount(8);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(1);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}

		UpdateItems();
	}
}

void CHostInfoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pGridCtrl && m_pGridCtrl->GetSafeHwnd())
	{
		CRect rect;
		GetClientRect(rect);
		m_pGridCtrl->MoveWindow(rect);
// 		int iColCount = m_pGridCtrl->GetColumnCount();
// 		int iWidth = (rect.Width() - 5) / iColCount;
// 		for (int i = 0; i < iColCount; i++)
// 		{
// 			m_pGridCtrl->SetColumnWidth(i, iWidth);
// 		}
	}
}

BOOL CHostInfoView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (m_pGridCtrl && IsWindow(m_pGridCtrl->m_hWnd))
	{
		if (m_pGridCtrl->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		{
			return TRUE;
		}
	}

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CHostInfoView::OnEraseBkgnd(CDC* pDC)
{
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void CHostInfoView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	if (m_pGridCtrl)
	{
		m_pGridCtrl->OnPrint(pDC, pInfo);
	}

//	CView::OnPrint(pDC, pInfo);
}

BOOL CHostInfoView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &=~ (WS_MAXIMIZE | WS_HSCROLL | WS_VSCROLL);
	cs.style &=~ (WS_BORDER);

	return CView::PreCreateWindow(cs);
}

VOID CHostInfoView::Close()
{
	if (m_pGridCtrl)
	{
		delete m_pGridCtrl;
		m_pGridCtrl = NULL;
	}
}

void CHostInfoView::OnClose()
{
	CView::OnClose();
}

#if 0
void CHostInfoView::ParseSpecTime(DWORD dwTime, DWORD &dwHour, DWORD &dwMin, DWORD &dwSec)
{
	dwHour = dwTime / 3600;
	dwTime -= dwTime / 3600 * 3600;
	dwMin = dwTime / 60;
	dwTime -= dwTime / 60 * 60;
	dwSec = dwTime;
}
#endif

void CHostInfoView::UpdateItems()
{
	if (NULL == m_pGridCtrl)
		return;

#if 0
	// fill rows/cols with text
	for (int row = 0; row < m_pGridCtrl->GetRowCount(); row++)
	{
		for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row < 1) {
				Item.nFormat = DT_LEFT|DT_WORDBREAK;
				Item.strText.Format(_T("Column %d"),col);
			} else if (col < 1) {
				Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
				Item.strText.Format(_T("Row %d"),row);
			} else {
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
				Item.strText.Format(_T("%d"),row*col);
			}
			m_pGridCtrl->SetItem(&Item);
		}
	}
#endif

	std::vector<ClientInfo_t> vecInfos = theProtoManager->GetResClient(FALSE);
	if (vecInfos.size() > m_pGridCtrl->GetRowCount() - 1)
	{
		m_pGridCtrl->SetRowCount(vecInfos.size() + 1);
	}

	// Header
	DWORD dwTimeStamp = Utils::GetCurrentTimestamp();
	for (int i = 0; i < 8; i++)
	{
		CString strText;
		if (0 == i)
		{
			strText = _T("内网机");
		}
		else
		{
		//	LPCTSTR pWeek = _T("一二三四五六日");
		//	TCHAR chWeek = pWeek[i - 1];
		//	strText.Format(_T("周%c"), chWeek);
			COleDateTime oleTime((__time32_t)dwTimeStamp);
			strText.Format(_T("%04d-%02d-%02d"), oleTime.GetYear(), oleTime.GetMonth(), oleTime.GetDay());
			dwTimeStamp += 3600 * 24;
		}
		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT;
		Item.row = 0;
		Item.col = i;
		Item.nFormat = DT_CENTER /*| DT_WORDBREAK*/;
		Item.strText = strText;
		m_pGridCtrl->SetItem(&Item);
// 		if (0 == (i % 2))
// 		{
// 			m_pGridCtrl->setcell
// 		}
	}
	// Row
	for (int i = 0; i < (int)vecInfos.size(); i++)
	{
		CString strText;
		//strText.Format(_T("内网%02d"), i);
		/*
		strText = vecInfos[i].szHostname;
		if (strText.IsEmpty())
		{
			strText = vecInfos[i].szIPOut;
		}
		*/
		std::vector<CAppConfig::GROUP_INFO>::iterator pIter = theConfig.FindGroup(vecInfos[i].dwClientId);
		if (pIter != theConfig.m_vecGroup.end())
		{
			strText = pIter->strName;
		}
		else
		{
			strText = _T("默认");
		}
		strText += _T(" - ") + theApp.GenDisplayName(&vecInfos[i]);

		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT;
		Item.row = i + 1;
		Item.col = 0;
		Item.nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
		Item.strText = strText;
		m_pGridCtrl->SetItem(&Item);
	}
#if 0
	// Data(Header)
	for (int i = 0; i < 14; i++)
	{
		CString strText;
		if (0 == (i % 2))
		{
			strText = _T("上午");
		}
		else
		{
			strText = _T("下午");
		}
		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT;
		Item.row = 1;
		Item.col = i + 1;
		Item.nFormat = DT_CENTER /*| DT_WORDBREAK*/;
		Item.strText = strText;
		m_pGridCtrl->SetItem(&Item);
	}
#endif

	// Data
#if 0
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < (int)vecInfos.size(); j++)
		{
			CString strHost;
			//strHost.Format(_T("内网%02d"), i + 1);
			strHost = vecInfos[j].szHostname;
			if (strHost.IsEmpty())
			{
				strHost = vecInfos[j].szIPOut;
			}

			//CString strText = _T("09:00 - 17:30");
			CString strText;
			CString strTipText;
			DWORD dwClientId = vecInfos[j].dwClientId;
			std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(dwClientId);
			if (pIter != theConfig.m_mapClient.end())
			{
				DWORD dwHourStart = 0, dwMinStart = 0, dwSecStart = 0;
				ParseSpecTime(pIter->second.uTimeStart, dwHourStart, dwMinStart, dwSecStart);
				DWORD dwHourEnd = 0, dwMinEnd = 0, dwSecEnd = 0;
				ParseSpecTime(pIter->second.uTimeEnd, dwHourEnd, dwMinEnd, dwSecEnd);

				strText.Format(_T("%d:%d - %d:%d"), dwHourStart, dwMinStart, dwHourEnd, dwMinEnd);
				strText = strHost + _T("\n") + strText;
				
				strTipText = pIter->second.szTimeDesc;
				strTipText += _T("\n");
				strTipText += strText;
			}

			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = j + 1;
			Item.col = i + 1;
			Item.nFormat = DT_CENTER | DT_VCENTER /*| DT_SINGLELINE*/ | DT_WORDBREAK /*| DT_END_ELLIPSIS | DT_NOPREFIX*/;
			Item.strText = strText;
			Item.strTipText = strTipText;
			m_pGridCtrl->SetItem(&Item);
		}
	}
#else
	for (int j = 0; j < (int)vecInfos.size(); j++)
	{
		CString strDayText[7];
		CString strHost;
		strHost = vecInfos[j].szHostname;
		if (strHost.IsEmpty())
		{
			strHost = vecInfos[j].szIPOut;
		}

		//CString strText = _T("09:00 - 17:30");
		CString strText;
		CString strTipText;
		DWORD dwClientId = vecInfos[j].dwClientId;
		std::map<DWORD, _CLIENT_INFO>::iterator pIter = theConfig.m_mapClient.find(dwClientId);
		if (pIter != theConfig.m_mapClient.end())
		{
			//COleDateTime curTime = COleDateTime::GetCurrentTime();
			//COleDateTime oleTimeStart;
			//oleTimeStart.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 0, 0, 0);
			//COleDateTime oleTimeEnd;
			//oleTimeEnd.SetDateTime(curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), 23, 59, 59);
			DWORD curTime = Utils::GetCurrentTimestamp();
			_TIME_RANGE rangeDaySplit[7];
			ZeroMemory(rangeDaySplit, sizeof(rangeDaySplit));
			for (int i = 0; i < _countof(rangeDaySplit); i++)
			{
				COleDateTime oleTimeCur((__time32_t)curTime);
				COleDateTime oleTimeStart;
				oleTimeStart.SetDateTime(oleTimeCur.GetYear(), oleTimeCur.GetMonth(), oleTimeCur.GetDay(), 0, 0, 0);
				COleDateTime oleTimeEnd;
				oleTimeEnd.SetDateTime(oleTimeCur.GetYear(), oleTimeCur.GetMonth(), oleTimeCur.GetDay(), 23, 59, 59);
				rangeDaySplit[i].uTimeStart = Utils::GetCurrentTimestamp(oleTimeStart);
				rangeDaySplit[i].uTimeEnd = Utils::GetCurrentTimestamp(oleTimeEnd);
				curTime += 3600 * 24;
			}

			for (int i = 0; i < _countof(rangeDaySplit); i++)
			{
				_TIME_RANGE rangeDay = rangeDaySplit[i];
				for (int k = 0; k < (int)pIter->second.vecTimeRange.size(); k++)
				{
					_TIME_RANGE range = pIter->second.vecTimeRange[k];
					if (rangeDay.uTimeStart > range.uTimeEnd ||
						rangeDay.uTimeEnd < range.uTimeStart)
					{
					}

					else if (rangeDay.uTimeStart < range.uTimeStart && rangeDay.uTimeEnd > range.uTimeEnd)
					{
						COleDateTime oleStart((__time32_t)range.uTimeStart);
						COleDateTime oleEnd((__time32_t)range.uTimeEnd);
						CString strTmp;
						strTmp.Format(_T("%02d:%02d:%02d - %02d:%02d:%02d"),
							oleStart.GetHour(), oleStart.GetMinute(), oleStart.GetSecond(), oleEnd.GetHour(), oleEnd.GetMinute(), oleEnd.GetSecond());
						if (!strDayText[i].IsEmpty())
							strDayText[i] += _T("\n");
						strDayText[i] += strTmp;
					}
					else if (rangeDay.uTimeStart > range.uTimeStart && rangeDay.uTimeEnd < range.uTimeEnd)
					{
						COleDateTime oleStart((__time32_t)rangeDay.uTimeStart);
						COleDateTime oleEnd((__time32_t)rangeDay.uTimeEnd);
						CString strTmp;
						strTmp.Format(_T("%02d:%02d:%02d - %02d:%02d:%02d"),
							oleStart.GetHour(), oleStart.GetMinute(), oleStart.GetSecond(), oleEnd.GetHour(), oleEnd.GetMinute(), oleEnd.GetSecond());
						if (!strDayText[i].IsEmpty())
							strDayText[i] += _T("\n");
						strDayText[i] += strTmp;
					}

					else if (rangeDay.uTimeStart >= range.uTimeStart && rangeDay.uTimeEnd >= range.uTimeEnd)
					{
						COleDateTime oleStart((__time32_t)rangeDay.uTimeStart);
						COleDateTime oleEnd((__time32_t)range.uTimeEnd);
						CString strTmp;
						strTmp.Format(_T("%02d:%02d:%02d - %02d:%02d:%02d"),
							oleStart.GetHour(), oleStart.GetMinute(), oleStart.GetSecond(), oleEnd.GetHour(), oleEnd.GetMinute(), oleEnd.GetSecond());
						if (!strDayText[i].IsEmpty())
							strDayText[i] += _T("\n");
						strDayText[i] += strTmp;
					}
					else if (rangeDay.uTimeStart <= range.uTimeStart && rangeDay.uTimeEnd <= range.uTimeEnd)
					{
						COleDateTime oleStart((__time32_t)range.uTimeStart);
						COleDateTime oleEnd((__time32_t)rangeDay.uTimeEnd);
						CString strTmp;
						strTmp.Format(_T("%02d:%02d:%02d - %02d:%02d:%02d"),
							oleStart.GetHour(), oleStart.GetMinute(), oleStart.GetSecond(), oleEnd.GetHour(), oleEnd.GetMinute(), oleEnd.GetSecond());
						if (!strDayText[i].IsEmpty())
							strDayText[i] += _T("\n");
						strDayText[i] += strTmp;
					}
				}
			}

			/*
			DWORD dwHourStart = 0, dwMinStart = 0, dwSecStart = 0;
			ParseSpecTime(pIter->second.uTimeStart, dwHourStart, dwMinStart, dwSecStart);
			DWORD dwHourEnd = 0, dwMinEnd = 0, dwSecEnd = 0;
			ParseSpecTime(pIter->second.uTimeEnd, dwHourEnd, dwMinEnd, dwSecEnd);

			strText.Format(_T("%d:%d - %d:%d"), dwHourStart, dwMinStart, dwHourEnd, dwMinEnd);
			strText = strHost + _T("\n") + strText;

			strTipText = pIter->second.szTimeDesc;
			strTipText += _T("\n");
			strTipText += strText;
			*/
		}

		for (int i = 0; i < 7; i++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = j + 1;
			Item.col = i + 1;
			Item.nFormat = DT_CENTER | DT_VCENTER /*| DT_SINGLELINE*/ | DT_WORDBREAK /*| DT_END_ELLIPSIS | DT_NOPREFIX*/;
			Item.strText = strDayText[i];
			Item.strTipText = strTipText;
			m_pGridCtrl->SetItem(&Item);
		}
#if 0
		GV_ITEM Item;
		Item.mask = GVIF_TEXT | GVIF_FORMAT;
		Item.row = j + 1;
		Item.col = i + 1;
		Item.nFormat = DT_CENTER | DT_VCENTER /*| DT_SINGLELINE*/ | DT_WORDBREAK /*| DT_END_ELLIPSIS | DT_NOPREFIX*/;
		Item.strText = strText;
		Item.strTipText = strTipText;
		m_pGridCtrl->SetItem(&Item);
#endif
	}
#endif

	m_pGridCtrl->AutoSize();
}

void CHostInfoView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CView::OnLButtonDblClk(nFlags, point);
}

void CHostInfoView::OnTimer(UINT_PTR nIDEvent)
{
	CView::OnTimer(nIDEvent);
}

void CHostInfoView::OnGridRClick(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*) pNotifyStruct;
//	Trace(_T("Right button click on row %d, col %d\n"), pItem->iRow, pItem->iColumn);
	
	CPoint point;
	GetCursorPos(&point);
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_MENU_POPUP_HOSTINFO, point.x, point.y, this, TRUE);
}

void CHostInfoView::OnHostRefresh()
{
	UpdateItems();
}

void CHostInfoView::OnUpdateHostRefresh(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}
