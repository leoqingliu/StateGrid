#pragma once


// CDialogPolicy dialog

#define LODWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xff))
#define MIDDWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 8) & 0xff))
#define HIDWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xff))

#include <vector>
#include "afxdtctl.h"

class CDialogPolicy : public CDialog
{
	DECLARE_DYNAMIC(CDialogPolicy)

public:
	CDialogPolicy(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogPolicy();

// Dialog Data
	enum { IDD = IDD_DIALOG_POLICY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strUserName;
	CString m_strCompany;
	CString m_strNumber;
	CString m_strId;
	COleDateTime m_timeStart;
	COleDateTime m_timeEnd;
	DWORD m_dwStart;
	DWORD m_dwEnd;
	afx_msg void OnBnClickedOk();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	CDateTimeCtrl m_PickerStart;
	CDateTimeCtrl m_PickerEnd;
	COleDateTime m_dateStart;
	COleDateTime m_dateEnd;
};
