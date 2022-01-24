#if !defined(AFX_SELECTDPSTAR_H__101F32A4_D1A7_11D3_82FC_00A0CC29DFDC__INCLUDED_)
#define AFX_SELECTDPSTAR_H__101F32A4_D1A7_11D3_82FC_00A0CC29DFDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectDpStar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectDpStar dialog

class SelectDpStar : public CDialog
{
// Construction
public:
	FlightPlan *fp;
	int airportIndex;
	double cdist, clat, clon;
	int isStar;
	CString icao, dpStarName, result;
	CString buildName(DpStarAirport* airport, CString route);
	CArray<CString, CString> names;
	SelectDpStar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SelectDpStar)
	enum { IDD = IDD_SELECTDPSTAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SelectDpStar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SelectDpStar)
	afx_msg void OnDblclkList();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTDPSTAR_H__101F32A4_D1A7_11D3_82FC_00A0CC29DFDC__INCLUDED_)
