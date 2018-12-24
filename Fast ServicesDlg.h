// Fast ServicesDlg.h : header file
//

#if !defined(AFX_FASTSERVICESDLG_H__FF08DE49_9085_11D2_82EF_00104B7474A2__INCLUDED_)
#define AFX_FASTSERVICESDLG_H__FF08DE49_9085_11D2_82EF_00104B7474A2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "winsvc.h"
#include "ServicesListCtrl.h"
#include "DlgPreferences.h"
#include "DlgRegister.h"

/////////////////////////////////////////////////////////////////////////////
// CFastServicesDlg dialog

class CFastServicesDlg : public CDialog
{
// Construction
public:
	CFastServicesDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CFastServicesDlg();
	void RefreshServiceList(BOOL bInitialize);
	void EndDialog(int nRetCode);
	BOOL GetRegisteredUser(CString& strRegUser);
	BOOL m_bAllowAutoRefresh;
	CCriticalSection m_csRefreshLock;
	CDlgRegister* m_pDlgRegister;

// Dialog Data
	//{{AFX_DATA(CFastServicesDlg)
	enum { IDD = IDD_FASTSERVICES_DIALOG };
	CServicesListCtrl	m_lstServices;
	CString	m_strStatus;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastServicesDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFastServicesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();	
	afx_msg void OnButtonStart();
	afx_msg void OnClose();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonRefresh();
	afx_msg void OnButtonPause();
	afx_msg void OnButtonContinue();			
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActionPreferences();
	afx_msg void OnActionExit();
	afx_msg void OnActionAboutFastServices();
	afx_msg void OnActionRegister();
	afx_msg void OnViewWin32Services();
	afx_msg void OnViewKernelServices();
	afx_msg void OnViewDriverServices();
	afx_msg void OnViewRelist();
	afx_msg void OnViewShowErrors();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void ShowMessageBox(CString strMessage);
	void EnumAndSendServiceMsg(CString strServiceName,
							   DWORD dwMessage);
	void WaitForStableState(SC_HANDLE hService);	
	BOOL IsActionAppropriate(SC_HANDLE hService,
							 DWORD dwMessage);
	void SendServiceMessage(DWORD dwMessage,
							CString strTypeMessage);
	void ChangeServicesMode(DWORD dwServiceMode, UINT iChoice);	
	void ShowErrors();
	void GetSetRegistry(BOOL bGetFromRegistry = TRUE);

	BOOL m_bRegistrationShown;
	SC_HANDLE m_scManager;
	CDlgPreferences* m_pDlgPreferences;	
	DWORD m_dwServicesType;	
	int m_iControlErrors;	
	int m_iPendingErrors;
	int m_iServiceStateErrors;
	BOOL m_bShowErrors;
	BOOL m_bPreferredWaiting;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTSERVICESDLG_H__FF08DE49_9085_11D2_82EF_00104B7474A2__INCLUDED_)
