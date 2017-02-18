#pragma once



#include "../public/PEUtility.h"
using namespace PEUtility;
//
#include "../ConnConf.h"
using namespace NInformation;

// CConnectConfigDlg dialog

class CConnectConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConnectConfigDlg)

public:
	CConnectConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConnectConfigDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CONNECT_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioPortDefault();
	afx_msg void OnBnClickedRadioPortUser();

	//
	CString		m_strTagerPath;
	//
	CPEHeader	m_PEHeader;	
	//
	BOOL		m_bIsDynamic;
	//
	CommunicationInfo	m_ConnectInfo;
	//
	//CHAR		m_szBuffer[128];
	//
	BOOL		ReadConfig();
	//
	BOOL		WriteConfig();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedCancel();
};
