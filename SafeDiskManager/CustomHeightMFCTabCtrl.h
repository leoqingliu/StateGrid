#pragma once


// CCustomMFCTabCtrl

class CCustomHeightMFCTabCtrl : public CMFCTabCtrl
{
	DECLARE_DYNAMIC(CCustomHeightMFCTabCtrl)

public:
	CCustomHeightMFCTabCtrl(int iHeight);
	virtual ~CCustomHeightMFCTabCtrl();
	void ResetHeight();

protected:
	DECLARE_MESSAGE_MAP()
	void SetTabsHeight();

protected:
	int m_iHeight;
};


