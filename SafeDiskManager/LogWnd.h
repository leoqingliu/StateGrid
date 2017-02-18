
#pragma once
#include "../Public/View/ViewListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COutputList 

class CLogWnd : public CDockablePane
{
public:
	CLogWnd();

public:
	virtual ~CLogWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

private:
	CFont *m_pFont;

public:
	CViewListCtrl m_List;

	VOID SetFont(CFont *pFont)
	{
		m_pFont = pFont;
	}
};
