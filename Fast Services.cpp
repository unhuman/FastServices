// Fast Services.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Fast Services.h"
#include "Fast ServicesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFastServicesApp

BEGIN_MESSAGE_MAP(CFastServicesApp, CWinApp)
	//{{AFX_MSG_MAP(CFastServicesApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastServicesApp construction

CFastServicesApp::CFastServicesApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFastServicesApp object

CFastServicesApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFastServicesApp initialization

BOOL CFastServicesApp::InitInstance()
{
	// If a previous instance of the application is already running,
	// then activate it and return FALSE from InitInstance to
	// end the execution of this instance.
	if(!FirstInstance())
		return FALSE;
	        
	// Continue initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CFastServicesDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CFastServicesApp::FirstInstance()
{
	CWnd *pWndPrev, *pWndChild;
	// Determine if another window with our class name exists...
	if (pWndPrev = CWnd::FindWindow(NULL, "Fast Services"))
	{
		// if so, does it have any popups?
		pWndChild = pWndPrev->GetLastActivePopup();
		
		// If iconic, restore the main window
		if (pWndPrev->IsIconic())
			pWndPrev->ShowWindow(SW_RESTORE);
		
		// Bring the main window or its popup to
		// the foreground		
		pWndChild->SetForegroundWindow();

		// and we are done activating the previous one.
		return FALSE;
	}
	else
		return TRUE;
}
