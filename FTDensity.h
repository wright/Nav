// FTDensity.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFTDensity dialog

class CFTDensity : public CDialog
{
// Construction
public:
	CFTDensity(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFTDensity)
	enum { IDD = IDD_DENSITY };
	int		m_density;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFTDensity)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFTDensity)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
