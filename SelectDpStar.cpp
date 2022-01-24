// SelectDpStar.cpp : implementation file
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
#include "SelectDpStar.h"
#include "DpStar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelectDpStar dialog


SelectDpStar::SelectDpStar(CWnd* pParent /*=NULL*/)
	: CDialog(SelectDpStar::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelectDpStar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	isStar = FALSE;
	cdist = -1.0;
	clat = -100.0;
}


void SelectDpStar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelectDpStar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectDpStar, CDialog)
	//{{AFX_MSG_MAP(SelectDpStar)
	ON_LBN_DBLCLK(IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectDpStar message handlers

BOOL SelectDpStar::OnInitDialog() {
	CDialog::OnInitDialog();

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	DpStarAirport* airport = doc->dpStar.lookupAirport(icao);
	if (isStar) {
		this->SetWindowText("Select Standard Approach Route");
		airport->listSTARs(names);
	} else {
		this->SetWindowText("Select Departure Procedure");
		airport->listDPs(names);
	}

	CListBox *list = (CListBox *) GetDlgItem(IDC_LIST);
	int index = list->AddString(buildName(airport, "None"));
	list->SetCurSel(index);
	for (int i=0; i<names.GetSize(); i++) {
		index = list->AddString(buildName(airport, names[i]));
		if (names[i] == dpStarName)
			list->SetCurSel(index);
	}

	return TRUE; 
}

void SelectDpStar::OnDblclkList() {
	OnOK();	
}

void SelectDpStar::OnOK() {
	CListBox *list = (CListBox *) GetDlgItem(IDC_LIST);
	int sel = list->GetCurSel(); 
	list->GetText(sel, result);
	int tab = result.Find('\t') + 1;
	result = result.Right(result.GetLength() - tab);

	CDialog::OnOK();
}

CString SelectDpStar::buildName(DpStarAirport* airport, CString route) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	DpStarRoute *r;
	DpStarWaypoint *w;

	if (cdist == -1.0) { // calculate constant part of route
		double slat, slon;
		cdist = 0.0;
		int wStar, wDp; 

		if (isStar) {
			wStar = fp->selToPlan(fp->wptStar);
			for (int i=0; i<wStar; i++) {
				if (i>0)
					cdist += view->gcDist(clat, clon, doc->db[view->plan[i]].lat, doc->db[view->plan[i]].lon);
				clat = doc->db[view->plan[i]].lat;
				clon = doc->db[view->plan[i]].lon;
			}
		} else {
			wDp = fp->selToPlan(fp->wptDp);
			for (int i=wDp+1; i<view->plancount; i++) {
				if (i==wDp+1) {
					clat = doc->db[view->plan[i]].lat;
					clon = doc->db[view->plan[i]].lon;
				} else
					cdist += view->gcDist(slat, slon, doc->db[view->plan[i]].lat, doc->db[view->plan[i]].lon);
				slat = doc->db[view->plan[i]].lat;
				slon = doc->db[view->plan[i]].lon;
			}
		}
	}

	double rdist = 0.0;  // dp or star distance
	if (route == "None") {
		if (clat != -100) // distance to constant part of route
			rdist = view->gcDist(clat, clon, doc->db[airportIndex].lat, doc->db[airportIndex].lon);
	} else {
		// find route
		if (isStar)
			r = airport->firstSTAR();
		else
			r = airport->firstDP();
		while (r != NULL) {
			if (r->name == route)
				break;
			if (isStar)
				r = airport->nextSTAR();
			else
				r = airport->nextDP();
		}
		// route distance
		double slat, slon, startlat, startlon;
		int count = 0;
		w = r->firstWaypoint();
		while (w != NULL) {
			if (count == 0) {
				startlat = w->latitude;
				startlon = w->longitude;
			} else 
				rdist += view->gcDist(slat, slon, w->latitude, w->longitude);
			slat = w->latitude;
			slon = w->longitude;

			count++;
			w = r->nextWaypoint();
		}
		// distance to airport
		if (isStar)
			rdist += view->gcDist(slat, slon, doc->db[airportIndex].lat, doc->db[airportIndex].lon);
		else
			rdist += view->gcDist(startlat, startlon, doc->db[airportIndex].lat, doc->db[airportIndex].lon);
		// distance to constant part of route
		if (clat != -100) {
			if (isStar)
				rdist += view->gcDist(startlat, startlon, clat, clon);
			else
				rdist += view->gcDist(slat, slon, clat, clon);
		}
	}

	char distString[10];
	sprintf(distString, "%5.0lf nm\t", (cdist + rdist)/1852.0);
	return CString(distString) + route;
}
