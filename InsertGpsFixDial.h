#if !defined(AFX_INSERTGPSFIXDIAL_H__DE60B683_CB19_11D1_B267_00805F219C77__INCLUDED_)
#define AFX_INSERTGPSFIXDIAL_H__DE60B683_CB19_11D1_B267_00805F219C77__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InsertGpsFixDial.h : header file
//

// code by Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>

/////////////////////////////////////////////////////////////////////////////
// InsertGpsFixDial dialog

class InsertGpsFixDial : public CDialog
{

	void RecalculateLatLong();

// Construction
public:
	double fromLat;
	double fromLong;

	InsertGpsFixDial(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(InsertGpsFixDial)
	enum { IDD = IDD_INSERT_A_GPS_FIX };
	double	m_gpsfix_distance;
	CString	m_navaidDetails;
	CString	m_gpsfix_newLat;
	CString	m_gpsfix_newLong;
	double	m_gpsfix_heading;
	CString	m_HeadingReverse;
	CString	m_id;
	CString	m_name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(InsertGpsFixDial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(InsertGpsFixDial)
	afx_msg void OnGpsfixInsert();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinDistance(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInsertGpsfixDistance();
	afx_msg void OnDeltaposSpinHeading(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeInsertGpsfixHdg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTGPSFIXDIAL_H__DE60B683_CB19_11D1_B267_00805F219C77__INCLUDED_)
