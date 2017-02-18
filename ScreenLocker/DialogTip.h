#pragma once


// CDialogTip dialog

class CDialogTip : public CDialog
{
	DECLARE_DYNAMIC(CDialogTip)

public:
	CDialogTip(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogTip();

// Dialog Data
	enum { IDD = IDD_DIALOG_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
