// Fast ServicesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Process.h"
#include "Fast Services.h"
#include "Fast ServicesDlg.h"
#include "DlgRegister.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TOTAL_SERVICE_MEMORY		12800
#define DEPENDENT_SERVICE_MEMORY	2056
#define STABLE_SLEEP_TIME			125
#define REFRESH_SLEEP_TIME			50
#define REFRESH_SHOW_REGISTER_COUNT	3

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	void DoModal(CFastServicesDlg* pFSDlg);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strRegistrationInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnButtonEmail();
	afx_msg void OnButtonWeb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strRegistrationInfo = _T("Registration is $10.  Please see the registration page.  Please make payment payable to Howard Uman.  Thank you.");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC__REGISTRATION_INFO, m_strRegistrationInfo);
	//}}AFX_DATA_MAP
}

void CAboutDlg::DoModal(CFastServicesDlg* pFSDlg)
{
	if (pFSDlg)
	{
		CString strRegUser(_T(""));
		if (pFSDlg->GetRegisteredUser(strRegUser))
		{
			m_strRegistrationInfo.Format("This copy of Fast Services is registered to:\n\n%s", strRegUser);		
		}
	}
	CDialog::DoModal();
}

void CAboutDlg::OnButtonEmail() 
{
	_spawnlp(_P_NOWAIT, "cmd.exe", "/C start mailto:human@unhuman.com", NULL);	
}

void CAboutDlg::OnButtonWeb() 
{
	_spawnlp(_P_NOWAIT, "cmd.exe", "/C start http://www.unhuman.com", NULL);	
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_BUTTON_EMAIL, OnButtonEmail)
	ON_BN_CLICKED(IDC_BUTTON_WEB, OnButtonWeb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastServicesDlg dialog
UINT AutoRefreshThread(LPVOID pParam)
{
	LONG lThisThread(0);
	static LONG lCounter(0);
	CFastServicesDlg* pFSDlg = (CFastServicesDlg*)pParam;
	lThisThread = InterlockedIncrement(&lCounter);
	while ((pFSDlg->m_bAllowAutoRefresh) &&
		   (lThisThread == lCounter))
	{
		pFSDlg->RefreshServiceList(FALSE);
//		Sleep(REFRESH_SLEEP_TIME);		
	}
	return 0;
}

CFastServicesDlg::CFastServicesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFastServicesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFastServicesDlg)
	m_strStatus = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_scManager = 0;
	m_pDlgPreferences = new CDlgPreferences;
	m_pDlgRegister = new CDlgRegister;
	m_bRegistrationShown = FALSE;
	m_dwServicesType = SERVICE_WIN32;
	m_bAllowAutoRefresh = FALSE;
	m_iControlErrors = 0;
	m_iPendingErrors = 0;
	m_iServiceStateErrors = 0;
	m_bShowErrors = TRUE;
	m_bAllowAutoRefresh = FALSE;
	m_bPreferredWaiting = FALSE;
}

CFastServicesDlg::~CFastServicesDlg()
{	
	if (m_scManager)
		CloseServiceHandle(m_scManager);
	if (m_pDlgPreferences)
		delete m_pDlgPreferences;
	if (m_pDlgRegister)
		delete m_pDlgRegister;
}


void CFastServicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFastServicesDlg)
	DDX_Control(pDX, IDC_LIST_SERVICES, m_lstServices);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFastServicesDlg, CDialog)
	//{{AFX_MSG_MAP(CFastServicesDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_CONTINUE, OnButtonContinue)		
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_ACTION_PREFERENCES, OnActionPreferences)
	ON_COMMAND(ID_ACTION_EXIT, OnActionExit)
	ON_COMMAND(ID_ACTION_ABOUTFASTSERVICES, OnActionAboutFastServices)
	ON_COMMAND(ID_ACTION_REGISTER, OnActionRegister)
	ON_COMMAND(ID_VIEW_WIN32_SERVICES, OnViewWin32Services)
	ON_COMMAND(ID_VIEW_KERNEL_SERVICES, OnViewKernelServices)
	ON_COMMAND(ID_VIEW_FILE_SYSTEM_SERVICES, OnViewDriverServices)
	ON_COMMAND(ID_VIEW_RELIST, OnViewRelist)
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_VIEW_SHOWERRORS, OnViewShowErrors)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastServicesDlg message handlers

BOOL CFastServicesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!m_scManager)
	{
		ShowMessageBox("Could not open Service Control Manager.  Terminating.");
		EndDialog(0);
		return TRUE;
	}

	m_lstServices.InitializeList();

	// Original Refresh now happens in OnPaint
	// along with registry retrieval.
	// This lets the dialog be displayed before
	// the data - making the program start up faster

	// Remove Preferences from the menu
	CMenu* pMenu = GetMenu();
	if (pMenu)
		pMenu->RemoveMenu(ID_ACTION_PREFERENCES, MF_BYCOMMAND);

	// Start the timer if we're auto-refreshing
	if (m_pDlgPreferences->m_bAutoRefresh)
	{
		m_bAllowAutoRefresh = TRUE;
		AfxBeginThread(AutoRefreshThread, this);
	}	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFastServicesDlg::EndDialog(int nRetCode)
{
	m_strStatus = _T("Terminating...");
	UpdateData(FALSE);

	m_lstServices.ShowWindow(FALSE);
	m_lstServices.DeleteAllItems();
	CDialog::EndDialog(nRetCode);
}

void CFastServicesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal(this);
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFastServicesDlg::OnPaint() 
{
	static BOOL bShown(FALSE);

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	if (!bShown)
	{
		bShown = TRUE;
		GetSetRegistry(TRUE);
	}

	if (!m_bRegistrationShown)
	{
		m_bRegistrationShown = TRUE;
		OnActionRegister();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFastServicesDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFastServicesDlg::RefreshServiceList(BOOL bInitialize) 
{		
	if (!bInitialize && m_bPreferredWaiting)
		return;
	
	if (!m_scManager)
	{
		ShowMessageBox("SC Manager not initialized.  Terminating.");
		EndDialog(0);
	}			

	// Turn off a preferred requestor is waiting
	if (bInitialize)
		m_bPreferredWaiting = TRUE;

	CSingleLock lockRefresh(&m_csRefreshLock);
	lockRefresh.Lock();

	if (bInitialize)
	{
		m_strStatus = _T("Working...");
		UpdateData(FALSE);

		m_lstServices.DeleteAllItems();
	}

	DWORD dwBytesNeeded(0);
	DWORD dwServicesReturned(0);
	DWORD dwResumeHandle(0);
	BOOL bSuccess(FALSE);
	ENUM_SERVICE_STATUS* pServices = (ENUM_SERVICE_STATUS*)malloc(TOTAL_SERVICE_MEMORY);

	do
	{
		// If we couldn't get any services and we haven't gotten
		// any to start with, error out
		if ((!EnumServicesStatus(m_scManager, m_dwServicesType, 
								 SERVICE_STATE_ALL, pServices, 
								 TOTAL_SERVICE_MEMORY, &dwBytesNeeded, 
								 &dwServicesReturned,
 								 &dwResumeHandle)) && 
			(dwServicesReturned == 0) &&
			(!bSuccess))
		{
			ShowMessageBox("Error retrieving services.  Terminating.");
			free(pServices);

			// Unlock if we had a lock
			lockRefresh.Unlock();

			EndDialog(0);
		}
		// If we've gotten here, then we're fine
		bSuccess = TRUE;

		// Sweep through the services we've loaded and add them
		// to the list control
		ENUM_SERVICE_STATUS* pServicesLoop = pServices;		
		for (DWORD i=0; i<dwServicesReturned; i++)
		{
			// Drop out of loop if we're not preferred and there
			// is a preferred thread waiting
			if (!bInitialize && m_bPreferredWaiting)
				break;

			DWORD dwStatus = pServicesLoop->ServiceStatus.dwCurrentState;
			
			if (bInitialize)
			{
				m_lstServices.AddItem(dwStatus, 
									  pServicesLoop->lpDisplayName,
									  pServicesLoop->lpServiceName,
									  "Unknown");
			}
			else
			{
				// If we can't update the item, add it.
				if (!m_lstServices.UpdateItem(dwStatus,
											  pServicesLoop->lpDisplayName,										 
											  pServicesLoop->lpServiceName,
											  "Unknown"))
				{
					m_lstServices.AddItem(dwStatus, 
										  pServicesLoop->lpDisplayName,
										  pServicesLoop->lpServiceName,
										  "Unknown");
				}
			}			
			
			pServicesLoop++;
		}		

		// Drop out of loop if we're not preferred and there
		// is a preferred thread waiting
		if (!bInitialize && m_bPreferredWaiting)
			break;
	} while (dwBytesNeeded > 0);

	free(pServices);

	if (bInitialize)
	{
		m_strStatus = _T("");
		UpdateData(FALSE);
	}

	// Unlock
	lockRefresh.Unlock();

	// Turn off a preferred requestor is waiting
	if (bInitialize)
		m_bPreferredWaiting = FALSE;
}

BOOL CFastServicesDlg::GetRegisteredUser(CString& strRegUser)
{
	BOOL bRegistered(FALSE);
	if (m_pDlgRegister->m_iRegistrationStatus == CDlgRegister::LICENSED)
	{
		bRegistered = TRUE;
		strRegUser = m_pDlgRegister->m_strUserName; 
	}

	return bRegistered;
}

void CFastServicesDlg::OnButtonRefresh() 
{
	static int iCounter(0);
	if (++iCounter == REFRESH_SHOW_REGISTER_COUNT)
	{
		iCounter = 0;
		CFastServicesDlg::OnActionRegister();
	}

	m_strStatus = _T("Working...");
	UpdateData(FALSE);

	RefreshServiceList(FALSE);

	m_strStatus = _T("");
	UpdateData(FALSE);
}

BOOL CFastServicesDlg::IsActionAppropriate(SC_HANDLE hService,
										   DWORD dwMessage)
{
	BOOL bAppropriate(FALSE);
	SERVICE_STATUS servStatus;

	// Get service status
	if (!QueryServiceStatus(hService, &servStatus))
	{
		m_iServiceStateErrors++;
		return FALSE;
	}
	switch(dwMessage)
	{
		case SERVICE_CONTROL_STOP:
			if ((servStatus.dwCurrentState == SERVICE_RUNNING) ||
				(servStatus.dwCurrentState == SERVICE_PAUSED))
				bAppropriate = TRUE;
			break;
		case SERVICE_CONTROL_PAUSE:
			if (servStatus.dwCurrentState == SERVICE_RUNNING)
				bAppropriate = TRUE;
			break;
		case SERVICE_CONTROL_CONTINUE:
			if (servStatus.dwCurrentState == SERVICE_PAUSED)
				bAppropriate = TRUE;
			break;
		default: // Startup
			if (servStatus.dwCurrentState == SERVICE_STOPPED)
				bAppropriate = TRUE;
			break;
	}

	// Error for any in-process services
	if ((servStatus.dwCurrentState == SERVICE_START_PENDING) ||
		(servStatus.dwCurrentState == SERVICE_STOP_PENDING) ||
		(servStatus.dwCurrentState == SERVICE_CONTINUE_PENDING) ||
		(servStatus.dwCurrentState == SERVICE_PAUSE_PENDING))
		m_iPendingErrors++;

	return bAppropriate;
}


// Waits until the service is in a stable setting
// ie started, stopped, or paused
void CFastServicesDlg::WaitForStableState(SC_HANDLE hService)
{
	BOOL bStable(FALSE);
	SERVICE_STATUS servStatus;
	DWORD dwServiceState(0);
	while (!bStable)
	{
		Sleep(STABLE_SLEEP_TIME);

		if (!QueryServiceStatus(hService, &servStatus))
		{
			ShowMessageBox("Error: Problem getting service state.");
			break;
		}
		RefreshServiceList(FALSE);

		dwServiceState = servStatus.dwCurrentState;
		if ((dwServiceState == SERVICE_STOPPED) ||
			(dwServiceState == SERVICE_RUNNING) ||
			(dwServiceState == SERVICE_PAUSED))
			break;		
	}
}

// This function will recursively stop other services required by a
// service to stop.
void CFastServicesDlg::EnumAndSendServiceMsg(CString strServiceName,
											 DWORD dwMessage)
{
	SC_HANDLE hService = OpenService(m_scManager, strServiceName,
									 SERVICE_ALL_ACCESS);
	// Ensure this service is valid and it's not already what we're asking for
	if ((hService) && IsActionAppropriate(hService, dwMessage))
	{
		// Only enumerate dependent services for a stop request
		if (dwMessage == SERVICE_CONTROL_STOP)
		{
			// Find out what the dependant services are, and stop them
			// by calling this function
			ENUM_SERVICE_STATUS* pServices = (ENUM_SERVICE_STATUS*)malloc(DEPENDENT_SERVICE_MEMORY);
			DWORD dwBytesNeeded(0);
			DWORD dwServicesReturned(0);

			while ((EnumDependentServices(hService, SERVICE_ACTIVE,
										  pServices, 
										  DEPENDENT_SERVICE_MEMORY,
										  &dwBytesNeeded, 
										  &dwServicesReturned)) &&
				   (dwServicesReturned > 0))
			{
				ENUM_SERVICE_STATUS* pServicesLoop = pServices;
				for (DWORD i=0; i<dwServicesReturned; i++)
				{
					EnumAndSendServiceMsg(pServicesLoop->lpServiceName,
										  dwMessage);
					pServicesLoop++;					
				}

				// Default to zero dependent services for next round
				dwServicesReturned = 0;
			}
			free (pServices);
		}

		SERVICE_STATUS latestStatus;
		if (!ControlService(hService, dwMessage, &latestStatus))
			m_iControlErrors++;
		else
		{			
			WaitForStableState(hService);
			RefreshServiceList(FALSE);
		}

		CloseServiceHandle(hService);
	}
}

void CFastServicesDlg::ShowErrors()
{
	if (m_bShowErrors == TRUE)
	{
		CString strErrors(_T(""));

		if (m_iControlErrors)
		{
			strErrors.Format("Error processing %d requests.", 
							 m_iControlErrors);
		}

		if (m_iPendingErrors)
		{
			if (strErrors.GetLength() > 0)
				strErrors = strErrors + "\n";
			strErrors.Format("%sCould not process %d pending processes.", 
							 strErrors, m_iPendingErrors);
		}
		if (m_iServiceStateErrors)
		{
			if (strErrors.GetLength() > 0)
				strErrors = strErrors + "\n";
			strErrors.Format("%sCould not process %d processes in pending states.", 
							 strErrors, m_iServiceStateErrors);
		}

		if (strErrors.GetLength() > 0)
		{
			strErrors = "Service Errors:\n\n" + strErrors;
			AfxMessageBox(strErrors);
		}
	}
}

void CFastServicesDlg::OnButtonStart() 
{
	unsigned int iHandled(0);
	int iItem(0);
	unsigned int iState(0);

	m_iControlErrors = 0;
	m_iPendingErrors = 0;
	m_iServiceStateErrors = 0;

	if (m_lstServices.GetSelectedCount() > 0)
	{
		m_strStatus = _T("Working...");
		UpdateData(FALSE);

		int iImage(0);
		CString strServiceDisplayName(_T(""));
		CString strStatus(_T(""));
		CString strStartup(_T(""));
		CString strServiceName(_T(""));

		while ((iHandled < m_lstServices.GetSelectedCount()) &&
			   (iItem < m_lstServices.GetItemCount()))
		{
			iState = m_lstServices.GetItemState(iItem, LVIS_SELECTED);
			if (iState && 
				m_lstServices.RetrieveItem(iItem, iImage, 
										   strServiceDisplayName,
										   strStatus, strStartup,
										   strServiceName))
			{
				SC_HANDLE hService = OpenService(m_scManager, strServiceName,
												 SERVICE_ALL_ACCESS);
				// If the service is valid and it's not started
				if ((hService) && IsActionAppropriate(hService, 983284)) // Goofy value for starting
				{
					if (!StartService(hService, NULL, NULL))
						m_iControlErrors++;
					else
						WaitForStableState(hService);
					CloseServiceHandle(hService);
				}

				iHandled++;
			}
			iItem++;
		}
		RefreshServiceList(FALSE);

		// Clear status
		m_strStatus = _T("");
		UpdateData(FALSE);

		ShowErrors();
	}
	else
	{
		// Clear status
		m_strStatus = _T("");
		UpdateData(FALSE);
	
		ShowMessageBox("You must select a service to start.");
	}
}

void CFastServicesDlg::OnClose() 
{
	m_lstServices.DeleteAllItems();

	GetSetRegistry(FALSE);
	
	CDialog::OnClose();
}

void CFastServicesDlg::SendServiceMessage(DWORD dwMessage,
										  CString strTypeMessage)
{
	unsigned int iHandled(0);
	int iItem(0);
	unsigned int iState(0);

	m_iControlErrors = 0;
	m_iPendingErrors = 0;
	m_iServiceStateErrors = 0;

	if (m_lstServices.GetSelectedCount() > 0)
	{
		m_strStatus = _T("Working...");
		UpdateData(FALSE);

		int iImage(0);
		CString strServiceDisplayName(_T(""));
		CString strStatus(_T(""));
		CString strStartup(_T(""));
		CString strServiceName(_T(""));

		while ((iHandled < m_lstServices.GetSelectedCount()) &&
			   (iItem < m_lstServices.GetItemCount()))
		{
			iState = m_lstServices.GetItemState(iItem, LVIS_SELECTED);
			if (iState && 
				m_lstServices.RetrieveItem(iItem, iImage, 
										   strServiceDisplayName,
										   strStatus, strStartup,
										   strServiceName))
			{
				EnumAndSendServiceMsg(strServiceName, dwMessage);
				iHandled++;
			}
			iItem++;
		}
		RefreshServiceList(FALSE);

		m_strStatus = _T("");
		UpdateData(FALSE);

		ShowErrors();
	}
	else
	{
		// Clear status
		m_strStatus = _T("");
		UpdateData(FALSE);

		CString strMessage(_T(""));
		strMessage.Format("You must select a service to %s.", strTypeMessage);
		ShowMessageBox(strMessage);
	}
}

void CFastServicesDlg::OnButtonStop() 
{
	SendServiceMessage(SERVICE_CONTROL_STOP, "stop");
}

void CFastServicesDlg::OnButtonPause() 
{
	SendServiceMessage(SERVICE_CONTROL_PAUSE, "pause");
}

void CFastServicesDlg::OnButtonContinue() 
{
	SendServiceMessage(SERVICE_CONTROL_CONTINUE, "continue");
}

void CFastServicesDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	
	// The timer is only disabled or (re)enabled when
	// both AutoRefresh and AutoRefreshOnlyOnTop are set
	if (nState == WA_INACTIVE)
	{
		// If auto refresh when only on top, and we are
		// no longer on top, turn off allowing autorefresh
		if ((m_pDlgPreferences->m_bAutoRefresh) &&
			(m_pDlgPreferences->m_bAutoRefreshOnlyOnTop))
		{
			m_bAllowAutoRefresh = FALSE;
		}
	}
	else
	{
		// We always refresh on activate
		RefreshServiceList(FALSE);

		// Start a thread if we're supposed to refresh
		// only when on top
		if ((m_pDlgPreferences->m_bAutoRefresh) &&
			(m_pDlgPreferences->m_bAutoRefreshOnlyOnTop))
		{
			m_bAllowAutoRefresh = TRUE;
			AfxBeginThread(AutoRefreshThread, this);
		}
	}
}

void CFastServicesDlg::OnActionPreferences() 
{
	if (m_pDlgPreferences->DoModal() == IDOK)
	{
		// Start the timer if we're auto-refreshing
		if ((m_pDlgPreferences->m_bAutoRefresh) && 
			!m_bAllowAutoRefresh)
		{
			// Start a thread if we're supposed to auto refresh
			m_bAllowAutoRefresh = TRUE;
			AfxBeginThread(AutoRefreshThread, this);
		}
		else // Turn off auto refreshing
		{
			m_bAllowAutoRefresh = FALSE;
		}
	}
}

void CFastServicesDlg::OnActionExit() 
{
	EndDialog(0);
}

void CFastServicesDlg::OnActionAboutFastServices() 
{
	CAboutDlg dlgAbout;

	dlgAbout.DoModal(this);	
}

BOOL CFastServicesDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN) && 
		((pMsg->wParam == VK_RETURN) ||
		 (pMsg->wParam == VK_ESCAPE)))
		return TRUE;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CFastServicesDlg::ShowMessageBox(CString strMessage)
{
	MessageBox(strMessage, "Fast Services", MB_ICONINFORMATION);
}

void CFastServicesDlg::ChangeServicesMode(DWORD dwServiceMode, UINT iChoice)
{	
	CMenu* pMenu = GetMenu();
	
	if (pMenu)
	{
		if (pMenu->GetMenuState(iChoice, MF_BYCOMMAND) ^ MF_CHECKED)
		{
			// Set the refresh mode
			m_dwServicesType = dwServiceMode;

			// Update the checkboxes in the menu
			pMenu->CheckMenuItem(ID_VIEW_WIN32_SERVICES, MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_VIEW_KERNEL_SERVICES, MF_UNCHECKED);
			pMenu->CheckMenuItem(ID_VIEW_FILE_SYSTEM_SERVICES, MF_UNCHECKED);
			pMenu->CheckMenuItem(iChoice, MF_CHECKED);
			
			// Refresh the list
			RefreshServiceList(TRUE);
		}
	}
}

void CFastServicesDlg::OnViewWin32Services() 
{	
	ChangeServicesMode(SERVICE_WIN32, ID_VIEW_WIN32_SERVICES);
}

void CFastServicesDlg::OnViewKernelServices() 
{	
	ChangeServicesMode(SERVICE_KERNEL_DRIVER, ID_VIEW_KERNEL_SERVICES);
}

void CFastServicesDlg::OnViewDriverServices() 
{
	ChangeServicesMode(SERVICE_FILE_SYSTEM_DRIVER, ID_VIEW_FILE_SYSTEM_SERVICES);
}

void CFastServicesDlg::OnViewRelist() 
{
	// Refresh the list
	RefreshServiceList(TRUE);
}

void CFastServicesDlg::OnActionRegister() 
{	
	// If we're not registered, then show the register dialog
	if (m_pDlgRegister->m_iRegistrationStatus != CDlgRegister::LICENSED)
	{
		m_pDlgRegister->DoModal();
		if (m_pDlgRegister->m_iRegistrationStatus == CDlgRegister::EXPIRED)		
		{
			AfxMessageBox("Fast Services trial period has expired.\nYou must purchase a registration code to continue use.\n\nFast Services will now terminate.");		
			EndDialog(0);
		}
	}

	// If the registration works, then
	// configure registry and disable the
	// register menu item
	if (m_pDlgRegister->m_iRegistrationStatus == CDlgRegister::LICENSED)
	{
		CMenu* pMenu = GetMenu();
		if (pMenu)
		{
			pMenu->RemoveMenu(ID_ACTION_REGISTER, MF_BYCOMMAND);
		}
	}
}

void CFastServicesDlg::OnViewShowErrors() 
{
	CMenu* pMenu = GetMenu();
	if (pMenu)
	{
		if (pMenu->GetMenuState(ID_VIEW_SHOWERRORS, MF_BYCOMMAND) ^ MF_CHECKED)
		{
			pMenu->CheckMenuItem(ID_VIEW_SHOWERRORS, MF_CHECKED);
			m_bShowErrors = TRUE;
		}
		else
		{
			pMenu->CheckMenuItem(ID_VIEW_SHOWERRORS, MF_UNCHECKED);
			m_bShowErrors = FALSE;
		}
	}
}

void CFastServicesDlg::GetSetRegistry(BOOL bGetFromRegistry /*TRUE*/)
{
	COXRegistry regFastServices;	

	if (regFastServices.Connect(COXRegistry::keyLocalMachine))
	{
		CString strRegKey(_T(""));
		CString strWinTime(_T(""));
		CString strDDTime(_T(""));
		CString strRegTime(_T(""));
		strRegKey.LoadString(IDS_REGISTRY_KEY);
		if (regFastServices.CheckAndOpenKeyForWrite(strRegKey))
		{	if (bGetFromRegistry)
			{
				DWORD dwRegValue(0);
				if (regFastServices.GetValue(_T("ShowErrors"), dwRegValue))
					m_bShowErrors = dwRegValue;

				// If we don't view errors, flip the button
				if (!m_bShowErrors)
					OnViewShowErrors();
				
				if (regFastServices.GetValue(_T("Win32ServicesView"), dwRegValue) && dwRegValue)
					ChangeServicesMode(SERVICE_WIN32, ID_VIEW_WIN32_SERVICES);

				if (regFastServices.GetValue(_T("KernelServicesView"), dwRegValue) && dwRegValue)
					ChangeServicesMode(SERVICE_KERNEL_DRIVER, ID_VIEW_KERNEL_SERVICES);
				
				if (regFastServices.GetValue(_T("FileSystemServicesView"), dwRegValue) && dwRegValue)
					ChangeServicesMode(SERVICE_FILE_SYSTEM_DRIVER, ID_VIEW_FILE_SYSTEM_SERVICES);
			}
			else
			{
				UpdateData();
				regFastServices.SetValue("ShowErrors", m_bShowErrors);

				CMenu* pMenu = GetMenu();
				if (pMenu)
				{
					BOOL bChecked(FALSE);
					bChecked = !(pMenu->GetMenuState(ID_VIEW_WIN32_SERVICES, MF_BYCOMMAND) ^ MF_CHECKED);
					regFastServices.SetValue("Win32ServicesView", bChecked);
					bChecked = !(pMenu->GetMenuState(ID_VIEW_KERNEL_SERVICES, MF_BYCOMMAND) ^ MF_CHECKED);
					regFastServices.SetValue("KernelServicesView", bChecked);
					bChecked = !(pMenu->GetMenuState(ID_VIEW_FILE_SYSTEM_SERVICES, MF_BYCOMMAND) ^ MF_CHECKED);
					regFastServices.SetValue("FileSystemServicesView", bChecked);
				}
			}
		}
		regFastServices.Disconnect();
	}
}
