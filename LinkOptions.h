// LinkOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LinkOptions dialog

class LinkOptions : public CDialog
{
// Construction
public:
	LinkOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(LinkOptions)
	enum { IDD = IDD_LINKOPTIONS };
	BOOL	m_flysetmode;
	BOOL	m_enablenetwork;
	double	m_flyupdate;
	double	m_update;
	CString	m_callsign;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LinkOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LinkOptions)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
