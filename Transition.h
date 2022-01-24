// Transition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Transition dialog

class Transition : public CDialog
{
// Construction
public:
	Transition(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Transition)
	enum { IDD = IDD_SETTRANSITION };
	int		m_trans;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Transition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Transition)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
