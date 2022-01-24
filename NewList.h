// NewList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// NewList dialog

class NewList : public CDialog
{
// Construction
public:
	NewList(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(NewList)
	enum { IDD = IDD_NEWLIST };
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(NewList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(NewList)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
