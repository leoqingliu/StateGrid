#pragma once


// CMFCDesktopAlertNoCloseWnd

class CMFCDesktopAlertNoCloseWnd : public CMFCDesktopAlertWnd
{
	DECLARE_DYNAMIC(CMFCDesktopAlertNoCloseWnd)

public:
	CMFCDesktopAlertNoCloseWnd();
	virtual ~CMFCDesktopAlertNoCloseWnd();

protected:
	DECLARE_MESSAGE_MAP()
};


