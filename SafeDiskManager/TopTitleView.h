#pragma once

#include "ViewTree.h"

// CTopTitleView

class CTopTitleView : public CDockablePane
{
	DECLARE_DYNAMIC(CTopTitleView)

public:
	CTopTitleView();
	virtual ~CTopTitleView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void AdjustLayout();
	CStatic m_ImageStatic;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual BOOL IsResizable() const;
	HBITMAP m_hBitmap;
	BITMAP m_bitmapInfo;
};


