// CenterAt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCenterAt dialog

class CCenterAt : public CDialog
{
// Construction
public:
	CCenterAt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCenterAt)
	enum { IDD = IDD_CENTERAT };
	double	m_lat;
	double	m_lon;
	int		m_latd;
	int		m_latm;
	double	m_lats;
	int		m_lond;
	int		m_lonm;
	double	m_lons;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCenterAt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:

	// Generated message map functions
	//{{AFX_MSG(CCenterAt)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDoubleclickedRadiodd();
	afx_msg void OnDoubleclickedRadiodmsd();
	afx_msg void OnUpdateDdeglat();
	afx_msg void OnUpdateDdeglon();
	afx_msg void OnUpdateLatd();
	afx_msg void OnUpdateLatm();
	afx_msg void OnUpdateLond();
	afx_msg void OnUpdateLonm();
	afx_msg void OnUpdateLons();
	afx_msg void OnUpdateLats();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
