#pragma once


// CModifyCommentDlg dialog

class CModifyCommentDlg : public CDialog
{
	DECLARE_DYNAMIC(CModifyCommentDlg)

public:
	CModifyCommentDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModifyCommentDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MODIFY_COMMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_strComment;
};
