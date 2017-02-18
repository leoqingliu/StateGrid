// DialogPolicy.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "DialogPolicy.h"
#include "../Public/Utils.h"

// CDialogPolicy dialog

IMPLEMENT_DYNAMIC(CDialogPolicy, CDialog)

CDialogPolicy::CDialogPolicy(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogPolicy::IDD, pParent)
	, m_strUserName(_T(""))
	, m_strCompany(_T(""))
	, m_strNumber(_T(""))
	, m_strId(_T(""))
	, m_timeStart(COleDateTime::GetCurrentTime())
	, m_timeEnd(COleDateTime::GetCurrentTime())
{
//	m_dwStart = 0;
//	m_dwEnd = 86399;
}

CDialogPolicy::~CDialogPolicy()
{
}

void CDialogPolicy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USER, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_COMPANY, m_strCompany);
	DDX_Text(pDX, IDC_EDIT_NUMBER, m_strNumber);
	DDX_Text(pDX, IDC_EDIT_ID, m_strId);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START, m_timeStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END, m_timeEnd);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START, m_PickerStart);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END, m_PickerEnd);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_DATE, m_dateStart);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_DATE, m_dateEnd);
}


BEGIN_MESSAGE_MAP(CDialogPolicy, CDialog)
	ON_BN_CLICKED(IDOK, &CDialogPolicy::OnBnClickedOk)
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CDialogPolicy message handlers

BOOL CDialogPolicy::OnInitDialog()
{
	CDialog::OnInitDialog();

	/*
//	DWORD dwHour = HIDWORD(m_dwStart);
//	DWORD dwMin = MIDDWORD(m_dwStart);
//	DWORD dwSec = LODWORD(m_dwStart);
	DWORD dwHour = m_dwStart / 3600;
	m_dwStart -= m_dwStart / 3600 * 3600;
	DWORD dwMin = m_dwStart / 60;
	m_dwStart -= m_dwStart / 60 * 60;
	DWORD dwSec = m_dwStart;
	m_timeStart.SetTime(dwHour, dwMin, dwSec);

//	dwHour = HIDWORD(m_dwEnd);
//	dwMin = MIDDWORD(m_dwEnd);
//	dwSec = LODWORD(m_dwEnd);
	dwHour = m_dwEnd / 3600;
	m_dwEnd -= m_dwEnd / 3600 * 3600;
	dwMin = m_dwEnd / 60;
	m_dwEnd -= m_dwEnd / 60 * 60;
	dwSec = m_dwEnd;
	m_timeEnd.SetTime(dwHour, dwMin, dwSec);
	*/

	m_dwStart = Utils::GetCurrentTimestamp();
	m_dwEnd = m_dwStart + 3600 * 24;
	{
		COleDateTime oleDate((__time32_t)m_dwStart);
		SYSTEMTIME sysTime;
		oleDate.GetAsSystemTime(sysTime);
		CTime dateNew(sysTime);
		m_dateStart.SetDateTime(dateNew.GetYear(), dateNew.GetMonth(), dateNew.GetDay(), dateNew.GetHour(), dateNew.GetMinute(), dateNew.GetSecond());
		m_timeStart.SetDateTime(dateNew.GetYear(), dateNew.GetMonth(), dateNew.GetDay(), dateNew.GetHour(), dateNew.GetMinute(), dateNew.GetSecond());
	}
	{
		COleDateTime oleDate((__time32_t)m_dwEnd);
		SYSTEMTIME sysTime;
		oleDate.GetAsSystemTime(sysTime);
		CTime dateNew(sysTime);
		m_dateEnd.SetDateTime(dateNew.GetYear(), dateNew.GetMonth(), dateNew.GetDay(), dateNew.GetHour(), dateNew.GetMinute(), dateNew.GetSecond());
		m_timeEnd.SetDateTime(dateNew.GetYear(), dateNew.GetMonth(), dateNew.GetDay(), dateNew.GetHour(), dateNew.GetMinute(), dateNew.GetSecond());
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogPolicy::OnBnClickedOk()
{
	UpdateData();

	/*
	DWORD dwHour = m_timeStart.GetHour();
	DWORD dwMin = m_timeStart.GetMinute();
	DWORD dwSec = m_timeStart.GetSecond();
	//m_dwStart = (((DWORD)dwHour) << 16) + (((DWORD)dwMin) << 8) + dwSec;
	m_dwStart = dwHour * 3600 + dwMin * 60 + dwSec;

	dwHour = m_timeEnd.GetHour();
	dwMin = m_timeEnd.GetMinute();
	dwSec = m_timeEnd.GetSecond();
	//m_dwEnd = (((DWORD)dwHour) << 16) + (((DWORD)dwMin) << 8) + dwSec;
	m_dwEnd = dwHour * 3600 + dwMin * 60 + dwSec;
	*/

	COleDateTime oleDate(m_dateStart.GetYear(), m_dateStart.GetMonth(), m_dateStart.GetDay(), m_timeStart.GetHour(), m_timeStart.GetMinute(), m_timeStart.GetSecond());
	m_dwStart = Utils::GetCurrentTimestamp(oleDate);
	COleDateTime oleDateEnd(m_dateEnd.GetYear(), m_dateEnd.GetMonth(), m_dateEnd.GetDay(), m_timeEnd.GetHour(), m_timeEnd.GetMinute(), m_timeEnd.GetSecond());
	m_dwEnd = Utils::GetCurrentTimestamp(oleDateEnd);
	
	OnOK();
}

BOOL CDialogPolicy::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
//	DATETIMEPICKERINFO info = {0};
//	BOOL bInfo = m_PickerStart.GetDateTimePickerInfo(&info);
	CWnd *pWnd = GetFocus();
	if (pWnd->IsKindOf(RUNTIME_CLASS(CDateTimeCtrl)))
	{
// 		INPUT Input = {26};
// 		Input.type = INPUT_KEYBOARD;
// 		Input.ki.dwFlags = VK_UP;
// 		SendInput(1, &Input, sizeof(INPUT));

		//pWnd->SendMessage(WM_CHAR, VK_UP);

		if (zDelta >= 0)
		{
			pWnd->SendMessage(WM_KEYDOWN, VK_UP);
		}
		else
		{
			pWnd->SendMessage(WM_KEYDOWN, VK_DOWN);
		}
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}
