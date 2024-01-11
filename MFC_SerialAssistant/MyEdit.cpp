// MyEdit.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MFC_SerialAssistantDlg.h"
#include "MyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEdit

CMyEdit::CMyEdit()
{
	m_bHex = FALSE;
	m_nCount = 0;
	m_bChange = TRUE;
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//{{AFX_MSG_MAP(CMyEdit)
	ON_WM_ERASEBKGND()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

void CMyEdit::CleanData()
{
	SetSel(0, -1);
	ReplaceSel("");
	m_nCount = 0;
	m_bChange = TRUE;
}

void CMyEdit::SetHex(BOOL bHex)
{
	m_bHex = bHex;
	m_bChange = TRUE;
}

int CMyEdit::GetData(char** ppch)
{
	int len, i;
	CString str;

	GetWindowText(str);
	*ppch = NULL;
	m_bChange = FALSE;

	if (m_bHex)
	{
		len = str.GetLength();
		str.MakeLower();

		if (len > 0)
		{
			int n = 0;
			char* p = new char[len];
			for (i = 0; i < len; i++)
			{
				char c = str[i];
				if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
				{
					p[n] = c;
					n++;
				}
				else if (c != ' ')
					break;
			}

			len = n / 2;
			if (n > 1)
			{
				*ppch = new char[len];
				for (i = 0; i < len; i++)
				{
					char c1 = p[i * 2];
					char c2 = p[i * 2 + 1];

					if (c1 >= '0' && c1 <= '9')
						c1 = c1 - '0';
					else
						c1 = c1 - 'a' + 10;

					if (c2 >= '0' && c2 <= '9')
						c2 = c2 - '0';
					else
						c2 = c2 - 'a' + 10;
					(*ppch)[i] = (c1 << 4) | c2;
				}
			}

			delete[]p;
		}
	}
	else
	{
		len = str.GetLength();
		if (len > 0)
		{
			*ppch = new char[len];
			memcpy(*ppch, (const char*)str, len);
		}
	}

	return len;
}

void CMyEdit::AppendData(char* pch, int len)
{
	if (m_bHex)
	{
		char* p = new char[len * 3 + 1];
		char* p1 = p;
		for (int i = 0; i < len; i++)
		{
			unsigned char c = pch[i];
			unsigned char c1 = c & 0x0f;
			unsigned char c2 = c >> 4;

			if (c2 > 9)
				p1[0] = c2 - 10 + 'A';
			else
				p1[0] = c2 + '0';

			if (c1 > 9)
				p1[1] = c1 - 10 + 'A';
			else
				p1[1] = c1 + '0';

			p1[2] = ' ';
			p1 += 3;
		}

		p[len * 3] = 0;
		SetSel(0x70000000, -1);
		ReplaceSel(p);
		delete[]p;
	}
	else
	{
		char* p = new char[len + 1];
		for (int i = 0; i < len; i++)
		{
			char c = pch[i];
			//if(!isprint(c) && c != 0x0D && c != 0x0A && c != '\t')
			//	p[i] = '.';
			//else
			p[i] = c;
		}
		p[len] = 0;
		SetSel(0x70000000, -1);
		ReplaceSel(p);
		delete[]p;
	}

	m_nCount += len;
	if (m_nCount > 4 * 1024)
		CleanData();
	m_bChange = TRUE;
}

void CMyEdit::SetMaxLen(int len)
{

}

BOOL CMyEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CEdit::OnEraseBkgnd(pDC);
}

void CMyEdit::OnChange()
{
	m_bChange = TRUE;
}
