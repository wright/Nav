// Wind.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Wind dialog

class Wind : public CDialog
{
// Construction
public:
	int noselection;
	Wind(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Wind)
	enum { IDD = IDD_WIND };
	int		m_direction;
	int		m_speed;
	BOOL	m_global;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Wind)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Wind)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
