// LoadAircraft.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// LoadAircraft dialog

class LoadAircraft : public CDialog
{
// Construction
public:
	LoadAircraft(CWnd* pParent = NULL);   // standard constructor
	CString selected;

// Dialog Data
	//{{AFX_DATA(LoadAircraft)
	enum { IDD = IDD_LOADAIRCRAFT };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(LoadAircraft)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(LoadAircraft)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkCraft();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
