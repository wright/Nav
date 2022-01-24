// Aircraft.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Aircraft dialog

class Aircraft : public CDialog
{
// Construction
public:
	Aircraft(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Aircraft)
	enum { IDD = IDD_CRAFT };
	CString	m_craft;
	int		m_cruisespeed;
	int		m_descentspeed;
	int		m_climbspeed;
	double	m_descentfuel;
	double	m_cruisefuel;
	double	m_climbfuel;
	int		m_climbrate;
	int		m_descentrate;
	long	m_cruisealtitude;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Aircraft)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Aircraft)
	afx_msg void OnLoad();
	afx_msg void OnSaveas();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
