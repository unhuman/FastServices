// DlgRegister.cpp : implementation file
//

#include "stdafx.h"
#include "Process.h"
#include "Fast Services.h"
#include "DlgRegister.h"
#include "RegCode.h"
#include "Registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRegister dialog


CDlgRegister::CDlgRegister(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRegister::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRegister)
	m_strUserName = _T("");
	m_strRegistration = _T("");
	m_strExpiration = _T("Fast Services trial period has expired.");
	//}}AFX_DATA_INIT

	m_iRegistrationStatus = EXPIRED;
	m_iTrialDaysLeft = -1;

	GetSetRegistry(TRUE);
}


void CDlgRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRegister)
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_REGISTRATION_CODE, m_strRegistration);
	DDX_Text(pDX, IDC_STATIC_EXPIRATION, m_strExpiration);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRegister, CDialog)
	//{{AFX_MSG_MAP(CDlgRegister)
	ON_BN_CLICKED(IDC_BUTTON_EMAIL, OnButtonEmail)
	ON_BN_CLICKED(IDC_BUTTON_WEB, OnButtonWeb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRegister message handlers

void CDlgRegister::OnButtonEmail() 
{
	_spawnlp(_P_NOWAIT, "cmd.exe", "/C start mailto:human@unhuman.com", NULL);
}

void CDlgRegister::OnButtonWeb() 
{
	_spawnlp(_P_NOWAIT, "cmd.exe", "/C start http://www.unhuman.com", NULL);
}

void CDlgRegister::OnOK() 
{
	CString strRegistration(_T(""));
	UpdateData();
	if (RegCode(m_strUserName, m_strRegistration))
	{
		m_iRegistrationStatus = LICENSED;
		GetSetRegistry(FALSE);
		CDialog::OnOK();
	}
	else //this copy is *NOT* registered
	{
		//Invalid stuff in registry - blank it out
		m_strUserName = _T("");
		m_strRegistration = _T("");
		UpdateData(FALSE);
		AfxMessageBox("Invalid user registration.", MB_ICONINFORMATION);
	}	
}

BOOL CDlgRegister::RegCode(CString strUser, CString& strPassword)
{
	// Generate the registration code
	unsigned long L0 = 0xAC931DA4L;
	unsigned long L1 = 0x124AC431L;
	unsigned long L2 = 0x765ABCDEL;
	unsigned long L3 = 0xFABCDE21L;
	unsigned long L4 = 0x8942CA4DL;
	CString strTempRegCode(_T(""));
	CRegCode codeRegistration(L0, L1, L2, L3, L4);
	if (!codeRegistration.GenerateRegCode(strUser, strTempRegCode))
		return FALSE;
	
	// Validate the password
	if (strTempRegCode != strPassword)
		return FALSE;
	
	return TRUE;
}

void CDlgRegister::GetSetRegistry(BOOL bGetFromRegistry /*TRUE*/)
{
	COXRegistry regRegistration;
	BOOL bSuccess(FALSE);
	CString strUser(_T(""));
	CString strCode(_T(""));	
	CString strWinDir(_T(""));
	CString strFSINI(_T(""));
	int iFound = 0;

	// Get the windows directory location
	GetWindowsDirectory(strWinDir.GetBuffer(MAX_PATH), MAX_PATH);
	strWinDir.ReleaseBuffer();
	if (!strWinDir.GetLength())
	{
		AfxMessageBox(IDS_FAILURE_GETWINDIR);
		exit(99);
	}
	if (strWinDir.GetAt(strWinDir.GetLength()-1) != _T('\\'))
		strWinDir += _T('\\');

	// Get the directory where the application
	// is running
	CWinApp* pApplication = AfxGetApp( ); 
	strFSINI = pApplication->m_pszHelpFilePath;	
	if ((iFound = strFSINI.ReverseFind(_T('\\'))) >= 2)
		strFSINI = strFSINI.Left(iFound+1);
	else
		strFSINI = strWinDir;

	strFSINI += _T("FastServices.INI");

	if (regRegistration.Connect(COXRegistry::keyLocalMachine))
	{
		CString strRegKey(_T(""));
		CString strWinTime(_T(""));
		CString strDDTime(_T(""));
		CString strRegTime(_T(""));
		strRegKey.LoadString(IDS_REGISTRY_KEY);
		if (regRegistration.CheckAndOpenKeyForWrite(strRegKey))
		{	if (bGetFromRegistry)
			{
				bSuccess = regRegistration.GetValue(_T("UserName"), m_strUserName);
				if (bSuccess)
					bSuccess = regRegistration.GetValue(_T("RegistrationCode"), m_strRegistration);

				//If there's valid info in the registry				
				if (bSuccess && RegCode(m_strUserName, m_strRegistration))
					m_iRegistrationStatus = LICENSED;
				else //this copy is *NOT* registered - date check time
				{
					//Invalid stuff in registry - blank it out
					m_strUserName = _T("");
					m_strRegistration = _T("");

					// Get the 3 times (registry, Win.ini, and FastServices.ini)
					time_t tmtNow(0);
					time(&tmtNow);
					DWORD dwTime = tmtNow;
					GetPrivateProfileString(_T("FastServices"),
											_T("Configuration"), _T(""),
											strWinTime.GetBuffer(MAX_PATH), MAX_PATH,
											strWinDir + _T("WIN.INI"));
					strWinTime.ReleaseBuffer();
					GetPrivateProfileString(_T("FastServices"),
											_T("InstallTime"), _T(""),
											strDDTime.GetBuffer(MAX_PATH), MAX_PATH,
											strFSINI);
					strDDTime.ReleaseBuffer();

					// This is the first time this thing has been run, set
					// up registry, WIN.INI, and DownloadDemon.INI
					if (!regRegistration.GetValue(_T("FirstRunTime"), dwTime) &&
						(strWinTime == _T("")) && (strDDTime == _T("")))
					{					
						regRegistration.SetValue(_T("FirstRunTime"), tmtNow);
											
						strWinTime.Format("%x", tmtNow);
						strDDTime = strWinTime;
						WritePrivateProfileString(_T("FastServices"),
												  _T("Configuration"),
												  strWinTime,
												  strWinDir + _T("WIN.INI"));
						WritePrivateProfileString(_T("FastServices"),
												  _T("InstallTime"),
												  strDDTime, strFSINI);
					}

					// Okay, now all the numbers have to match up.
					// If they don't, require the user to register.
					strRegTime.Format("%x", dwTime);
					
					if ((strWinTime != strDDTime) || (strWinTime != strRegTime))
					{						
						m_iRegistrationStatus = EXPIRED;
					}
					else
					{
						// Check the time - if it's over 30 days,
						// expire, otherwise, we're still in a trial
						m_iRegistrationStatus = EXPIRED;
						m_strExpiration = _T("Fast Services trial period has expired.");

						// Ensure the user didn't set the clock back 8^)
						CTime tmInstall(dwTime);
						CTime tmNow(tmtNow);
						if (tmNow >= tmInstall)
						{
							CTimeSpan tmDiff(tmNow - tmInstall);
							m_iTrialDaysLeft = 30 - tmDiff.GetDays();
							if (m_iTrialDaysLeft >= 0)
							{																
								m_iRegistrationStatus = TRIAL;
								m_strExpiration.Format("Fast Services expires in %d days.", m_iTrialDaysLeft);
							}

						}
					}
					// If the license expired, write garbage back out
					// so the program won't be able to be used again
					if (m_iRegistrationStatus == EXPIRED)
					{
						// Populate INI files with random values
						// so registration never works, even if clock
						// is set back
						srand(tmtNow);
						strRegTime.Format("%x", rand() + 4389243);
						WritePrivateProfileString(_T("FastServices"),
												  _T("Configuration"),
												  strRegTime,
												  strWinDir + _T("WIN.INI"));
						strRegTime.Format("%x", rand() + 13489388);
						WritePrivateProfileString(_T("FastServices"),
												  _T("InstallTime"),
												  strRegTime, strFSINI);
					}
				}
			}
			else
			{
				UpdateData();
				regRegistration.SetValue("UserName", m_strUserName);
				regRegistration.SetValue("RegistrationCode", m_strRegistration);
			}
		}
		regRegistration.Disconnect();
	}
}

int CDlgRegister::DoModal() 
{
	GetSetRegistry(TRUE);
	return CDialog::DoModal();
}
