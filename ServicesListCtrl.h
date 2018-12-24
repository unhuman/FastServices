#if !defined(AFX_SERVICESLISTCTRL_H__39D07095_910B_11D2_82F0_00104B7474A2__INCLUDED_)
#define AFX_SERVICESLISTCTRL_H__39D07095_910B_11D2_82F0_00104B7474A2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ServicesListCtrl.h : header file
//

#include "winsvc.h"

/////////////////////////////////////////////////////////////////////////////
// CServicesListCtrl window

class CServicesListCtrl : public CListCtrl
{
// Construction
public:
	CServicesListCtrl();

// Attributes
public:

// Operations
public:
	BOOL AddItem(DWORD dwStatus, CString strServiceDisplayName,
				 CString strServiceName, CString strStartup);
	BOOL UpdateItem(DWORD dwStatus, CString strServiceDisplayName,
					CString strServiceName, CString strStartup);
	BOOL RetrieveItem(int iItemRet, int& iImage, 
					  CString& strServiceDisplayName,
					  CString& strStatus, CString& strStartup,
					  CString& strServiceName);
	int GetItemStatus(int iItem);
	BOOL DeleteItem(int nItem);
	BOOL DeleteAllItems();	

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnPaint();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServicesListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitializeList();
	virtual ~CServicesListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CServicesListCtrl)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_iChanging;
	int m_iStopped;
	int m_iStarted;
	int m_iUnknown;
	int m_iPaused;
	CImageList m_cImageList;


public:
	enum EHighlight {HIGHLIGHT_NORMAL, HIGHLIGHT_ALLCOLUMNS, HIGHLIGHT_ROW};
protected:
	int  m_nHighlight;              // Indicate type of selection highlighting
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVICESLISTCTRL_H__39D07095_910B_11D2_82F0_00104B7474A2__INCLUDED_)
