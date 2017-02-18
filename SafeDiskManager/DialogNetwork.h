#pragma once


// CDialogNetwork dialog

class CDialogNetwork : public CDialog
{
	DECLARE_DYNAMIC(CDialogNetwork)

public:
	CDialogNetwork(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogNetwork();

// Dialog Data
	enum { IDD = IDD_DIALOG_NETWORK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
