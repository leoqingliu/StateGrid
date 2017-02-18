// TransparentCheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "SafeDiskManager.h"
#include "TransparentCheckBox.h"


// CTransparentCheckBox

IMPLEMENT_DYNAMIC(CTransparentCheckBox, CButton)

CTransparentCheckBox::CTransparentCheckBox()
{

}

CTransparentCheckBox::~CTransparentCheckBox()
{
}


BEGIN_MESSAGE_MAP(CTransparentCheckBox, CButton)
	ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTransparentCheckBox message handlers

HBRUSH CTransparentCheckBox::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}

HBRUSH CTransparentCheckBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CButton::OnCtlColor(pDC, pWnd, nCtlColor);

	pDC->SetBkMode(TRANSPARENT);
	return (HBRUSH)::GetStockObject(NULL_BRUSH);

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CTransparentCheckBox::OnEraseBkgnd(CDC* pDC)
{
	//return CButton::OnEraseBkgnd(pDC);
	return TRUE;
}

void CTransparentCheckBox::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CButton::OnPaint() for painting messages

	if( ( GetStyle() & BS_OWNERDRAW ) == BS_OWNERDRAW )
	{
		CPaintDC dc( this );

		RECT rect;
		GetClientRect( & rect );
		rect.right = rect.left + 20;
		CMFCVisualManager::GetInstance()->DrawCheckBox(
			& dc
			, rect
			, false                               // highlighted
			//, mv_bIsChecked ? 1 : 0 // state
			,1
			, true                                // enabled
			, false                               // pressed
			);

		// draw text next to the checkbox if you like
		//CMFCVisualManager::GetInstance()->DrawTextOnGlass()
	}
	else
		__super::OnPaint();
}

BOOL CTransparentCheckBox::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= BS_OWNERDRAW;
	return CButton::PreCreateWindow(cs);
}
