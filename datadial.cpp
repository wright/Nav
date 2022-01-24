// datadial.cpp : implementation file
//
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
#include <afxtempl.h>
#include "mainfrm.h"
#include "Dpstar.h"
#include "navdoc.h"
#include "flightpl.h"
#include "navview.h"
#include "datadial.h"
#include "math.h"
#include "details.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DataDialog dialog


DataDialog::DataDialog(CWnd* pParent /*=NULL*/)
	: CDialog(DataDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(DataDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	NDBcount = 0;
	ILScount = 0;
	VORcount = 0;
	APTcount = 0;
	MKRcount = 0;
	RWYcount = 0;
	GPScount = 0;
	ATIScount = 0;
	DYNcount = 0;
	WAYcount = 0;
	RTEcount = 0;
}


void DataDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DataDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DataDialog, CDialog)
	//{{AFX_MSG_MAP(DataDialog)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	ON_BN_CLICKED(IDC_DETAILS, OnDetails)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_PLAN, OnPlan)
	ON_BN_CLICKED(IDC_SUMMARY, OnSummary)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DataDialog message handlers
void DataDialog::OnCenter() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	lb = (CListBox *) GetDlgItem(IDC_LIST);
	int lbindex = lb->GetCurSel();
	if (lbindex == LB_ERR)
		return;
	int dbindex = lb->GetItemData(lbindex);

	double lat = doc->db[dbindex].lat;
	double lon = doc->db[dbindex].lon;
	if (doc->db[dbindex].type == 14) { // center of route
		lat += doc->db[dbindex].approach;
		lon += doc->db[dbindex].range;
		lat /= 2.0;
		lon /= 2.0;
	} 

	view->CenterAt(lat, lon);
}

void DataDialog::OnDblclkList() {
	OnDetails();	
}

void DataDialog::OnDetails() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	lb = (CListBox *) GetDlgItem(IDC_LIST);
	int lbindex = lb->GetCurSel();
	if (lbindex == LB_ERR)
		return;
	int dbindex = lb->GetItemData(lbindex);

	CString details = "";
	doc->formatData(details, dbindex);
	Details d;
	d.index = dbindex;
	d.m_text = details;
	d.DoModal();
}

BOOL DataDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	lb = (CListBox *) GetDlgItem(IDC_LIST);
	int count = 0;

	lb->ResetContent();
	lb->SetTabStops();
	lb->InitStorage(32767, 100);

	if (view->search == "") {	// we are not searching
		CWaitCursor wait;	// display wait cursor
		main->SetStatusBar("Collecting data...");
		for (int i=0; i<doc->dbCount; i++) {
			if (view->planOnly) {
				int proceed = FALSE;
				for (int q=0; q<view->plancount; q++)
					if (i == view->plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					goto next;
			}
			// limit to window being viewed
	 		if ((doc->db[i].lon) < view->left) goto next;
			if ((doc->db[i].lon) > view->right) goto next;
			if ((doc->db[i].lat) < view->bottom) goto next;
			if ((doc->db[i].lat) > view->top) goto next;
			if ((doc->db[i].type == 1) && (!view->ndb)) goto next;
			if ((doc->db[i].type == 2) && (!view->ils)) goto next;
			if ((doc->db[i].type == 3) && (!view->vor)) goto next;
			if ((doc->db[i].type == 4) && (!view->airports)) goto next;
			if ((doc->db[i].type == 5) && (!view->markers)) goto next;
			if ((doc->db[i].type == 6) && (!view->markers)) goto next;
			if ((doc->db[i].type == 7) && (!view->markers)) goto next;
			if ((doc->db[i].type == 8) && (!view->airports)) goto next;
			if ((doc->db[i].type ==10) && (!view->airports)) goto next;
			if ((doc->db[i].type ==11) && (!view->airports)) goto next;
			if ((doc->db[i].type ==13) && (!view->waypoints)) goto next;
			if ((doc->db[i].type ==14) && (!view->routes)) goto next;
			add(doc->db[i], i);
			count++;
			if(count > 32765) {
				char buf[300];
				sprintf(buf, "The Windows 'ListBox' has run out of room\nOnly %d of %d items will be shown\nTry zooming in first", i+1, doc->dbCount);
				AfxMessageBox(buf);
				break;
			}
			next:;
		}
	} else { 					// we are searching
		main->SetStatusBar("Searching...");
		char s[100], t[6], u[100];
		CString airportName;
		strcpy(s, view->search);
		_strlwr(s);
		for (int i=0; i<doc->dbCount; i++) {
			if (doc->db[i].type == 9) {
				strncpy(t, "gps  ", 5);
			} else {
				if (doc->db[i].type == 4) { // airport?
					if (doc->db[i].id[0] != 0x00) {
						strncpy(t, doc->db[i].id, 5);
					} else if (doc->lookUpAirport(airportName, doc->db[i].lat, doc->db[i].lon, i)) {
						// very slow way of finding ICAO codes
						strncpy(t, airportName.GetBuffer(1), 5);
						airportName.ReleaseBuffer();
					} else
						t[0] = 0x00;
				}  else
					strncpy(t, doc->db[i].id, 5);
			}
			t[5] = 0x00;
			_strlwr(t);
			strncpy(u, doc->db[i].name, 25);
			_strlwr(u);
			if ((NULL != strstr(t, s)) || (NULL != strstr(u, s)))
				add(doc->db[i], i);
		}
	}
	main->SetStatusBar("");
		
	return TRUE;  // return TRUE unless you set the focus to a control
}

void DataDialog::add(dbRec dbr, int index) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	CString airportName, cline;
	char line[100];
	if (1 == dbr.type) {
		sprintf(line, "NDB  %6s\t(%5.1f)\t%s", dbr.id, dbr.freq, dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		NDBcount++;
	} else if (2 == dbr.type) {
		sprintf(line, "ILS  %8s\t(%6.2f)\t%s", dbr.id, dbr.freq, dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		ILScount++;
	} else if (3 == dbr.type) {
		sprintf(line, "VOR  %6s\t(%6.2f)\t%s", dbr.id, dbr.freq, dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		VORcount++;
	} else if (4 == dbr.type) {
		cline = CString("Airport  ") + dbr.name;
		cline.TrimRight();
		if (dbr.id[0] != 0x00)
			cline += CString(" (") + dbr.id + CString(")");
		else if (doc->lookUpAirport(airportName, dbr.lat, dbr.lon))
			cline += CString(" (") + airportName + CString(")");
		lb->SetItemData(lb->AddString(cline.GetBuffer(1)), index);
		cline.ReleaseBuffer();
		APTcount++;
	} else if (5 == dbr.type) {
		sprintf(line, "Inner marker");
		lb->SetItemData(lb->AddString(line), index);
		MKRcount++;
	} else if (6 == dbr.type) {
		sprintf(line, "Middle marker");
		lb->SetItemData(lb->AddString(line), index);
		MKRcount++;
	} else if (7 == dbr.type) {
		sprintf(line, "Outer marker");
		lb->SetItemData(lb->AddString(line), index);
		MKRcount++;
	} else if (8 == dbr.type) {
		sprintf(line, "Runway  %s", dbr.id);
		lb->SetItemData(lb->AddString(line), index);
		RWYcount++;
	} else if (9 == dbr.type) {
		sprintf(line, "GPS fix %s\t\t%s", dbr.id, dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		GPScount++;
	} else if (10 == dbr.type) {
		sprintf(line, "ATIS station %s", dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		ATIScount++;
	} else if (11 == dbr.type) {
		sprintf(line, "Dynamic Object %s", dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		DYNcount++;
	} else if (12 == dbr.type) {
		sprintf(line, "Dynamic Scenery %s", dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		DYNcount++;
	} else if (13 == dbr.type) {
		sprintf(line, "Waypoint %s", dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		WAYcount++;
	} else if (14 == dbr.type) {
		sprintf(line, "Route segment %s", dbr.name);
		lb->SetItemData(lb->AddString(line), index);
		RTEcount++;
	}
}

void DataDialog::OnPlan() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	lb = (CListBox *) GetDlgItem(IDC_LIST);
	int lbindex = lb->GetCurSel();
	if (lbindex == LB_ERR)
		return;
	int dbindex = lb->GetItemData(lbindex);
	
	view->SendToPlan(dbindex);
}

void DataDialog::OnSummary() {
	char buf[4096];	
	int c = 0;

	c += sprintf(&buf[c], "Currently displaying:\n");
	c += sprintf(&buf[c], "%d VORs\n", VORcount);
	c += sprintf(&buf[c], "%d NDBs\n", NDBcount);
	c += sprintf(&buf[c], "%d Airports\n", APTcount);
	c += sprintf(&buf[c], "%d Runways\n", RWYcount);
	c += sprintf(&buf[c], "%d ILS beacons\n", ILScount);
	c += sprintf(&buf[c], "%d Markers\n", MKRcount);
	c += sprintf(&buf[c], "%d ATIS stations\n", ATIScount);
	c += sprintf(&buf[c], "%d GPS fixes\n", GPScount);
	c += sprintf(&buf[c], "%d Dynamic objects\n", DYNcount);
	c += sprintf(&buf[c], "%d Waypoints\n", WAYcount);
	c += sprintf(&buf[c], "%d Route segments\n", RTEcount);
	c += sprintf(&buf[c], "%d Total objects\n", VORcount+NDBcount+APTcount+RWYcount+ILScount+MKRcount+ATIScount+GPScount+DYNcount+WAYcount+RTEcount);

	AfxMessageBox(buf);
}
