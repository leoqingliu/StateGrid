
// SafeDiskManagerView.cpp : CSafeDiskManagerView 类的实现
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "SafeDiskManagerDoc.h"
#include "SafeDiskManagerView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSafeDiskManagerView

IMPLEMENT_DYNCREATE(CSafeDiskManagerView, CListView)

BEGIN_MESSAGE_MAP(CSafeDiskManagerView, CListView)
	ON_WM_KEYDOWN()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CSafeDiskManagerView::OnLvnItemchanged)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// CSafeDiskManagerView 构造/析构

CSafeDiskManagerView::CSafeDiskManagerView()
{
	// TODO: 在此处添加构造代码

}

CSafeDiskManagerView::~CSafeDiskManagerView()
{
}


int CSafeDiskManagerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//	lpCreateStruct->cy = 0;
//	lpCreateStruct->cx = 0;

	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//
	lpCreateStruct->style = LVS_REPORT;

	//
	m_ImageList.Create(32, 32, ILC_COLOR32|ILC_MASK, 0, 0);
	GetListCtrl().SetImageList(&m_ImageList, LVSIL_NORMAL);

	return 0;
}

BOOL CSafeDiskManagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CListView::PreCreateWindow(cs);
}

// CSafeDiskManagerView 绘制

void CSafeDiskManagerView::OnDraw(CDC* /*pDC*/)
{
	CSafeDiskManagerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

CImageList* CSafeDiskManagerView::GetImageList()
{
	return &m_ImageList;
}

// CSafeDiskManagerView 诊断

#ifdef _DEBUG
void CSafeDiskManagerView::AssertValid() const
{
	CListView::AssertValid();
}

void CSafeDiskManagerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSafeDiskManagerDoc* CSafeDiskManagerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSafeDiskManagerDoc)));
	return (CSafeDiskManagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSafeDiskManagerView 消息处理程序

void CSafeDiskManagerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
}

CChildFrame* CSafeDiskManagerView::GetChildFrame()
{
	CChildFrame* pChild = (CChildFrame*)(CWnd::GetParentFrame());
	ASSERT(pChild != NULL);
	return pChild;
}

void CSafeDiskManagerView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//
	CListView::OnKeyDown(nChar, nRepCnt, nFlags);

	//
	INT iClickIndex;
	if(nChar == VK_RETURN)
	{
		// Click Index 
		iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
		GetChildFrame()->OnFileViewDblClick(iClickIndex);
	}
	else if(nChar == VK_BACK)
	{
		/*
		if(m_iRemoteSelLevel == 0)
		{
			return;
		}
		else
		{
			OnDblClickFileView(0);
		}
		*/
	}
	else if(
		nChar == VK_LEFT ||
		nChar == VK_RIGHT || 
		nChar == VK_UP || 
		nChar == VK_DOWN || 
		nChar == VK_HOME || 
		nChar == VK_END )
	{
		// Click Index 
		iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
		GetChildFrame()->OnFileViewSelChanged(iClickIndex);
	}
}

void CSafeDiskManagerView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//
	if((pNMLV->uOldState & LVIS_SELECTED) == 0 && (pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED) 
	{
		GetChildFrame()->OnFileViewSelChanged(pNMLV->iItem);
	}
	*pResult = 0;
}

void CSafeDiskManagerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//
	CListView::OnLButtonDown(nFlags, point);

	// Click Index 
	INT iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
	if (-1 == iClickIndex)
	{
		GetChildFrame()->OnFileViewSelChanged(iClickIndex);
	}
}

void CSafeDiskManagerView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//
	CListView::OnLButtonDblClk(nFlags, point);

	// Click Index 
	INT iClickIndex = GetListCtrl().GetNextItem(-1, LVIS_SELECTED);
	GetChildFrame()->OnFileViewDblClick(iClickIndex);
}

void CSafeDiskManagerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CListView::OnRButtonDown(nFlags, point);
}
