
// SafeDiskManagerDoc.cpp : CSafeDiskManagerDoc 类的实现
//

#include "stdafx.h"
#include "SafeDiskManager.h"

#include "SafeDiskManagerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSafeDiskManagerDoc

IMPLEMENT_DYNCREATE(CSafeDiskManagerDoc, CDocument)

BEGIN_MESSAGE_MAP(CSafeDiskManagerDoc, CDocument)
END_MESSAGE_MAP()


// CSafeDiskManagerDoc 构造/析构

CSafeDiskManagerDoc::CSafeDiskManagerDoc()
{
	// TODO: 在此添加一次性构造代码

}

CSafeDiskManagerDoc::~CSafeDiskManagerDoc()
{
}

BOOL CSafeDiskManagerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CSafeDiskManagerDoc 序列化

void CSafeDiskManagerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CSafeDiskManagerDoc 诊断

#ifdef _DEBUG
void CSafeDiskManagerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSafeDiskManagerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CSafeDiskManagerDoc 命令

void CSafeDiskManagerDoc::SetTitle(LPCTSTR lpszTitle)
{
//	CDocument::SetTitle(lpszTitle);
}
