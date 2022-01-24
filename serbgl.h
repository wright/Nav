// serbgl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SerBgl dialog

class SerBgl : public CDialog
{
// Construction
public:
	SerBgl(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SerBgl)
	enum { IDD = IDD_SEARCHBGL };
	BOOL	m_recurse;
	CString	m_drive;
	BOOL	m_skip;
	BOOL	m_useAFD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SerBgl)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SerBgl)
	afx_msg void OnChoose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
