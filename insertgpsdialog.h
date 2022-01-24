#if !defined(AFX_INSERTGPSDIALOG_H__7215C880_D2FF_11D3_B498_005056816121__INCLUDED_)
#define AFX_INSERTGPSDIALOG_H__7215C880_D2FF_11D3_B498_005056816121__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InsertGPSDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// InsertGPSDialog dialog

class InsertGPSDialog : public CDialog
{
// Construction
public:
	double lat, lon;
	InsertGPSDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(InsertGPSDialog)
	enum { IDD = IDD_INSERTGPSFIX };
	CString	m_id;
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(InsertGPSDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(InsertGPSDialog)
	afx_msg void OnSendtoplan();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTGPSDIALOG_H__7215C880_D2FF_11D3_B498_005056816121__INCLUDED_)
