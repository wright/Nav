#if !defined(AFX_REATEDPSTAR_H__83210F60_D1A4_11D3_82FC_00A0CC29DFDC__INCLUDED_)
#define AFX_REATEDPSTAR_H__83210F60_D1A4_11D3_82FC_00A0CC29DFDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CreateDpStar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CreateDpStar dialog

class CreateDpStar : public CDialog
{
// Construction
public:
	CreateDpStar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CreateDpStar)
	enum { IDD = IDD_CREATEDPSTAR };
	CString	m_prompt;
	CString	m_routename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CreateDpStar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CreateDpStar)
	afx_msg void OnChangeRoutename();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REATEDPSTAR_H__83210F60_D1A4_11D3_82FC_00A0CC29DFDC__INCLUDED_)
