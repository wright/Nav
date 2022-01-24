// ExportProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ExportProgress dialog

class ExportProgress : public CDialog
{
// Construction
public:
	ExportProgress(CWnd* pParent = NULL);   // standard constructor
	int cancelled;

// Dialog Data
	//{{AFX_DATA(ExportProgress)
	enum { IDD = IDD_EXPORTPROGRESS };
	CProgressCtrl	m_runways;
	CProgressCtrl	m_navaids;
	CProgressCtrl	m_airports;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ExportProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ExportProgress)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
