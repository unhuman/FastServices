// DlgPreferences.cpp : implementation file
//

#include "stdafx.h"
#include "fast services.h"
#include "DlgPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPreferences dialog


CDlgPreferences::CDlgPreferences(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPreferences::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPreferences)
	m_bAutoRefresh = FALSE;
	m_bAutoRefreshOnlyOnTop = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgPreferences::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPreferences)
	DDX_Control(pDX, IDC_CHECK_AUTOREFRESH_ONLY_ON_TOP, m_chkAutoRefreshOnlyOnTop);
	DDX_Check(pDX, IDC_CHECK_AUTOREFRESH, m_bAutoRefresh);
	DDX_Check(pDX, IDC_CHECK_AUTOREFRESH_ONLY_ON_TOP, m_bAutoRefreshOnlyOnTop);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPreferences, CDialog)
	//{{AFX_MSG_MAP(CDlgPreferences)
	ON_BN_CLICKED(IDC_CHECK_AUTOREFRESH, OnCheckAutoRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPreferences message handlers

void CDlgPreferences::OnCheckAutoRefresh() 
{
	UpdateData();
	
	m_chkAutoRefreshOnlyOnTop.EnableWindow(m_bAutoRefresh);
}

BOOL CDlgPreferences::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnCheckAutoRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgPreferences::DoModal() 
{
	BOOL bBackupRefresh = m_bAutoRefresh;
	BOOL bBackupRefreshOnlyOnTop = m_bAutoRefreshOnlyOnTop;
	
	int iRet = CDialog::DoModal();
	
	if (iRet != IDOK)
	{
		m_bAutoRefresh = bBackupRefresh;
		m_bAutoRefreshOnlyOnTop = bBackupRefreshOnlyOnTop;
	}

	return iRet;
}
