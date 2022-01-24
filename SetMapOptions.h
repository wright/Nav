#if !defined(AFX_SETMAPOPTIONS_H__4AA76403_BB32_11D0_B509_00A024BF257A__INCLUDED_)
#define AFX_SETMAPOPTIONS_H__4AA76403_BB32_11D0_B509_00A024BF257A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SetMapOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SetMapOptions dialog

class SetMapOptions : public CDialog
{
// Construction
public:
	SetMapOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SetMapOptions)
	enum { IDD = IDD_SETMAPOPTIONS };
	BYTE	m_blue;
	BYTE	m_green;
	BYTE	m_red;
	int		m_mag;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SetMapOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SetMapOptions)
	afx_msg void OnClose();
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETMAPOPTIONS_H__4AA76403_BB32_11D0_B509_00A024BF257A__INCLUDED_)
