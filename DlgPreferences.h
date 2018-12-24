#if !defined(AFX_DLGPREFERENCES_H__2E29A1B3_96BF_11D2_82F2_00104B7474A2__INCLUDED_)
#define AFX_DLGPREFERENCES_H__2E29A1B3_96BF_11D2_82F2_00104B7474A2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgPreferences.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPreferences dialog

class CDlgPreferences : public CDialog
{
// Construction
public:
	CDlgPreferences(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPreferences)
	enum { IDD = IDD_DIALOG_PREFERENCE };
	CButton	m_chkAutoRefreshOnlyOnTop;
	BOOL	m_bAutoRefresh;
	BOOL	m_bAutoRefreshOnlyOnTop;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPreferences)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPreferences)
	afx_msg void OnCheckAutoRefresh();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPREFERENCES_H__2E29A1B3_96BF_11D2_82F2_00104B7474A2__INCLUDED_)
