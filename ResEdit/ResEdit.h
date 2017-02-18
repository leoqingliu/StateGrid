// ResEdit.h : main header file for the RESEDIT application
//

#if !defined(AFX_RESEDIT_H__B94BE54B_C8E4_4E65_AB2F_18F97A6ACF32__INCLUDED_)
#define AFX_RESEDIT_H__B94BE54B_C8E4_4E65_AB2F_18F97A6ACF32__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CResEditApp:
// See ResEdit.cpp for the implementation of this class
//

class CResEditApp : public CWinApp
{
public:
	CResEditApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResEditApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CResEditApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RESEDIT_H__B94BE54B_C8E4_4E65_AB2F_18F97A6ACF32__INCLUDED_)
