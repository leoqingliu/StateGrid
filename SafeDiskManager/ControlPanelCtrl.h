#pragma once


// CControlPanelCtrl

class CControlPanelCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CControlPanelCtrl)

public:
	CControlPanelCtrl();
	virtual ~CControlPanelCtrl();

protected:
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CImageList m_ExLargeImageList;
	CImageList m_SmallImageList;

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	VOID UpdateList();
	VOID ClearExLargeImageList()
	{
		int iExLargeImageCount = m_ExLargeImageList.GetImageCount();
		for(int i = iExLargeImageCount - 1 ; i >= 0; i--)
		{
			m_ExLargeImageList.Remove(i);
		}
	}
	VOID ClearSmallImageList()
	{
		int iSmallImageCount = m_SmallImageList.GetImageCount();
		for(int i = iSmallImageCount - 1 ; i >= 0; i--)
		{
			m_SmallImageList.Remove(i);
		}
	}
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	std::map<INT, INT> m_mapItemImage;
};


