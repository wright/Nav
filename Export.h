// Export.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CExport dialog

class CExport : public CDialog
{
// Construction
public:
	CExport(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CExport)
	enum { IDD = IDD_EXPORT };
	CString	m_center;
	CString	m_state;
	BOOL	m_headings;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExport)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
