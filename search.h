// search.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Search dialog

class Search : public CDialog
{
// Construction
public:
	Search(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Search)
	enum { IDD = IDD_SEARCH };
	CString	m_target;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Search)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Search)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
