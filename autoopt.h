// autoopt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AutoOpt dialog

class AutoOpt : public CDialog
{
// Construction
public:
	AutoOpt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AutoOpt)
	enum { IDD = IDD_AUTO };
	BOOL	m_airports;
	BOOL	m_continue;
	double	m_ideal;
	BOOL	m_ndb;
	BOOL	m_vor;
	BOOL	m_waypoints;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AutoOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AutoOpt)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
