// details.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Details dialog

class Details : public CDialog
{
// Construction
public:
	Details(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Details)
	enum { IDD = IDD_DETAILS };
	CString	m_text;
	//}}AFX_DATA
	int index;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Details)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Details)
	afx_msg void OnPlan();
	afx_msg void OnCenter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
