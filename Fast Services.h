// Fast Services.h : main header file for the FAST SERVICES application
//

#if !defined(AFX_FASTSERVICES_H__FF08DE47_9085_11D2_82EF_00104B7474A2__INCLUDED_)
#define AFX_FASTSERVICES_H__FF08DE47_9085_11D2_82EF_00104B7474A2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFastServicesApp:
// See Fast Services.cpp for the implementation of this class
//

class CFastServicesApp : public CWinApp
{
public:
	CFastServicesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastServicesApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFastServicesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL FirstInstance();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTSERVICES_H__FF08DE47_9085_11D2_82EF_00104B7474A2__INCLUDED_)
