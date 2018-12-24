#if !defined(UNHUMAN_SOFTWARE_REGISTRATION)
#define UNHUMAN_SOFTWARE_REGISTRATION

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CRegCode
{
public:
	CRegCode(unsigned long L0, unsigned long L1, unsigned long L2, 
			 unsigned long L3, unsigned long L4)
	{
		m_L0 = L0;
		m_L1 = L1;
		m_L2 = L2;
		m_L3 = L3;
		m_L4 = L4;
	}
	
	BOOL GenerateRegCode(CString strInput, CString& strOutput);
private:
	void SHA(char* mem, unsigned long length, unsigned long* buf);
	BOOL Base64Encode(BYTE* pSource, int iSourceLen, CString& destination);
	unsigned long m_L0;
	unsigned long m_L1;
	unsigned long m_L2;
	unsigned long m_L3;
	unsigned long m_L4;
};

#endif