
// SafeDiskManagerView.h : CSafeDiskManagerView 类的接口
//


#pragma once

class CSafeDiskManagerDoc;
class CChildFrame;
class CSafeDiskManagerView : public CListView
{
protected: // 仅从序列化创建
	CSafeDiskManagerView();
	DECLARE_DYNCREATE(CSafeDiskManagerView)

// 属性
public:
	CSafeDiskManagerDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CSafeDiskManagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	CChildFrame* GetChildFrame();
	CImageList m_ImageList;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CImageList* GetImageList();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // SafeDiskManagerView.cpp 中的调试版本
inline CSafeDiskManagerDoc* CSafeDiskManagerView::GetDocument() const
   { return reinterpret_cast<CSafeDiskManagerDoc*>(m_pDocument); }
#endif

