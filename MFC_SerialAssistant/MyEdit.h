#if !defined(AFX_MYEDIT_H__5D73D753_BD9B_4F1C_BCC0_B48C7C8AC876__INCLUDED_)
#define AFX_MYEDIT_H__5D73D753_BD9B_4F1C_BCC0_B48C7C8AC876__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyEdit window

class CMyEdit : public CEdit
{
	// Construction
public:
	CMyEdit();

	// Attributes
public:
	BOOL m_bChange;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyEdit)
	//}}AFX_VIRTUAL

	// Implementation
public:
	void SetMaxLen(int len);
	void AppendData(char* pch, int len);
	int GetData(char** ppch);
	void SetHex(BOOL bHex);
	BOOL GetHex() { return m_bHex; }
	void CleanData();
	virtual ~CMyEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyEdit)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	BOOL m_bHex;
	int m_nCount;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYEDIT_H__5D73D753_BD9B_4F1C_BCC0_B48C7C8AC876__INCLUDED_)
