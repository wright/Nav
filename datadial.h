// datadial.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DataDialog dialog

class DataDialog : public CDialog
{
// Construction
public:
	DataDialog(CWnd* pParent = NULL);   // standard constructor
	void add(dbRec dbr, int index);
	CListBox *lb;

// Dialog Data
	//{{AFX_DATA(DataDialog)
	enum { IDD = IDD_DATABASE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DataDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int NDBcount;
	int ILScount;
	int VORcount;
	int APTcount;
	int MKRcount;
	int RWYcount;
	int GPScount;
	int ATIScount;
	int DYNcount;
	int WAYcount;
	int RTEcount;

	// Generated message map functions
	//{{AFX_MSG(DataDialog)
	afx_msg void OnCenter();
	afx_msg void OnDetails();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList();
	afx_msg void OnPlan();
	afx_msg void OnSummary();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
