#if !defined(AFX_DLGREGISTER_H__9FD8DD83_9F45_11D2_82F5_00104B7474A2__INCLUDED_)
#define AFX_DLGREGISTER_H__9FD8DD83_9F45_11D2_82F5_00104B7474A2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgRegister.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgRegister dialog

class CDlgRegister : public CDialog
{
// Construction
public:
	enum {EXPIRED, LICENSED, TRIAL};

	CDlgRegister(CWnd* pParent = NULL);   // standard constructor
	int m_iRegistrationStatus;
	
// Dialog Data
	//{{AFX_DATA(CDlgRegister)
	enum { IDD = IDD_REGISTER };
	CString	m_strUserName;
	CString	m_strRegistration;
	CString	m_strExpiration;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRegister)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRegister)
	afx_msg void OnButtonEmail();
	afx_msg void OnButtonWeb();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL RegCode(CString strUser, CString& strPassword);
	void GetSetRegistry(BOOL bGetFromRegistry = TRUE);
	int m_iTrialDaysLeft;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREGISTER_H__9FD8DD83_9F45_11D2_82F5_00104B7474A2__INCLUDED_)
