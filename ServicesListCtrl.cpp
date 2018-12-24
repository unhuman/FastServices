// ServicesListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Fast Services.h"
#include "ServicesListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NUMCOLS		2
#define IMAGE_MARGIN	2

/////////////////////////////////////////////////////////////////////////////
// CServicesListCtrl

CServicesListCtrl::CServicesListCtrl()
{
	m_iChanging = 0;
	m_iStopped = 0;
	m_iStarted = 0;
	m_iUnknown = 0;
	m_iPaused = 0;

	m_nHighlight = 2;
}

CServicesListCtrl::~CServicesListCtrl()
{	
}


BEGIN_MESSAGE_MAP(CServicesListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CServicesListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServicesListCtrl message handlers

void CServicesListCtrl::InitializeList()
{
	// Setup the Services List Box
	InsertColumn(0, "Service", LVCFMT_LEFT, 285);
	InsertColumn(1, "Status", LVCFMT_LEFT, 75);
//	InsertColumn(2, "Startup", LVCFMT_LEFT, 75);

	m_cImageList.Create(16, 16, TRUE, 5, 1);
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP_UNKNOWN);
	m_iUnknown = m_cImageList.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP_STARTED);
	m_iStarted = m_cImageList.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP_STOPPED);
	m_iStopped = m_cImageList.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP_PAUSED);
	m_iPaused = m_cImageList.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	bitmap.LoadBitmap(IDB_BITMAP_CHANGING);
	m_iChanging = m_cImageList.Add(&bitmap, (COLORREF)0xFFFFFF);
	bitmap.DeleteObject();
	SetImageList(&m_cImageList, LVSIL_SMALL);
}

BOOL CServicesListCtrl::AddItem(DWORD dwStatus, 
								CString strServiceDisplayName,
								CString strServiceName,
								CString strStartup)
{
	BOOL bSuccess = TRUE;
	int iLoc = GetItemCount();
	int iStatusImage = m_iUnknown;
	CString strStatus(_T("Unknown"));

	switch (dwStatus)
	{
		case SERVICE_STOPPED:
			iStatusImage = m_iStopped;
			strStatus = _T("Stopped");
			break;
		case SERVICE_RUNNING:
			iStatusImage = m_iStarted;
			strStatus = _T("Started");
			break;		
		case SERVICE_PAUSED:
			iStatusImage = m_iPaused;
			strStatus = _T("Paused");
			break;
		case SERVICE_START_PENDING:
			strStatus = _T("Start Pending");
			iStatusImage = m_iChanging;			
			break;
		case SERVICE_STOP_PENDING:
			strStatus = _T("Stop Pending");
			iStatusImage = m_iChanging;			
			break;
		case SERVICE_CONTINUE_PENDING:
			strStatus = _T("Continue Pending");
			iStatusImage = m_iChanging;			
			break;
		case SERVICE_PAUSE_PENDING:
			strStatus = _T("Pause Pending");
			iStatusImage = m_iChanging;			
			break;
	}
		
	LV_ITEM lvItem;
	lvItem.iItem = iLoc;
	lvItem.iSubItem = 0; 
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	lvItem.state = LVIF_IMAGE | LVIF_TEXT;	
	lvItem.iImage = iStatusImage;
	lvItem.pszText = strServiceDisplayName.GetBuffer(strServiceDisplayName.GetLength());
	iLoc = InsertItem(&lvItem);
	strServiceDisplayName.ReleaseBuffer();	
	
	SetItemText(iLoc, 1, strStatus);
//	SetItemText(iLoc, 2, strStartup);
	SetItemData(iLoc, (unsigned long)new CString(strServiceName));

	return bSuccess;
}

BOOL CServicesListCtrl::UpdateItem(DWORD dwStatus, 
								   CString strServiceDisplayName,
								   CString strServiceName,
								   CString strStartup)
{
	int iImage(0);
	CString strCompDisplayName(_T(""));
	CString strCompStatus(_T(""));
	CString strCompStartup(_T(""));
	CString strCompServiceName(_T(""));

	BOOL bFound(FALSE);
	int iCount = GetItemCount();
	int iLoc(-1);
	
	CString strFindDispName(strServiceDisplayName);
	LV_FINDINFO findInfo;
	findInfo.flags = LVFI_STRING;
	findInfo.psz = strFindDispName;
	findInfo.lParam = NULL;
	while ((iLoc = FindItem(&findInfo, iLoc)) != -1)	
	{
		if (RetrieveItem(iLoc, iImage, strCompDisplayName,
						 strCompStatus, strCompStartup, 
						 strCompServiceName) &&
			(strServiceName == strCompServiceName))
		{

			int iStatusImage = m_iUnknown;
			CString strStatus(_T("Unknown"));

			switch (dwStatus)
			{
				case SERVICE_STOPPED:
					iStatusImage = m_iStopped;
					strStatus = _T("Stopped");
					break;
				case SERVICE_RUNNING:
					iStatusImage = m_iStarted;
					strStatus = _T("Started");
					break;		
				case SERVICE_PAUSED:
					iStatusImage = m_iPaused;
					strStatus = _T("Paused");
					break;
				case SERVICE_START_PENDING:
					strStatus = _T("Start Pending");
					iStatusImage = m_iChanging;			
					break;
				case SERVICE_STOP_PENDING:
					strStatus = _T("Stop Pending");
					iStatusImage = m_iChanging;			
					break;
				case SERVICE_CONTINUE_PENDING:
					strStatus = _T("Continue Pending");
					iStatusImage = m_iChanging;			
					break;
				case SERVICE_PAUSE_PENDING:
					strStatus = _T("Pause Pending");
					iStatusImage = m_iChanging;			
					break;
			}
					
			// Only update this guy if we're changing status
			if (iStatusImage != iImage)
			{
				LV_ITEM lvItem;
				lvItem.iItem = iLoc;
				lvItem.iSubItem = 0; 
				lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
				lvItem.state = LVIF_IMAGE | LVIF_TEXT;	
				lvItem.iImage = iStatusImage;
				lvItem.pszText = strServiceDisplayName.GetBuffer(strServiceDisplayName.GetLength());
				SetItem(&lvItem);
				strServiceDisplayName.ReleaseBuffer();	
				
				SetItemText(iLoc, 1, strStatus);
				//SetItemText(iLoc, 2, strStartup);

				CString* pStrServiceName = (CString*)GetItemData(iLoc);
				if (pStrServiceName)
					delete pStrServiceName;

				SetItemData(iLoc, (unsigned long)new CString(strServiceName));
			}

			bFound = TRUE;
			break;
		}
	}	

	return bFound;
}


BOOL CServicesListCtrl::RetrieveItem(int iItemRet, int& iImage, 
									 CString& strServiceDisplayName,
									 CString& strStatus, 
									 CString& strStartup,
									 CString& strServiceName)
{
	if (iItemRet >=  GetItemCount())
		return FALSE;

	iImage = GetItemStatus(iItemRet);
	strServiceDisplayName = GetItemText(iItemRet, 0);	
	strStatus	= GetItemText(iItemRet, 1);
//	strStartup	= GetItemText(iItemRet, 2);
	strStartup = _T("");
	CString* pStrServiceName = (CString*)GetItemData(iItemRet);
	strServiceName = *pStrServiceName;

	return TRUE;
}

int CServicesListCtrl::GetItemStatus(int iItem)
{
	LV_ITEM lvItem;
	lvItem.iItem = iItem;
	lvItem.iSubItem  = 0; 
	lvItem.mask = LVIF_IMAGE;
	GetItem(&lvItem);

	return lvItem.iImage;
}

BOOL CServicesListCtrl::DeleteItem(int nItem)
{
	CString* pStrPW = (CString*)GetItemData(nItem);
	if (pStrPW)
	{
		delete pStrPW;
		pStrPW = NULL;
		SetItemData(nItem, NULL);
	}

	return CListCtrl::DeleteItem(nItem);
}

BOOL CServicesListCtrl::DeleteAllItems()
{
	while (GetItemCount() > 0)
	{
		DeleteItem(0);
	}
	return CListCtrl::DeleteAllItems();
}

void CServicesListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    //CActivity* pActivity = (CActivity*)lpDrawItemStruct->itemData;
    //ASSERT(pActivity);

    CDC* pdc;

	CRect rectListCtrl;
	GetClientRect(&rectListCtrl);

//	if (lpDrawItemStruct->rcItem.right < rectListCtrl.right)
//	{		
//		rectListCtrl.top = lpDrawItemStruct->rcItem.top;
//		rectListCtrl.bottom = lpDrawItemStruct->rcItem.bottom;
//		rectListCtrl.left = 0;
//		InvalidateRect(rectListCtrl);		
//	}

    CRect rc(&lpDrawItemStruct->rcItem);
    CRect rcFocus(rc);
    CString str;
	
	pdc = CDC::FromHandle(lpDrawItemStruct->hDC);

	//Draw in appropriate color if highlighted

	COLORREF clrBackground;
    if (lpDrawItemStruct->itemState & ODS_SELECTED)
    {
        pdc->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
		clrBackground = GetSysColor(COLOR_HIGHLIGHT);        
    }
    else
    {
        pdc->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
		clrBackground = GetSysColor(COLOR_WINDOW);
    }
	pdc->SetBkColor(clrBackground);

	rcFocus.left = 0;
	rcFocus.right = rectListCtrl.right;

	// Account for scroll bar's location to draw properly
	rc.right = -1 - GetScrollPos(SB_HORZ);
	for (int i=0; i<NUMCOLS; i++)
	{
		rc.left = rc.right + 1;
		rc.right = rc.left + GetColumnWidth(i) - 1;

		rcFocus.right = rcFocus.left + GetColumnWidth(i) - 8;		

		//Clear out the background.  This prevents pixel damage		
		pdc->FillSolidRect(rcFocus, clrBackground);

		// Figure out how much space we have to write text
		CSize size;
		int iMaxTextWidth = rcFocus.right - rcFocus.left + 1;
		if (i == 0)
			iMaxTextWidth -= rc.Height() + IMAGE_MARGIN * 2;
		// Only draw this if we're in the zone
		if (rcFocus.right > rcFocus.left)
		{			
			str = GetItemText(lpDrawItemStruct->itemID, i);

			// Make text look presentable in space provided
			BOOL bAppended(FALSE);
			do
			{
				size = pdc->GetTextExtent(str, str.GetLength());
				if (size.cx > iMaxTextWidth)
				{
					// Only append "..." the first time
					if (!bAppended)
					{
						str = str + _T("...");
					}
					bAppended = TRUE;

					// Remove characters if we have a character
					// to remove that's not appended
					if (str.GetLength() > 3)
					{
						str = str.Left(str.GetLength()-4) + _T("...");
					}
					else // Drop out if there's no characters to eliminate
						break;
				}
			} while (size.cx > iMaxTextWidth);


			rcFocus.right = rectListCtrl.right;
			pdc->ExtTextOut(rcFocus.left, rcFocus.top, ETO_OPAQUE, 
							rcFocus, "", 0, NULL);		

			//Only draw bitmap on left side
			if (i == 0)
			{
				if (lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
				{				
					CImageList* pImageList = GetImageList(LVSIL_SMALL);

					rc.left += IMAGE_MARGIN;
					pImageList->Draw(pdc, GetItemStatus(lpDrawItemStruct->itemID), rc.TopLeft(), ILD_TRANSPARENT);

					rc.left += rc.Height() + IMAGE_MARGIN;
				}
			}
			
			pdc->DrawText(str, str.GetLength(), rc, 
						  DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		}

		if (lpDrawItemStruct->itemAction == ODA_SELECT ||
			lpDrawItemStruct->itemAction == ODA_FOCUS)
		{
			return;
		}
		rcFocus.left = rc.right + 1;
	}

	rcFocus.right = rectListCtrl.right;
	//  We don't highlight all the way to the right side anymore.
	//pdc->FillSolidRect(rcFocus, clrBackground);
	pdc->FillSolidRect(rcFocus, GetSysColor(COLOR_WINDOW));	
}

//This is the key to having the s draw all the way across.
void CServicesListCtrl::OnPaint() 
{
	// in full row select mode, we need to extend the clipping region
	// so we can paint a selection all the way to the right
	CRect rcAllLabels;
	CRect rcListCtrl;
	GetItemRect(0, rcAllLabels, LVIR_BOUNDS);
	GetWindowRect(&rcListCtrl);

	if(rcAllLabels.right < rcListCtrl.right)
	{
		// need to call BeginPaint (in CPaintDC c-tor)
		// to get correct clipping rect
		CPaintDC dc(this);

		CRect rcClip;
		dc.GetClipBox(rcClip);

		rcClip.left = min(rcAllLabels.right-1, rcClip.left);
		rcClip.right = rcListCtrl.right;
		
		InvalidateRect(rcClip, FALSE);
		// EndPaint will be called in CPaintDC d-tor
	}

	CListCtrl::OnPaint();
}
