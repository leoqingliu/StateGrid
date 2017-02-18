#pragma once


// CTopWeatherView

class CTopWeatherView : public CDockablePane
{
	DECLARE_DYNAMIC(CTopWeatherView)

public:
	CTopWeatherView();
	virtual ~CTopWeatherView();

protected:
	DECLARE_MESSAGE_MAP()
//	CStatic m_TextStatic;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void AdjustLayout();
	virtual BOOL IsResizable() const;
	afx_msg void OnPaint();
	CString m_strText;
	CFont m_Font;

public:
	VOID SetText(CString strText);
};


