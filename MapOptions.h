#if !defined(AFX_MAPOPTIONS_H__4AA76402_BB32_11D0_B509_00A024BF257A__INCLUDED_)
#define AFX_MAPOPTIONS_H__4AA76402_BB32_11D0_B509_00A024BF257A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MapOptions.h : header file
//
#include "FancyMapper.h"

/////////////////////////////////////////////////////////////////////////////
// MapOptions dialog

class MapOptions : public CDialog
{
// Construction
public:
	void update();
	void edit(int index);
	visibilityStruct *v;
	MapOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(MapOptions)
	enum { IDD = IDD_MAPOPTIONS };
	int		m_decimation;
	BOOL	m_showall;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MapOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(MapOptions)
	afx_msg void OnCil1();
	afx_msg void OnCil2();
	afx_msg void OnCil3();
	afx_msg void OnCil4();
	afx_msg void OnCil6();
	afx_msg void OnCil7();
	afx_msg void OnCil8();
	afx_msg void OnCil9();
	afx_msg void OnCil10();
	afx_msg void OnCil13();
	afx_msg void OnCil14();
	afx_msg void OnCil15();
	afx_msg void OnIpb1();
	afx_msg void OnIpb2();
	afx_msg void OnIpb3();
	afx_msg void OnNpb1();
	afx_msg void OnNpb2();
	afx_msg void OnNpb3();
	afx_msg void OnRiv1();
	afx_msg void OnRiv2();
	afx_msg void OnRiv3();
	afx_msg void OnRiv4();
	afx_msg void OnRiv5();
	afx_msg void OnRiv6();
	afx_msg void OnRiv7();
	afx_msg void OnRiv8();
	afx_msg void OnRiv10();
	afx_msg void OnRiv11();
	afx_msg void OnRiv12();
	afx_msg void OnClose();
	afx_msg void OnShowall();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPOPTIONS_H__4AA76402_BB32_11D0_B509_00A024BF257A__INCLUDED_)
