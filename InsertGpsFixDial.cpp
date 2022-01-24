// InsertGpsFixDial.cpp : implementation file
//
// Copyright (C) Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>
/*
    Copyright (C) 2000 Ted Wright 
    NASA Business: wright@grc.nasa.gov, Remainder: wright@en.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "stdafx.h"
#include "Nav.h"
#include "InsertGpsFixDial.h"
#include <afxtempl.h>
#include "mainfrm.h"
#include "Dpstar.h"
#include "navdoc.h"
#include "flightpl.h"
#include "navview.h"
#include "utilcalcul.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InsertGpsFixDial dialog


InsertGpsFixDial::InsertGpsFixDial(CWnd* pParent /*=NULL*/)
	: CDialog(InsertGpsFixDial::IDD, pParent)
{
	//{{AFX_DATA_INIT(InsertGpsFixDial)
	m_gpsfix_distance = 0.0;
	m_navaidDetails = _T("");
	m_gpsfix_newLat = _T("");
	m_gpsfix_newLong = _T("");
	m_gpsfix_heading = 0.0;
	m_HeadingReverse = _T("");
	m_id = _T("");
	m_name = _T("");
	//}}AFX_DATA_INIT
}


void InsertGpsFixDial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InsertGpsFixDial)
	DDX_Text(pDX, IDC_INSERT_GPSFIX_DISTANCE, m_gpsfix_distance);
	DDX_Text(pDX, IDC_TEXT, m_navaidDetails);
	DDX_Text(pDX, IDC_GPSFIX_NEWLAT, m_gpsfix_newLat);
	DDX_Text(pDX, IDC_GPSFIX_NEWLONG, m_gpsfix_newLong);
	DDX_Text(pDX, IDC_INSERT_GPSFIX_HDG, m_gpsfix_heading);
	DDX_Text(pDX, IDC_HEADING_REVERSE, m_HeadingReverse);
	DDX_Text(pDX, IDC_ID, m_id);
	DDV_MaxChars(pDX, m_id, 5);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDV_MaxChars(pDX, m_name, 26);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(InsertGpsFixDial, CDialog)
	//{{AFX_MSG_MAP(InsertGpsFixDial)
	ON_BN_CLICKED(IDC_GPSFIX_INSERT, OnGpsfixInsert)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DISTANCE, OnDeltaposSpinDistance)
	ON_EN_CHANGE(IDC_INSERT_GPSFIX_DISTANCE, OnChangeInsertGpsfixDistance)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HEADING, OnDeltaposSpinHeading)
	ON_EN_CHANGE(IDC_INSERT_GPSFIX_HDG, OnChangeInsertGpsfixHdg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InsertGpsFixDial message handlers

// "send to plan"
void InsertGpsFixDial::OnGpsfixInsert() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	UpdateData(TRUE);
	
	double fixLat = fromLat;
	double fixLong = fromLong;
	DistHeadToCoord(m_gpsfix_distance,m_gpsfix_heading,fixLat,fixLong);
	
	int newIndex = doc->OnMapInsertgpsfix(fixLat, fixLong, m_name, m_id);
	view->SendToPlan(newIndex);
	view->Invalidate(TRUE);
	CDialog::OnCancel();
}

void InsertGpsFixDial::OnOK() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	UpdateData(TRUE);

	double fixLat = fromLat;
	double fixLong = fromLong;
	DistHeadToCoord(m_gpsfix_distance,m_gpsfix_heading,fixLat,fixLong);

	doc->OnMapInsertgpsfix(fixLat, fixLong, m_name, m_id);
	view->Invalidate(TRUE);
	CDialog::OnOK();
}

BOOL InsertGpsFixDial::OnInitDialog() {
	CDialog::OnInitDialog();
	
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	// let's round the heading and distance
	m_gpsfix_heading = (double)((int)(m_gpsfix_heading + 0.5));
	m_gpsfix_distance = (double)((int)(m_gpsfix_distance*10))/10;
	double fixLat = fromLat;
	double fixLong = fromLong;
	DistHeadToCoord(m_gpsfix_distance,m_gpsfix_heading,fixLat,fixLong);
	LatToString(fixLat, m_gpsfix_newLat);
	LongToString(fixLong, m_gpsfix_newLong);

	double	reverse_Heading = m_gpsfix_heading + 180;

	if (reverse_Heading < 0)
		reverse_Heading += 360;
	if (reverse_Heading > 360)
		reverse_Heading -= 360;

	m_HeadingReverse.Format("%3.0f",reverse_Heading);

	m_id = doc->GenUniqueNumber();
	char dirx, diry;// generate DMS name
	if (fixLong < 0) 
		dirx = 'W'; 
	else 
		dirx = 'E';
	if (fixLat < 0) 
		diry = 'S'; 
	else 
		diry = 'N';
	int xdeg, xmin, ydeg, ymin;
	double xsec, ysec, gpslat, gpslon;
	gpslat = fabs(fixLat);
	gpslon = fabs(fixLong);
	xdeg = (int) gpslon;
	xmin = (int) ((gpslon - xdeg) * 60);
	xsec = (gpslon - xdeg - xmin / 60.0) * 3600;
	if (xsec >= 59.995) { // round to .01 seconds
		xsec = 0.0;
		xmin += 1;
	}
	if (xmin >= 60) {
		xmin -= 60;
		xdeg += 1;
	}

	ydeg = (int) gpslat;
	ymin = (int) ((gpslat - ydeg) * 60);
	ysec = (gpslat - ydeg - ymin / 60.0) * 3600;
	if (ysec >= 59.995) { // round to .01 seconds
		ysec = 0.0;
		ymin += 1;
	}
	if (ymin >= 60) {
		ymin -= 60;
		ydeg += 1;
	}

	char buf[27];
	sprintf(buf, "%d %d'%2.0f\"%c, %d %d'%2.0f\"%c", ydeg, ymin, ysec, diry, xdeg, xmin, xsec, dirx);
	m_name = buf;

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void InsertGpsFixDial::OnChangeInsertGpsfixDistance() {
	UpdateData(TRUE);
	RecalculateLatLong();
	UpdateData(FALSE);
}

void InsertGpsFixDial::OnChangeInsertGpsfixHdg() {
	UpdateData(TRUE);
	RecalculateLatLong();
	UpdateData(FALSE);
}

void InsertGpsFixDial::RecalculateLatLong() {
	double fixLat = fromLat;
	double fixLong = fromLong;
	DistHeadToCoord(m_gpsfix_distance,m_gpsfix_heading,fixLat,fixLong);
	LatToString(fixLat, m_gpsfix_newLat);
	LongToString(fixLong, m_gpsfix_newLong);

	char dirx, diry;// generate DMS name
	if (fixLong < 0) 
		dirx = 'W'; 
	else 
		dirx = 'E';
	if (fixLat < 0) 
		diry = 'S'; 
	else 
		diry = 'N';
	int xdeg, xmin, ydeg, ymin;
	double xsec, ysec, gpslat, gpslon;
	gpslat = fabs(fixLat);
	gpslon = fabs(fixLong);
	xdeg = (int) gpslon;
	xmin = (int) ((gpslon - xdeg) * 60);
	xsec = (gpslon - xdeg - xmin / 60.0) * 3600;
	ydeg = (int) gpslat;
	ymin = (int) ((gpslat - ydeg) * 60);
	ysec = (gpslat - ydeg - ymin / 60.0) * 3600;
	char buf[27];
	sprintf(buf, "%d %d'%2.0f\"%c, %d %d'%2.0f\"%c", ydeg, ymin, ysec, diry, xdeg, xmin, xsec, dirx);
	m_name = buf;
}

void InsertGpsFixDial::OnDeltaposSpinHeading(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	m_gpsfix_heading-= pNMUpDown->iDelta;

	if (m_gpsfix_heading < 0)
		m_gpsfix_heading += 360;
	if (m_gpsfix_heading > 360)
		m_gpsfix_heading -= 360;

	double	reverse_Heading = m_gpsfix_heading + 180;

	if (reverse_Heading < 0)
		reverse_Heading += 360;
	if (reverse_Heading > 360)
		reverse_Heading -= 360;

	m_HeadingReverse.Format("%.0f",reverse_Heading);
	
	RecalculateLatLong();
	UpdateData(FALSE);

	*pResult = 0;
}

void InsertGpsFixDial::OnDeltaposSpinDistance(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_gpsfix_distance-= pNMUpDown->iDelta;

	if (m_gpsfix_distance < 0)
		m_gpsfix_distance = 0;
	
	RecalculateLatLong();
	UpdateData(FALSE);

	*pResult = 0;
}

