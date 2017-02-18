#pragma once


// CGroupDialog dialog

class CGroupDialog : public CDialog
{
	DECLARE_DYNAMIC(CGroupDialog)

public:
	CGroupDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGroupDialog();

// Dialog Data
	enum { IDD = IDD_GROUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strName;
	afx_msg void OnBnClickedOk();
};
