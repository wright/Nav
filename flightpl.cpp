// flightpl.cpp : implementation file
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
#include "string.h"
#include "datadial.h"
#include "search.h"
#include "details.h"
#include "Aircraft.h"
#include <math.h>
#include "Wind.h"
#include <afxdlgs.h>
#include <stdlib.h>
#include <time.h>
#include "CreateDpStar.h"
#include "SelectDpStar.h"

#define pi			 3.1415946536
#define degToRad     (pi / 180.0)
#define CLIMB 1
#define CRUISE 2
#define DESCEND 3
#define PLANVERSION 10020

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// struct, holds Super Flight Planner like flight plan info
#pragma pack(1)
typedef struct {
	int type;
	double lat;
	double lon;
	double alt;
	double freq;
	char id[6];
	char name[31];
	double magdev;
	double approach;
	double range;
	double windVel;
	double windheading;
	double heading;
	double dist;
	double Tdist;
	double ete;
	double Tete;
	double fuel;
	double Tfuel;
	double correctedheading;
	double correctedvelocity;
	int reserved[3];
} tPlanRec;
#pragma pack(4)

/////////////////////////////////////////////////////////////////////////////
// FlightPlan dialog


FlightPlan::FlightPlan(CWnd* pParent /*=NULL*/)
	: CDialog(FlightPlan::IDD, pParent)
{
	plantable.SetSize(0,20);
	excessWind = FALSE;
	openPlanName = CString("");
	alternateID = CString("");
	alternateName = CString("");
	//{{AFX_DATA_INIT(FlightPlan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_NAVPLAN);
	updating = FALSE;
}

FlightPlan::~FlightPlan() {
	if (oldImageList != NULL)
		m_list.SetImageList(oldImageList, LVSIL_SMALL);   
	plantable.RemoveAll();
	plantable.FreeExtra();
}

void FlightPlan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FlightPlan)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(FlightPlan, CDialog)
	//{{AFX_MSG_MAP(FlightPlan)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_BN_CLICKED(IDC_AUTO, OnAuto)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST, OnBeginlabeleditList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_AIRCRAFT, OnAircraft)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_WIND, OnWind)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_FILE_EXPORTSQUAWKBOX, OnFileExportSquawkBox)
	ON_COMMAND(ID_FILE_EXPORT_ACSGPS, OnFileExportACSGPS)
	ON_COMMAND(ID_FILE_EXPORTSUPERFLIGHTPLANNER, OnFileExportSuperFlightPlanner)
	ON_COMMAND(ID_FILE_IMPORT_IMPORTFROMSUPERFLIGHTPLANNER, OnFileImportSuperFlightPlanner)
	ON_COMMAND(ID_FILE_IMPORTFROMOLDNAV1X, OnFileImportOldNav)
	ON_COMMAND(ID_FILE_EXPORTTORADARCONTACT99, OnFileExportRadarContact)
	ON_COMMAND(ID_FILE_EXPORTTOFLIGHTSIMULATOR2000, OnFileExportFS2000)
	ON_COMMAND(ID_FILE_IMPORTFROMFLIGHTSIMULATOR2000, OnFileImportFS2000)
	ON_BN_CLICKED(IDC_FLYTOPLANBUTTON, OnFlytoplan)
	ON_BN_CLICKED(IDC_DPSTAR, OnDpstar)
	ON_COMMAND(ID_AIRCRAFT, OnAircraft)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// FlightPlan message handlers

BOOL FlightPlan::OnInitDialog() {
	CDialog::OnInitDialog();
	long style = GetWindowLong(m_hWnd, GWL_STYLE);
	style |= LVS_SHOWSELALWAYS;
	SetWindowLong(m_hWnd, GWL_STYLE, style);
	style = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	style |= WS_EX_APPWINDOW;
	SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
	 
	// Set the icon for this dialog. 
	SetIcon(m_hIcon, TRUE);			// Set big icon

	imageList.Create(IDB_FLIGHTPLANIMAGELIST, 12, 12, RGB(255,255,255));
	oldImageList = m_list.SetImageList(&imageList, LVSIL_SMALL);   

	sel = -1;
	LV_COLUMN c;
	c.mask = LVCF_TEXT | LVCF_WIDTH;
	c.pszText = "Location";
	c.cx = m_list.GetStringWidth("XXXX_XXXX_XXXX_XXXX_");
	m_list.InsertColumn(0, &c);
	c.pszText = "ID";
	c.cx = m_list.GetStringWidth("XXXXXXX");
	m_list.InsertColumn(1, &c);
	c.pszText = "Type";
	c.cx = 50;
	m_list.InsertColumn(2, &c);
	c.pszText = "Freq";
	c.cx = m_list.GetStringWidth("XXXXXXX");
	m_list.InsertColumn(3, &c);
	c.pszText = "Head";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(4, &c);
	c.pszText = "Dist";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(5, &c);
	c.pszText = "Total";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(6, &c);
	c.pszText = "ETE";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(7, &c);
	c.pszText = "Total";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(8, &c);
	c.pszText = "Fuel";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(9, &c);
	c.pszText = "Total";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(10, &c);
	c.pszText = "Alt";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(11, &c);
	c.pszText = "Wind Vel";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(12, &c);
	c.pszText = "Wind Dir";
	c.cx = m_list.GetStringWidth("XXXXXXXX");
	m_list.InsertColumn(13, &c);

	m_list.SetItemState(0, LVNI_SELECTED, LVNI_SELECTED);
	m_list.SetItemState(0, LVIS_DROPHILITED, LVIS_DROPHILITED);
	m_list.SetItemState(0, 0, LVIS_FOCUSED);
	m_list.Update(0);
	m_list.EnsureVisible(0, FALSE);

	char buf[20];  // read default craft characteristics
	GetPrivateProfileString("Plan", "lastCraft", "Cessna 182", craftBuffer, 50, "nav.ini");
	GetPrivateProfileString(craftBuffer, "cruisealtitude", "9000", buf, 20, "nav.ini");
	sscanf(buf, "%ld", &cruisealtitude);
	GetPrivateProfileString(craftBuffer, "climbfuel", "14.1", buf, 20, "nav.ini");
	sscanf(buf, "%lf", &climbfuel);
	GetPrivateProfileString(craftBuffer, "cruisefuel", "14.1", buf, 20, "nav.ini");
	sscanf(buf, "%lf", &cruisefuel);
	GetPrivateProfileString(craftBuffer, "descentfuel", "14.1", buf, 20, "nav.ini");
	sscanf(buf, "%lf", &descentfuel);
	GetPrivateProfileString(craftBuffer, "climbspeed", "76", buf, 20, "nav.ini");
	sscanf(buf, "%d", &climbspeed);
	GetPrivateProfileString(craftBuffer, "cruisespeed", "140", buf, 20, "nav.ini");
	sscanf(buf, "%d", &cruisespeed);
	GetPrivateProfileString(craftBuffer, "descentspeed", "140", buf, 20, "nav.ini");
	sscanf(buf, "%d", &descentspeed);
	GetPrivateProfileString(craftBuffer, "climbrate", "900", buf, 20, "nav.ini");
	sscanf(buf, "%d", &climbrate);
	GetPrivateProfileString(craftBuffer, "descentrate", "1000", buf, 20, "nav.ini");
	sscanf(buf, "%d", &descentrate);

	wptDp = wptStar = -1;
	update();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void FlightPlan::OnAdd() {
	DataDialog d(this);	
	Search d2(this);
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	if (d2.DoModal() == IDOK) {
		view->search = d2.m_target;
		if (view->search != "")	{
			int c = view->plancount;
			d.DoModal();
			if (c != view->plancount) {
				view->abort = TRUE;
				view->Invalidate(TRUE);
			}
		}
	} 
	view->search = "";
}

// average two wind vectors (in magnitude and angle format)
void FlightPlan::averageWind(double m1, double a1, double m2, double a2, double &m3, double &a3, double percent) {
	ASSERT(percent >= 0.0);
	ASSERT(percent <= 1.0);
	int zero = ((m1 == 0) && (m2 == 0));
	if (zero) // special case to average angle correctly when magnitude is zero
		m1 = m2 = 1;
	double wx1 = m1 * sin(a1*degToRad);
	double wy1 = m1 * cos(a1*degToRad);
	double wx2 = m2 * sin(a2*degToRad);
	double wy2 = m2 * cos(a2*degToRad);
	double wx = (1-percent)*wx1 + percent*wx2;
	double wy = (1-percent)*wy1 + percent*wy2;
	m3 = sqrt(wx*wx+wy*wy);
	a3 = atan2(wx, wy) / degToRad;
	if (a3 < 0)
		a3 += 360;
	if (zero)
		m3 = 0;
}

// take wind into account for indicated heading and effective speed
int FlightPlan::wind(double &ihead, double ispeed, double ehead, double &espeed, double whead, double wspeed) {
	double wx = wspeed * sin(whead*degToRad);
	double wy = wspeed * cos(whead*degToRad);
	double st = sin(ehead*degToRad);
	double ct = cos(ehead*degToRad);
	double b = 2*(st*wx+ct*wy);
	double check = b*b-4*(wx*wx+wy*wy-ispeed*ispeed);
	if ((check < 0) || ((b<0) && (b*b > check))) {
		// clean up plan here
		if (!excessWind)
			AfxMessageBox("Wind exceeds airplane's capabilities");
		excessWind = TRUE;
		for (int p=1; p<plantable.GetSize(); p++) { 
			plantable[p].ete = 0;
			plantable[p].Tete  = 0;
			plantable[p].fuel = 0;
			plantable[p].Tfuel = 0;
		}
		return FALSE;
	}
	espeed = (b+sqrt(check))/2.0;
	ihead = atan2(espeed*st-wx, espeed*ct-wy) / degToRad;
	if (ihead<0)
		ihead += 360;
	return TRUE;
}

int FlightPlan::adjust(int i, int forward, int mode) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	double wspeed, whead, speed, rate, fuel, percent, talt;
	planRec toc;
	if ((mode == CLIMB) && (itoc != -1) && (itoc < i))
		mode = CRUISE;
	if ((mode == DESCEND) && (ibod != -1) && (ibod >= i))
		mode = CRUISE;
	if (mode == CLIMB) {
		speed = climbspeed;
		rate = climbrate;
		fuel = climbfuel;
	} else if (mode == CRUISE) {
		speed = cruisespeed;
		fuel = cruisefuel;
	} else if (mode == DESCEND) {
		speed = descentspeed;
		rate = descentrate;
		fuel = descentfuel;
	}
	averageWind(plantable[i-1].windVel, plantable[i-1].windheading, plantable[i].windVel, plantable[i].windheading, wspeed, whead);
	if (FALSE == wind(plantable[i].correctedheading, speed, plantable[i].heading, plantable[i].correctedvelocity, whead, wspeed))
		return FALSE; 
	if (mode == CRUISE)	{
		if (((i != plantable.GetSize()-1) || !D) && ((i != 0) || !C)) {
			plantable[i].alt = cruisealtitude;
		}
	} else if (forward) {
		talt = plantable[i-1].alt + (plantable[i].dist / plantable[i].correctedvelocity) * 60 * rate;
//		if (D && (i == plantable.GetSize()-1)) {
//			; // do nothing for now
//		} else 
		if ((talt > cruisealtitude) && (itoc == -1)) { 
			// need to insert toc
			if (i != plantable.GetSize()-1)
				plantable[i].alt = cruisealtitude;
			toc.index = -1; //special value for toc
			percent = (cruisealtitude - plantable[i-1].alt) / (talt - plantable[i-1].alt);
			view->gcDistInterpolate(plantable[i-1].lat, plantable[i-1].lon, plantable[i].lat, plantable[i].lon, percent, toc.lat, toc.lon);
			toc.alt = cruisealtitude;
			averageWind(plantable[i-1].windVel, plantable[i-1].windheading, plantable[i].windVel, plantable[i].windheading, toc.windVel, toc.windheading, percent);
 			toc.dist = view->gcDist(plantable[i-1].lat, plantable[i-1].lon, toc.lat, toc.lon)/1852; // in nm
			toc.heading = view->bearing + doc->GetMagVar(toc.lat, toc.lon);
 			plantable[i].dist = view->gcDist(toc.lat, toc.lon, plantable[i].lat, plantable[i].lon)/1852; // in nm
			plantable[i].heading = view->bearing + doc->GetMagVar(plantable[i].lat, plantable[i].lon);
			itoc = i;
			if ((ibod != -1) && (ibod > i))
				ibod++;
			plantable.InsertAt(i, toc); // insert toc record, shift others 
			if (FALSE == adjust(i, forward, CLIMB)) return FALSE;
			if (FALSE == adjust(i+1, forward, CRUISE)) return FALSE;
			return TRUE;
		} else if ((i == itoc) && (ibod != itoc))
			plantable[i].alt = cruisealtitude;
		else if ((i != plantable.GetSize()-1) || (C && !D)) {
			plantable[i].alt = talt;
			if (plantable[i].alt > cruisealtitude) {
				plantable[i].alt = cruisealtitude;
			}
		}
	} else {
		talt = plantable[i].alt + (plantable[i].dist / plantable[i].correctedvelocity) * 60 * rate;
		if (C && D && (talt > plantable[i-1].alt) && (plantable[i-1].alt < .98*cruisealtitude) && !((itoc==ibod)&&(itoc!=-1))) {
			//delete old toc & recalculate
			if (itoc != -1) {
				plantable.RemoveAt(itoc);
				if (i > itoc) 
					i -= 1;
				int del = itoc;
				itoc = ibod = -2; // fixup spot where toc used to be
 				plantable[del].dist = view->gcDist(plantable[del-1].lat, plantable[del-1].lon, plantable[del].lat, plantable[del].lon)/1852; // in nm
				plantable[del].heading = view->bearing + doc->GetMagVar(plantable[del].lat, plantable[del].lon);
				if (FALSE == adjust(del, FALSE, DESCEND)) return FALSE;
			}
			// need to insert toc/bod
			toc.index = -3; // special value for toc/bod
			double cspeed, dspeed; // take wind into account for speed calculations
			if (FALSE == wind(plantable[i].correctedheading, climbspeed, plantable[i].heading, cspeed, whead, wspeed))
				return FALSE; 
			if (FALSE == wind(plantable[i].correctedheading, descentspeed, plantable[i].heading, dspeed, whead, wspeed)) 
				return FALSE; 
			percent = plantable[i].alt - plantable[i-1].alt + plantable[i].dist*60*descentrate/dspeed;
			percent /= plantable[i].dist*60*(climbrate/cspeed+descentrate/dspeed);
			view->gcDistInterpolate(plantable[i-1].lat, plantable[i-1].lon, plantable[i].lat, plantable[i].lon, percent, toc.lat, toc.lon);
			averageWind(plantable[i-1].windVel, plantable[i-1].windheading, plantable[i].windVel, plantable[i].windheading, toc.windVel, toc.windheading, percent);
 			toc.dist = view->gcDist(plantable[i-1].lat, plantable[i-1].lon, toc.lat, toc.lon)/1852; // in nm
			toc.heading = view->bearing + doc->GetMagVar(toc.lat, toc.lon);
 			plantable[i].dist = view->gcDist(toc.lat, toc.lon, plantable[i].lat, plantable[i].lon)/1852; // in nm
			plantable[i].heading = view->bearing + doc->GetMagVar(plantable[i].lat, plantable[i].lon);
			itoc = ibod = i;
			plantable.InsertAt(i, toc); // insert toc/bod record, shift others 
			if (FALSE == adjust(i,   TRUE,  CLIMB))  return FALSE;
			if (FALSE == adjust(i+1, FALSE, DESCEND)) return FALSE;
			return TRUE;
		} else if ((talt > cruisealtitude) && (ibod == -1)) { 
			// need to insert bod
			toc.index = -2; // special value for bod
			percent = (talt - cruisealtitude) / (talt - plantable[i].alt);
			view->gcDistInterpolate(plantable[i-1].lat, plantable[i-1].lon, plantable[i].lat, plantable[i].lon, percent, toc.lat, toc.lon);
			toc.alt = cruisealtitude;
			averageWind(plantable[i-1].windVel, plantable[i-1].windheading, plantable[i].windVel, plantable[i].windheading, toc.windVel, toc.windheading, percent);
 			toc.dist = view->gcDist(plantable[i-1].lat, plantable[i-1].lon, toc.lat, toc.lon)/1852; // in nm
			toc.heading = view->bearing + doc->GetMagVar(toc.lat, toc.lon);
 			plantable[i].dist = view->gcDist(toc.lat, toc.lon, plantable[i].lat, plantable[i].lon)/1852; // in nm
			plantable[i].heading = view->bearing + doc->GetMagVar(plantable[i].lat, plantable[i].lon);
			ibod = i;
			plantable.InsertAt(i, toc); // insert toc record, shift others 
			if (FALSE == adjust(i,   forward, CRUISE))  return FALSE;
			if (FALSE == adjust(i+1, forward, DESCEND)) return FALSE;
			return TRUE;
		} else if ((itoc==-2)&&(ibod==-2))
			; // do nothing
		else if (((i-1) == ibod) && (ibod != itoc))
			plantable[i-1].alt = cruisealtitude;
		else 
			plantable[i-1].alt = min (talt, cruisealtitude) ;
	}
 	plantable[i].ete = plantable[i].dist / plantable[i].correctedvelocity * 60; //minutes
	plantable[i].fuel = fuel * plantable[i].ete / 60;

	return TRUE;
}

void FlightPlan::buildTable() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	plantable.RemoveAll();  // delete old table
	plantable.FreeExtra();
	if (view->plancount < 1)
		return;

	double windspeed, windir, oldLat, oldLon;
	int type;
	windStruct w;
	// insert known items into table
	for (int p=0; p<(view->plancount); p++) {
		planRec r;
		r.index = view->plan[p];
		type = doc->db[r.index].type;
		r.lat = doc->db[r.index].lat;
		r.lon = doc->db[r.index].lon;
		if (view->windtable.Lookup(r.index, w)) {
			windspeed = (double) w.speed;
			windir = (double) w.direction;
		} else
			windspeed = windir = 0.0;
		r.windVel = windspeed;	// correct wind heading for mag variation (& change "from" to "to")							
		r.windheading = windir + 180 + doc->GetMagVar(doc->db[r.index].lat, doc->db[r.index].lon);	
		if (r.windheading > 360)
			r.windheading -= 360;
		if (r.windheading < 0)
			r.windheading += 360;
		if (p == 0) {
			r.dist = r.Tdist = r.ete = r.Tete = r.fuel = r.Tfuel = 0.0; 
			r.heading = r.correctedheading = r.correctedvelocity = 0;
			r.alt = doc->db[r.index].alt/.3048; // convert to ft
			if (((r.alt <= 0)&&(type != 4)) || ((type != 2)&&(type != 4)&&(type != 8))) // some things don't have a valid altitude, so cruise
				r.alt = cruisealtitude;
		} else {
			r.dist = view->gcDist(oldLat, oldLon, doc->db[r.index].lat, doc->db[r.index].lon)/1852; // in nm
			r.heading = view->bearing + doc->GetMagVar(doc->db[r.index].lat, doc->db[r.index].lon);
			if (r.heading <= 0)
				r.heading += 360;
			if (p == view->plancount-1) {
				r.alt = doc->db[r.index].alt/.3048; // convert to ft
				if (((r.alt <= 0)&&(type != 4)) || ((type != 2)&&(type != 4)&&(type != 8))) // some things don't have a valid altitude, so cruise
					r.alt = cruisealtitude;
			} else
				r.alt = cruisealtitude;
		}
		oldLat = r.lat;
		oldLon = r.lon;
		plantable.Add(r);
	}

	if (view->plancount < 2) {
		checkDpStar();
		return;
	}

	for (p=1; p<(view->plancount); p++) // insert inital cruise data
		adjust(p, TRUE, CRUISE);

	itoc = ibod = -1;
	D = C = TRUE;
	// ILS, runways and airports use their own altitude, others use cruise altitude
	int idp = doc->db[plantable[plantable.GetSize()-1].index].type; 
	if ( (idp != 2)&&(idp != 4)&&(idp != 8) )
		D = FALSE;
	idp = doc->db[plantable[0].index].type; 
	if ( (idp != 2)&&(idp != 4)&&(idp != 8) )
		C = FALSE;

	if (C || !D) {
		for (int p=1; p<plantable.GetSize(); p++) {
			if (p == itoc+1)
				continue;
			if (!C)
				adjust(p, TRUE, CRUISE);
			else
				adjust(p, TRUE, CLIMB);
		}
	}
	if (D) {
		for (p=plantable.GetSize()-1; p>0; p--) {
			if (p <= ibod)
				break;
			adjust(p, FALSE, DESCEND);
		}
	}

	for (p=1; p<plantable.GetSize(); p++) { // update totals
		plantable[p].Tdist = plantable[p-1].Tdist + plantable[p].dist;
		plantable[p].Tete  = plantable[p-1].Tete  + plantable[p].ete;
		plantable[p].Tfuel = plantable[p-1].Tfuel + plantable[p].fuel;
	}
	checkDpStar();
}

CString FlightPlan::PlanTableName(int planItem) {
	ASSERT (planItem >= 0);
	ASSERT (planItem < plantable.GetSize());

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	int dbtype;

	if (plantable[planItem].index >= 0)
		dbtype = doc->db[plantable[planItem].index].type;
	else 
		dbtype = plantable[planItem].index; // toc, bod, toc/bob
	
	CString name;
	if (-1 == dbtype) {
		name = "top of climb";
	} else if (-2 == dbtype) {
		name = "beginning of descent";
	} else if (-3 == dbtype) {
		name = "top climb/begin descent";
	} else if (5 == dbtype) {
		name = "Inner marker";	
	} else if (6 == dbtype) {
		name = "Middle marker";	
	} else if (7 == dbtype) {
		name = "Outer marker";	
	} else if (8 == dbtype) {
		name = "Runway ";
		name += doc->db[plantable[planItem].index].id;	
	} else if (2 == dbtype) {
		name = "ILS - ";
		name += doc->db[plantable[planItem].index].name;	
	} else {
		name = doc->db[plantable[planItem].index].name;
	}
	name.TrimRight();
	return name;
}

void FlightPlan::update() {
	if (updating) {// avoid weird recursive updating
		Beep(100,100);
		return;
	}
	updating = TRUE;
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
    m_list.DeleteAllItems();
	LV_ITEM i;
	i.mask = LVIF_TEXT|LVIF_IMAGE;
	char line[50];
	int dbtype;
	CString name;

	buildTable();

	for (int k=0; k<plantable.GetSize(); k++) {
 		i.iItem = k;	   
		i.iSubItem = 0;
		i.iImage = getImage(k);
		if (plantable[k].index >= 0)
			dbtype = doc->db[plantable[k].index].type;
		else 
			dbtype = plantable[k].index; // toc, bod, toc/bod

		i.iImage = getImage(k); // set plan item icon

		name = PlanTableName(k);
		i.pszText = name.GetBuffer(1);
		m_list.InsertItem(&i);
		name.ReleaseBuffer();

		if ((1 == dbtype) || (2 == dbtype) || (3 == dbtype)) {
			i .iSubItem = 1;
			i.pszText = doc->db[plantable[k].index].id;
			m_list.SetItem(&i);
			sprintf(line, "%6.2f", doc->db[plantable[k].index].freq);
			i .iSubItem = 3;
			i.pszText = line;
			m_list.SetItem(&i);
		}

		if (4 == dbtype) {
			CString icao;
			if (doc->lookUpAirport(icao, plantable[k].lat, plantable[k].lon)) {
				i .iSubItem = 1;
				i.pszText = icao.GetBuffer(1);;
				m_list.SetItem(&i);
				icao.ReleaseBuffer();
			}
		}

		if (10 == dbtype) {
			i .iSubItem = 1;
			i.pszText = "ATIS";
			m_list.SetItem(&i);
			sprintf(line, "%6.2f", doc->db[plantable[k].index].freq);
			i .iSubItem = 3;
			i.pszText = line;
			m_list.SetItem(&i);
		}

		if (11 == dbtype) {
			i .iSubItem = 1;
			i.pszText = "Dynamic Object ";
			m_list.SetItem(&i);
			sprintf(line, "%s", doc->db[plantable[k].index].name);
			i .iSubItem = 3;
			i.pszText = line;
			m_list.SetItem(&i);
		}

		if (12 == dbtype) {
			i .iSubItem = 1;
			i.pszText = "Dynamic Scenery ";
			m_list.SetItem(&i);
			sprintf(line, "%s", doc->db[plantable[k].index].name);
			i .iSubItem = 3;
			i.pszText = line;
			m_list.SetItem(&i);
		}

		if ((8 == dbtype) || (9 == dbtype) || (13 == dbtype) || (14 == dbtype)) {
			i .iSubItem = 1;
			i.pszText = doc->db[plantable[k].index].id;
			m_list.SetItem(&i);
		}

		if ((-1 == dbtype) || (-2 == dbtype) || (-3 == dbtype)) {
			i .iSubItem = 1;
			sprintf(line, "%dft", (int) (plantable[k].alt + .5));
			i.pszText = line;
			m_list.SetItem(&i);
		}

		if (1 == dbtype)
			strcpy(line, "NDB");	
		else if (2 == dbtype)
			strcpy(line, "ILS");	
		else if (3 == dbtype)
			strcpy(line, "VOR");	
		else if (4 == dbtype)
			strcpy(line, "Airport");	
		else if (5 == dbtype)
			strcpy(line, "Marker");	
		else if (6 == dbtype)
			strcpy(line, "Marker");	
		else if (7 == dbtype)
			strcpy(line, "Marker");	
		else if (8 == dbtype)
			strcpy(line, "Airport");	
		else if (9 == dbtype)
			strcpy(line, "GPS fix");	
		else if (10 == dbtype)
			strcpy(line, "ATIS");	
		else if (11 == dbtype)
			strcpy(line, "DYNOBJ");	
		else if (12 == dbtype)
			strcpy(line, "DYNSCN");	
		else if (13 == dbtype)
			strcpy(line, "Waypoint");	
		else if (14 == dbtype)
			strcpy(line, "Route");	
		else if (-1 == dbtype)
			strcpy(line, "TOC");	
		else if (-2 == dbtype)
			strcpy(line, "BOD");	
		else if (-3 == dbtype)
			strcpy(line, "TOC/BOD");	
		i .iSubItem = 2;
		i.pszText = line;
		m_list.SetItem(&i);

		sprintf(line, "%3.0f", plantable[k].correctedheading);
		i .iSubItem = 4;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%.1f", plantable[k].dist);
		i .iSubItem = 5;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%.1f", plantable[k].Tdist);
		i .iSubItem = 6;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%.1f", plantable[k].ete);
		i .iSubItem = 7;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%.1f", plantable[k].Tete);
		i .iSubItem = 8;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%.1f", plantable[k].fuel);
		i .iSubItem = 9;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%.1f", plantable[k].Tfuel);
		i .iSubItem = 10;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%d", (int)(plantable[k].alt + .5));
		i .iSubItem = 11;
		i.pszText = line;
		m_list.SetItem(&i);
		sprintf(line, "%d", (int)(plantable[k].windVel + .5));
		i .iSubItem = 12;
		i.pszText = line;
		m_list.SetItem(&i);
		int wh = (int)(plantable[k].windheading + 180.49 - doc->GetMagVar(plantable[k].lat, plantable[k].lon));
		wh %= 360;
		sprintf(line, "%d", wh);
		i .iSubItem = 13;
		i.pszText = line;
		m_list.SetItem(&i);
	}
	setColumnWidth();

	if (sel != -1) {
		m_list.SetItemState(sel, LVNI_SELECTED, LVNI_SELECTED);
		m_list.SetItemState(sel, LVIS_DROPHILITED, LVIS_DROPHILITED);
		m_list.SetItemState(sel, 0, LVIS_FOCUSED);
		m_list.Update(sel);
		m_list.EnsureVisible(sel, FALSE);
	}
	checkButtons();
	updating = FALSE;
}

// set columns to be as small as possible
void FlightPlan::setColumnWidth() {
	int t, max;
	char buf[100];
	LV_ITEM pItem;
	pItem.mask = LVIF_TEXT;
	pItem.pszText = buf;
	pItem.cchTextMax = 100;
	LV_COLUMN pColumn;
	pColumn.mask = LVCF_TEXT;
	pColumn.pszText = buf;
	pColumn.cchTextMax = 100;
	for (int col=0; col<=13; col++) {
		pColumn.iSubItem = col;
		m_list.GetColumn(col, &pColumn);
		max = m_list.GetStringWidth(pColumn.pszText);
		for (int k=0; k<plantable.GetSize(); k++) {
			pItem.iItem = k;
			pItem.iSubItem = col;
			m_list.GetItem(&pItem);
			t = m_list.GetStringWidth(pItem.pszText);
			if (t>max)
				max = t;
		}
		// despite the docs, it does not take column margin into account
		m_list.SetColumnWidth(col, max + 13 + (col==0 ? 12 : 0)); 
	}
}

void FlightPlan::OnClear() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	view->stopFlyTo();
	view->plancount = 0;
	view->windtable.RemoveAll();
	view->abort = TRUE;
	view->Invalidate(TRUE);
	update();
}

void FlightPlan::OnOK() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	view->planning = FALSE;
	view->abort = TRUE;
	view->Invalidate(TRUE);
	CDialog::OnOK();
}

void FlightPlan::OnClose() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	view->stopFlyTo();
	view->planning = FALSE;
	view->abort = TRUE;
	view->Invalidate(TRUE);

	DestroyWindow(); // kill self, PostNcDestroy deletes memory	
}

void FlightPlan::OnUp() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	// check for valid selection
	if (view->plancount < 2)
		return;
	int selected = getSel();
	if (selected == -1) // nothing was selected
		return;
	int t = plantable[selected].index; // skip toc/bod if selected
	if ((t == -1) || (t == -2) | (t == -3))
		return;
	if (selected == 0) // beginning of list
		return;

	// swap
	int iplan = selToPlan(selected);
	int temp = view->plan[iplan-1];
	view->plan[iplan-1] = view->plan[iplan];
	view->plan[iplan] = temp;
	update(); // toc/bod/etc might have changed
	sel = planToSel(iplan-1);

	// update UI
	checkButtons(); 
	m_list.SetItemState(selected, 0, LVNI_SELECTED);
	m_list.SetItemState(sel, LVNI_SELECTED, LVNI_SELECTED);
	m_list.SetItemState(sel, LVIS_DROPHILITED, LVIS_DROPHILITED);
	m_list.SetItemState(sel, 0, LVIS_FOCUSED);
	if ((view->flyToWaypoint-1) == selected)
		view->FlyToWaypoint(view->flyToWaypoint-1);
	view->Invalidate(TRUE);
}

void FlightPlan::OnDown() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	// check for valid selection
	if (view->plancount < 2)
		return;
	int selected = getSel();
	if (selected == -1) // nothing was selected
		return;
	int t = plantable[selected].index; // skip toc/bod if selected
	if ((t == -1) || (t == -2) | (t == -3))
		return;
	if (selected == plantable.GetSize()-1) // end of list
		return;

	// swap
	int iplan = selToPlan(selected);
	int temp = view->plan[iplan+1];
	view->plan[iplan+1] = view->plan[iplan];
	view->plan[iplan] = temp;
	update(); // toc/bod/etc might have changed
	sel = planToSel(iplan+1);

	// update UI
	checkButtons(); 
	m_list.SetItemState(selected, 0, LVNI_SELECTED);
	m_list.SetItemState(sel, LVNI_SELECTED, LVNI_SELECTED);
	m_list.SetItemState(sel, LVIS_DROPHILITED, LVIS_DROPHILITED);
	m_list.SetItemState(sel, 0, LVIS_FOCUSED);
	if ((view->flyToWaypoint-1) == selected)
		view->FlyToWaypoint(view->flyToWaypoint+1);
	view->Invalidate(TRUE);
}

void FlightPlan::OnDelete() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	// check for valid selection
	if (view->plancount == 0)
		return;
		
	int selected = getSel();
	if (selected == -1) // nothing was selected
		return;
	int t = plantable[selected].index; // skip toc/bod if selected
	if ((t == -1) || (t == -2) | (t == -3))
		return;

	view->windtable.RemoveKey(plantable[selected].index);

	if ((view->flyToWaypoint-1) == selected)
		view->stopFlyTo();

	int iplan = selToPlan(selected);
	for (int i=iplan+1; i<view->plancount; i++) 
		view->plan[i-1] = view->plan[i];
	view->plancount -=1;
	update(); // toc/bod/etc might have changed

	sel = -1; // no selection now
	view->Invalidate(TRUE);
}

void FlightPlan::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) {	
	int newSel = 0;
	for (int i=0; i<m_list.GetItemCount(); i++) {
		if(m_list.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED) {
			newSel= i;
			break;
		}
	}

	if (sel != -1) { // remove old highlighting (sometimes get stuck)
		m_list.SetItemState(sel, 0, LVNI_SELECTED);
		m_list.SetItemState(sel, 0, LVIS_DROPHILITED);
		m_list.SetItemState(sel, 0, LVIS_FOCUSED);
		m_list.Update(sel);
	}

	sel = newSel;
	m_list.SetItemState(sel, LVNI_SELECTED, LVNI_SELECTED);
	m_list.SetItemState(sel, LVIS_DROPHILITED, LVIS_DROPHILITED);
	m_list.SetItemState(sel, 0, LVIS_FOCUSED);
	m_list.Update(sel);

	checkButtons();
	*pResult = 0;
}

void FlightPlan::checkButtons() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	int selected = getSel();

	CButton *flyToButton;
	flyToButton = (CButton *) GetDlgItem(IDC_FLYTOPLANBUTTON);
	if (view->isLinked() && (selected != -1))
		flyToButton->EnableWindow(TRUE);
	else
		flyToButton->EnableWindow(FALSE);

	if (selected >=0) {
		int t = plantable[selected].index; // toc/bod is selected
		if ((t == -1) || (t == -2) | (t == -3))
			selected = -1;  // act as if nothing is selected
	}

	CButton *clearButton;
	clearButton = (CButton *) GetDlgItem(IDC_CLEAR);
	if (plantable.GetSize() == 0) 
		clearButton->EnableWindow(FALSE);
	else
		clearButton->EnableWindow(TRUE);

	CButton *deleteButton;
	deleteButton = (CButton *) GetDlgItem(IDC_DELETE);
	if (selected == -1) 
		deleteButton->EnableWindow(FALSE);
	else
		deleteButton->EnableWindow(TRUE);

	CButton *upButton;
	upButton = (CButton *) GetDlgItem(IDC_UP);
	if (selected < 1) 
		upButton->EnableWindow(FALSE);
	else
		upButton->EnableWindow(TRUE);

	CButton *downButton;
	downButton = (CButton *) GetDlgItem(IDC_DOWN);
	if ((selected == plantable.GetSize()-1) || (selected < 0)) 
		downButton->EnableWindow(FALSE);
	else
		downButton->EnableWindow(TRUE);

	CButton *autoButton;
	autoButton = (CButton *) GetDlgItem(IDC_AUTO);
	if ((view->plancount < 2) || (selected == 0))
		autoButton->EnableWindow(FALSE);
	else
		autoButton->EnableWindow(TRUE);

	CButton *windButton; // need at least one item to set wind
	windButton = (CButton *) GetDlgItem(IDC_WIND);
	if (view->plancount < 1)
		windButton->EnableWindow(FALSE);
	else
		windButton->EnableWindow(TRUE);

	CButton *dpstarButton; 
	dpstarButton = (CButton *) GetDlgItem(IDC_DPSTAR);
		dpstarButton->EnableWindow(enableDPSTARButton());
}

void FlightPlan::OnAuto() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	char sbar[300];

	if (view->plancount < 2)
		return;
		
	int selected = getSel();
	if (selected == -1) // no selection, assume last entry
		selected = plantable.GetSize()-1;

	for (int i=0; i<=selected; i++) {
		int t = plantable[i].index; // toc/bod is not in plan, so ignore
		if ((t == -1) || (t == -2) | (t == -3))
			selected -= 1;  
	}

	if (selected == 0) // nothing to route if first entry is selected
		return;

	if ((wptStar != -1) && (selected > selToPlan(wptStar))) // check for STAR
		selected = selToPlan(wptStar); // don't autoroute within a STAR

	// maybe insert nearest VOR ot NDB to endpoint for RC99
	if (selected == view->plancount-1) { // routing to last waypoint
		int type = doc->db[view->plan[selected]].type;
		if ((type == 4) || (type == 8)) { // it's an airport or runway
			double slat = doc->db[view->plan[selected]].lat; 
			double slon = doc->db[view->plan[selected]].lon; 

			double llat = doc->db[view->plan[selected-1]].lat; 
			double llon = doc->db[view->plan[selected-1]].lon; 
			view->gcDist(llat, llon, slat, slon);
			double desiredHeading = view->bearing;

			int inserted = FALSE;
			int nearest = doc->findNearestInlineNavaid(llat, llon, slat, slon, 50000); // 50 km is close enough
			if (nearest != -1) { // found something
				inserted = TRUE;
				if (nearest != view->plan[view->plancount-2]) {// make sure it's not already there
					view->plan[view->plancount] = view->plan[selected];
					view->plancount += 1;
					view->plan[selected] = nearest;
				}
			}
			if (!inserted) { // try again with NDB
				nearest = doc->findNearestInlineNavaid(llat, llon, slat, slon, 50000, 1); // 50 km is close enough
				if (nearest != -1) { // found something
					if (nearest != view->plan[view->plancount-2]) {// make sure it's not already there
						view->plan[view->plancount] = view->plan[selected];
						view->plancount += 1;
						view->plan[selected] = nearest;
					}
				}
			}
		}
	}

	int start = view->plan[selected-1]; 
	int end = view->plan[selected];
	double startx = doc->db[start].lon;
	double starty = doc->db[start].lat;
	double endx = doc->db[end].lon;
	double endy = doc->db[end].lat;
	double ideal = view->idealDist*1000; // in km
	double percent, midx, midy, startDist, newdist;
	int index, type;
	int found, recover;
	MSG msg;  
 
	double dist = view->gcDist(starty, startx, endy, endx);

	CButton *autoButton;
	autoButton = (CButton *) GetDlgItem(IDC_AUTO);
	autoButton->EnableWindow(FALSE);
	while (dist > ideal) {
		if (dist > 2*ideal)
			percent = ideal / dist;
		else 
			percent = .5;
		view->gcDistInterpolate(starty, startx, endy, endx, percent, midy, midx);

		sprintf(sbar, "Routing from %s", doc->db[start].name);
		main->SetStatusBar(sbar);

		// find nearest station to midx, midy
		startDist = dist;
		newdist = 2.0 * ideal;
		found = FALSE;
		recover = FALSE;
			
		for (int i=0; i<doc->dbCount; i++) {
			type = doc->db[i].type;
			if (type == 2) goto next;
			if (type == 5) goto next;
			if (type == 6) goto next;
			if (type == 7) goto next;
			if (type == 9) goto next;
			if (type == 10) goto next;
			if (type == 11) goto next;
			if (type == 12) goto next;
			if (type == 14) goto next;
			if ((type == 1) && (!view->planNDB)) goto next;
			if ((type == 3) && (!view->planVOR)) goto next;
			if ((type == 4) && (!view->planAirport)) goto next;
			if ((type == 8) && (!view->planAirport)) goto next;
			if ((type == 13) && (!view->planWaypoints)) goto next;

			if ((type == 1) || (type == 3) || (type == 4) || (type == 8) || (type == 13)) {				
				// take over message processing to simulate threading 
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
					if (msg.message == WM_QUIT) { 
			 			PostQuitMessage(1);
			 			return;      
					}	 
			 		TranslateMessage(&msg);
			 		DispatchMessage(&msg);
	  			}
				// continue
				newdist = view->gcDist(midy, midx, doc->db[i].lat, doc->db[i].lon);
				if ((newdist < startDist) && (i != start)) {
					startDist = newdist;
					found = TRUE;
					index = i;
				}
			}
			next:;
		} // index is closest navaid to midx, midy
		if (found == FALSE) { 
			recover = TRUE;
		} else { // found == TRUE
			newdist = view->gcDist(doc->db[index].lat, doc->db[index].lon, doc->db[end].lat, doc->db[end].lon);
			if (newdist > (dist - 1000))  // must make at least a 1 km improvement
				recover = TRUE;
			else {
				double legdist = view->gcDist(doc->db[index].lat, doc->db[index].lon, doc->db[start].lat, doc->db[start].lon);
				if (legdist > 2*ideal)
					recover = TRUE;
			}
		}

		if (recover == FALSE) {
			if(index == view->plan[selected])  // found end point, we must be done
				break;
			// insert the new navaid in the plan
			view->plancount += 1;
			for (int k=view->plancount-1; k>selected; k--) 
			 	view->plan[k] = view->plan[k-1];
			view->plan[k] = index;
			selected++;
			
			// repeat until done
			start = index;
			startx = doc->db[index].lon;
			starty = doc->db[index].lat;
			dist = newdist;
		} else { // recover by pretending we found something and move on or insert GPS fix
			if (view->planContinue) { //insert a GPS fix
				doc->OnMapInsertgpsfix(midy, midx);
				index = doc->dbCount - 1;
				// insert the new gps in the plan
				view->plancount += 1;
				for (int k=view->plancount-1; k>selected; k--) 
			 		view->plan[k] = view->plan[k-1];
				view->plan[k] = index;
				selected++;
			}
			recover = FALSE;
			start = index;
			startx = midx;
			starty = midy;
			dist = view->gcDist(starty, startx, endy, endx);
		}
	}

	buildTable();
	autoButton->EnableWindow(TRUE);
	main->SetStatusBar("Ready");
	view->Invalidate(TRUE);
}

void FlightPlan::OnOptions() {
 	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	view->planOpts(this);
}

void FlightPlan::OnPrint() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	CFont font;
	TEXTMETRIC tm;

	if (view->plancount < 2)
		return;
	
	CPrintDialog pd(FALSE, PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS 
	              | PD_HIDEPRINTTOFILE | PD_NOSELECTION, this);

	if (pd.DoModal() == IDOK) {
		HDC hDC = pd.GetPrinterDC();
		if (hDC == NULL)
			return;
		CDC *pDC;
		pDC = pDC->FromHandle(hDC);

		typedef struct { // Win95 version since VC++ 2.1 has only NT version
		  int cbSize;
		  LPTSTR lpszDocName;
		  LPTSTR lpszOutput;
		  LPTSTR lpszDataType;
		  DWORD fwType;
		} W95DOCINFO;
		W95DOCINFO di;
	
		di.cbSize = sizeof(W95DOCINFO);
		di.lpszDocName = "NavPlan.doc";
		di.lpszOutput = (LPTSTR) NULL;
		di.lpszDataType = (LPTSTR) NULL; // secret new parameters for Win95?
		di.fwType = 0;					 // maybe this should be NULL?

		int startIndex = 1;
		pDC->StartDoc((char *) &di);
		beginPage:
		int newPage = FALSE;
		pDC->StartPage();

		pDC->SetMapMode(MM_TWIPS); 

		// 10 point proportional-pitch font
		font.CreateFont(-200, 0, 0, 0, 400, FALSE, FALSE,
						0, ANSI_CHARSET, OUT_TT_PRECIS,
						CLIP_DEFAULT_PRECIS, PROOF_QUALITY,
						DEFAULT_PITCH | FF_SWISS | TMPF_TRUETYPE, "Ariel");
		CFont *pOldFont = (CFont *) (pDC->SelectObject(&font));
		
 		double pagew = pDC->GetDeviceCaps(HORZRES) / (double) pDC->GetDeviceCaps(LOGPIXELSX);
		double pageh = pDC->GetDeviceCaps(VERTRES) / (double) pDC->GetDeviceCaps(LOGPIXELSY);

		pDC->GetOutputTextMetrics(&tm);
		int nHeight = tm.tmHeight + tm.tmExternalLeading;

		// ready to print

		// find minimum size for columns
		CSize locSize  = pDC->GetTextExtent("From", sizeof("From"));
		CSize idSize   = pDC->GetTextExtent("ID", sizeof("ID"));
		CSize freqSize = pDC->GetTextExtent("Freq", sizeof("Freq"));
		CSize headSize = pDC->GetTextExtent("Course", sizeof("Course"));
		CSize mheadSize = pDC->GetTextExtent("Mag Crs", sizeof("Mag Crs"));
		CSize recipSize = pDC->GetTextExtent("(recip)", sizeof("(recip)"));
		CSize distSize = pDC->GetTextExtent("Distance", sizeof("Distance"));
		CSize tdisSize = pDC->GetTextExtent("Total", sizeof("Total"));
		CSize eteSize = pDC->GetTextExtent("ETE", sizeof("ETE"));
		CSize teteSize = pDC->GetTextExtent("Total", sizeof("Total"));
		CSize fuelSize = pDC->GetTextExtent("Fuel", sizeof("Fuel"));
		CSize tfuelSize = pDC->GetTextExtent("Total", sizeof("Total"));

		CSize nameSize;
		CString name;
		LV_ITEM i;
		i.mask = LVIF_TEXT;
		int dbtype;
		for (int k=0; k<plantable.GetSize(); k++) {
			if (plantable[k].index >= 0)
				dbtype = doc->db[plantable[k].index].type;
			else 
				dbtype = plantable[k].index; // toc, bod, toc/bob
			CString bigName;
			name = readList(k, 0);
			nameSize = pDC->GetTextExtent(name, (name).GetLength());
			if (dbtype == 2) {
				bigName = name + "ILS - ";
				if (nameSize.cx < (pDC->GetTextExtent(bigName, bigName.GetLength())).cx)
					nameSize = pDC->GetTextExtent(bigName, bigName.GetLength());
			}
			if (nameSize.cx > locSize.cx)
				locSize.cx = nameSize.cx;

			if ((dbtype < 4) || (dbtype == 8)) {
				name = readList(k, 1);
				nameSize = pDC->GetTextExtent(name, name.GetLength());
				if (nameSize.cx > idSize.cx)
					idSize.cx = nameSize.cx;
			}

			if ((dbtype < 4) || (dbtype == 10)){
				name = readList(k, 3);
				nameSize = pDC->GetTextExtent(name, name.GetLength());
				if (nameSize.cx > freqSize.cx)
					freqSize.cx = nameSize.cx;
			}

			name = readList(k, 4);
			nameSize = pDC->GetTextExtent(name, name.GetLength());
			if (nameSize.cx > headSize.cx)
				headSize.cx = nameSize.cx;

			name = readList(k, 5);
			nameSize = pDC->GetTextExtent(name, name.GetLength());
			if (nameSize.cx > distSize.cx)
				distSize.cx = nameSize.cx;

			name = readList(k, 6);
			nameSize = pDC->GetTextExtent(name, name.GetLength());
			if (nameSize.cx > tdisSize.cx)
				tdisSize.cx = nameSize.cx;

			name = readList(k, 7);
			nameSize = pDC->GetTextExtent(name, name.GetLength());
			if (nameSize.cx > eteSize.cx)
				eteSize.cx = nameSize.cx;

			name = readList(k, 8);
			nameSize = pDC->GetTextExtent(name, name.GetLength());
			if (nameSize.cx > teteSize.cx)
				teteSize.cx = nameSize.cx;

			name = readList(k, 9);
			nameSize = pDC->GetTextExtent(name, name.GetLength());
			if (nameSize.cx > fuelSize.cx)
				fuelSize.cx = nameSize.cx;

			name = readList(k, 10);
			nameSize = pDC->GetTextExtent(name, name.GetLength());
			if (nameSize.cx > tfuelSize.cx)
				tfuelSize.cx = nameSize.cx;
		}
		// leave space between columns (2 pts, 1 pt on either size of line)
 		locSize.cx   = locSize.cx   + 144; 
		idSize.cx    = idSize.cx    + 144;
		freqSize.cx  = freqSize.cx  + 144;
		headSize.cx  = headSize.cx  + 144;
		mheadSize.cx = mheadSize.cx + 144;
		distSize.cx  = distSize.cx  + 144;
		tdisSize.cx  = tdisSize.cx  + 144;
		eteSize.cx   = eteSize.cx   + 144;
		teteSize.cx  = teteSize.cx  + 144;
		fuelSize.cx  = fuelSize.cx  + 144;
		tfuelSize.cx = tfuelSize.cx + 144;
		int idfreq = max(idSize.cx, freqSize.cx);
		int disttotal = max(distSize.cx, tdisSize.cx);
		int etetotal = max(eteSize.cx, teteSize.cx);
		int fueltotal = max(fuelSize.cx, tfuelSize.cx);
		// resolution remaining
		int excess = (int) pagew * 1440 - (locSize.cx + idfreq + headSize.cx + mheadSize.cx + disttotal + etetotal + fueltotal);

		// do column headings
		int x = 72;
		pDC->TextOut(x, 0, "From");
		x += locSize.cx;
		pDC->TextOut(x, 0, "ID");
		pDC->TextOut(x, -nHeight - 20, "Freq");
		pDC->MoveTo(x, nHeight);
		pDC->LineTo(x+idfreq, nHeight);
		x += idfreq;
		pDC->TextOut(x, 0, "To");
		x += locSize.cx;
		pDC->TextOut(x, 0, "ID");
		pDC->TextOut(x, -nHeight - 20, "Freq");
		pDC->MoveTo(x, nHeight);
		pDC->LineTo(x+idfreq, nHeight);
		x += idfreq;
		pDC->TextOut(x, 0, "Course");
		pDC->TextOut(x, -nHeight - 20, "(recip)");
		x += headSize.cx;
		pDC->TextOut(x, 0, "Mag Crs");
		pDC->TextOut(x, -nHeight - 20, "(recip)");
		x += mheadSize.cx;
		pDC->TextOut(x, 0, "Distance");
		pDC->TextOut(x, -nHeight - 20, "Total");
		pDC->MoveTo(x, nHeight);
		pDC->LineTo(x+disttotal, nHeight);
		x += disttotal;
		pDC->TextOut(x, 0, "ETE");
		pDC->TextOut(x, -nHeight - 20, "Total");
		pDC->MoveTo(x, nHeight);
		pDC->LineTo(x+etetotal, nHeight);
		x += etetotal;
		pDC->TextOut(x, 0, "Fuel");
		pDC->TextOut(x, -nHeight - 20, "Total");
		pDC->MoveTo(x, nHeight);
		pDC->LineTo(x+fueltotal, nHeight);
		x += fueltotal;

		// do data
		int y = -2 * nHeight - 40, dbtypeLast; 
		for (k=startIndex; k<plantable.GetSize(); k++) {
			if (plantable[k].index >= 0)
				dbtype = doc->db[plantable[k].index].type;
			else 
				dbtype = plantable[k].index; // toc, bod, toc/bob
			if (plantable[k-1].index > 0)
				dbtypeLast = doc->db[plantable[k-1].index].type;
			else 
				dbtypeLast = plantable[k-1].index; // toc, bod, toc/bob

			x = 72;
			name = readList(k-1, 0);
			pDC->TextOut(x, y, name);
			x += locSize.cx;

			if ((dbtypeLast < 5) || (dbtypeLast == 10)) {
				name = readList(k-1, 1);
				pDC->TextOut(x, y, name);
				if ((dbtypeLast == 4)||(dbtypeLast == 8)) { // airport or runway runway
					name = readList(k-1, 11); // altitude
					pDC->TextOut(x, y - nHeight, name + "ft");
				} else if (dbtypeLast > 0 ) {
					name = readList(k-1, 3);
					pDC->TextOut(x, y - nHeight, name);
				}
			} else if (dbtypeLast == 9) {
				name = "GPS";
				pDC->TextOut(x, y, name);
				name = readList(k-1, 1);
				pDC->TextOut(x, y - nHeight, name);
			}
			x += idfreq;

			name = readList(k, 0);
			pDC->TextOut(x, y, name);
			x += locSize.cx;

			if ((dbtype < 5) || (dbtype == 10)) {
				name = readList(k, 1);
				pDC->TextOut(x, y, name);
				if ((dbtype == 4)||(dbtype == 8)) { // airport or runway runway
					name = readList(k, 11); // altitude
					pDC->TextOut(x, y - nHeight, name + "ft");
				} else if (dbtype > 0 ) {
					name = readList(k, 3);
					pDC->TextOut(x, y - nHeight, name);
				}
			} else if (dbtype == 9) {
				name = "GPS";
				pDC->TextOut(x, y, name);
				name = readList(k, 1);
				pDC->TextOut(x, y - nHeight, name);
			}
			x += idfreq;
			
			char mbuf[30];
			int magHeading, trueHeading;

			//  calculate true heading
			view->gcDist(plantable[k-1].lat, plantable[k-1].lon, plantable[k].lat, plantable[k].lon);
			trueHeading = (int) view->bearing;
			sprintf(mbuf, "%d", trueHeading);
			name = mbuf;
			name.TrimRight();
			pDC->TextOut(x, y, " " + name);

			// recip heading
			if (trueHeading >= 180) trueHeading -= 180;
			else if (trueHeading < 180) trueHeading += 180;
			sprintf(mbuf, "(%d)", trueHeading);
			name = mbuf;
			name.TrimRight();
			pDC->TextOut(x, y - nHeight, name);
			x += headSize.cx;

			// magnetic heading
			name = readList(k, 4); //  corrected (magnetic + wind) heading
			sscanf((LPCTSTR) name, "%d", &magHeading);
			pDC->TextOut(x, y, " " + name);

			// recip heading
			if (magHeading >= 180) magHeading -= 180;
			else if (magHeading < 180) magHeading += 180;
			sprintf(mbuf, "(%d)", magHeading);
			name = mbuf;
			name.TrimRight();
			pDC->TextOut(x, y - nHeight, name);
			x += mheadSize.cx;

			name = readList(k, 5);	// distance
			pDC->TextOut(x, y, name);
			name = readList(k, 6);	// total distance
			pDC->TextOut(x, y - nHeight, name);
			x += disttotal;

			name = readList(k, 7);	// ete
			pDC->TextOut(x, y, name);
			name = readList(k, 8);	// total ete
			pDC->TextOut(x, y - nHeight, name);
			x += etetotal;

			name = readList(k, 9);	// fuel
			pDC->TextOut(x, y, name);
			name = readList(k, 10);	// total fuel
			pDC->TextOut(x, y - nHeight, name);

			y -= 2*nHeight-20;
 			// do horizontal line
			pDC->MoveTo(0, y);
			x += fueltotal;
			pDC->LineTo(x, y);
			y -= 20;

			// check to see if next item must go on new page
			if ((y < (int) (pageh * -1440)) && (k < plantable.GetSize()-1)) {
				newPage = TRUE;
				startIndex = k+1;
				break;
			}
		}

		// do vertical lines
		y += 20;
		pDC->MoveTo(0, 0);
		pDC->LineTo(0, y);
		x = locSize.cx;
		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += idfreq;
 		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += locSize.cx;
		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += idfreq;
 		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += headSize.cx;
 		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += mheadSize.cx;
 		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += disttotal;
 		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += etetotal;
 		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
		x += fueltotal+72;
 		pDC->MoveTo(x, 0);
		pDC->LineTo(x, y);
 		// do horizontal lines
		pDC->MoveTo(0, 0);
		pDC->LineTo(x, 0);
		pDC->MoveTo(0, -2 * nHeight -20);
		pDC->LineTo(x, -2 * nHeight -20);

		if (newPage) {
			pDC->SelectObject(pOldFont);
			pDC->EndPage();
			goto beginPage;
		}

		// clean up
		pDC->SelectObject(pOldFont);
		pDC->EndPage();
		pDC->EndDoc();
		ReleaseDC(pDC);
		DeleteDC(hDC);		
	}
	GlobalFree(pd.m_pd.hDevMode);
	GlobalFree(pd.m_pd.hDevNames);
}

// disable editing the selected list item
void FlightPlan::OnBeginlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) {
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
		*pResult = 0;
}

// deallocate own memory since window can be closed from view or from here
void FlightPlan::PostNcDestroy() { 		
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	view->stopFlyTo();
	plantable.RemoveAll();
	plantable.FreeExtra();
	delete this; // yes, Al Stevens would not hire me
}

void FlightPlan::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) {
	OnClickList(pNMHDR, pResult);

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	int selected = getSel();
	if (selected == -1) // nothing was selected
		return;

	int index = plantable[selected].index;
	if (index >= 0) { // show details dialog box
		CString details = "";
		doc->formatData(details, index);
		Details d(this);
		d.index = index;
		d.m_text = details;
		d.DoModal();
	}

	*pResult = 0;
}

void FlightPlan::OnAircraft() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	Aircraft d(this);
	d.m_cruisealtitude = cruisealtitude;
	d.m_descentfuel = descentfuel;
	d.m_climbfuel = climbfuel;
	d.m_cruisefuel = cruisefuel;
	d.m_cruisespeed = cruisespeed;
	d.m_climbspeed = climbspeed;
	d.m_descentspeed = descentspeed;
	d.m_climbrate = climbrate;
	d.m_descentrate = descentrate;
	d.m_craft = craftBuffer;
	if (d.DoModal() == IDOK) {
		cruisealtitude = d.m_cruisealtitude;
		descentfuel = d.m_descentfuel;
		climbfuel = d.m_climbfuel;
		cruisefuel = d.m_cruisefuel;
		cruisespeed = d.m_cruisespeed;
		climbspeed = d.m_climbspeed;
		descentspeed = d.m_descentspeed;
		climbrate = d.m_climbrate;
		descentrate = d.m_descentrate;
		strcpy(craftBuffer, (LPCTSTR) d.m_craft);
		update();
		view->Invalidate(TRUE);
	}	
}

void FlightPlan::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) {
	checkButtons();

	*pResult = 0;
}

void FlightPlan::OnPaint() {
	if (IsIconic())	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
//HCURSOR FlightPlan::OnQueryDragIcon() {
//	return (HCURSOR) m_hIcon;
//}

void FlightPlan::OnWind() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	Wind d(this);
	d.noselection = FALSE;
	windStruct w;

	int selected = getSel();
	if (selected == -1) // nothing was selected
		d.noselection = TRUE;
	if (selected >= 0) {
		int t = plantable[selected].index; // toc/bod are not valid selections
		if ((t == -1) || (t == -2) | (t == -3))
			d.noselection = TRUE;
	}

	if (d.noselection == FALSE) { // initialize to current values
		if (view->windtable.Lookup(plantable[selected].index, w)) {
			d.m_speed = w.speed;
			d.m_direction = w.direction;
		}
	} else { // if all values the same, use that
		if (view->windtable.Lookup(plantable[0].index, w)) {
			int same = TRUE;
			int samespeed = w.speed;
			int samedirection = w.direction;
			for (int j=1; j<plantable.GetSize(); j++) {
				if (plantable[j].index < 0)
					continue;
				if (view->windtable.Lookup(plantable[j].index, w)) {
					if ((samespeed != w.speed) || (samedirection != w.direction)) {
						same = FALSE;
						break;
					}
				} else { 
					same = FALSE;
					break;
				}
			}
			if (same) {
				d.m_speed = w.speed;
				d.m_direction = w.direction;
			}
		}
	}

	if (d.DoModal() == IDOK) {
		if (d.m_global) { // update all entries
			for (int j=0; j<plantable.GetSize(); j++) {
				if (plantable[j].index < 0)
					continue;
				w.speed = d.m_speed;
				w.direction = d.m_direction;
				view->windtable.SetAt(plantable[j].index, w);
			}
		} else {
			w.speed = d.m_speed;
			w.direction = d.m_direction;
			view->windtable.SetAt(plantable[selected].index, w);
		}
		excessWind = FALSE; 
		update();
		view->Invalidate(TRUE);
	}
}

CString FlightPlan::readList(int row, int column) {
	char buf[100];
	LV_ITEM pItem;
	pItem.mask = LVIF_TEXT;
	pItem.pszText = buf;
	pItem.cchTextMax = 100;
	pItem.iItem = row;
	pItem.iSubItem = column;
	m_list.GetItem(&pItem);
	CString r;
	r = buf;
	r.TrimLeft();
	r.TrimRight();
	return (r);
}

void FlightPlan::OnFilePrint() {
	OnPrint();	
}

void FlightPlan::OnUpdateFilePrint(CCmdUI* pCmdUI) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	pCmdUI->Enable(view->plancount > 1); // works, but doesn't grey out
}

void FlightPlan::OnFileNew() {
	OnClear();
}

void FlightPlan::OnFileOpen() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	static char BASED_CODE szFilter[] = "Plan Files (*.pln)|*.pln|All Files (*.*)|*.*||";
	CFileDialog d(TRUE, ".pln", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;

	// use last directory, if available
	char currentDir[260], buffer[300];
	GetCurrentDirectory(260, currentDir);
	GetPrivateProfileString("Plan", "lastPlanDirectory", "", buffer, 256, "nav.ini");
	if (0 != strcmp(buffer, "")) {
		SetCurrentDirectory(buffer);
		d.m_ofn.lpstrInitialDir = buffer;
	}

	if (d.DoModal() != IDOK) {
		SetCurrentDirectory(currentDir);
		return;
	}
	CString filename = d.GetPathName();

	// save directory for future default
	CString dirname = filename;
	int index = dirname.ReverseFind('\\');
	dirname = dirname.Left(index);
	WritePrivateProfileString("Plan", "lastPlanDirectory", dirname, "nav.ini");

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}

	int version, items;
    fread(&version, sizeof(version), 1, fp);
	if (version != PLANVERSION) {
		fclose(fp);
		char buf[256];
		sprintf(buf, "%s does not appear to be a valid Nav plan\nMaybe you need to import the plan from another version", filename);
		AfxMessageBox(buf);
		return;
	}
	openPlanName = filename;
	alternateID = CString("");
	alternateName = CString("");

    fread(&items, sizeof(items), 1, fp);

	tPlanRec planRec;
	CArray<tPlanRec, tPlanRec> it;
	char sourceFile[256];
	CArray<CString, CString> sourceFiles;
	it.SetSize(0,25);	
	sourceFiles.SetSize(0,25);	
	for (int p=0; p<items; p++) {
        fread(&planRec, sizeof(tPlanRec), 1, fp);
		it.Add(planRec);
        fread(sourceFile, 256, 1, fp);
		sourceFiles.Add(CString(sourceFile));
	}
	fclose(fp);

	view->plancount = 0;
	int oldCount=doc->dbCount;
	for (int i=0; i<items; i++) {
		if (-1 == (index=doc->findIndex(it[i].type, it[i].lat, it[i].lon,
						(int)it[i].alt, it[i].freq, it[i].id, it[i].name,
						it[i].magdev, it[i].approach, it[i].range, 50))) { 
			// insert navaid into database
			doc->addDB(it[i].type, it[i].lat, it[i].lon, (int)(it[i].alt*0.3048), // convert to meters
				it[i].freq, it[i].id, it[i].name, it[i].magdev, it[i].approach, 
				it[i].range, sourceFiles[i].GetBuffer(1));
			sourceFiles[i].ReleaseBuffer();
			index = doc->dbCount - 1;
		} 
		view->plan[view->plancount] = index;
		view->plancount += 1;
	}
	if (doc->dbCount > oldCount)
		doc->Index();

	view->abort = TRUE;
	view->Invalidate(TRUE); 
	update();
	it.RemoveAll();
	it.FreeExtra();
	sourceFiles.RemoveAll();
	sourceFiles.FreeExtra();
}

void FlightPlan::OnFileImportOldNav() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	static char BASED_CODE szFilter[] = "Plan Files (*.pln)|*.pln|All Files (*.*)|*.*||";
	CFileDialog d(TRUE, ".pln", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;
	if (d.DoModal() != IDOK)
		return;
	CString filename = d.GetPathName();

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}
	alternateID = CString("");
	alternateName = CString("");

	int dbindex;
	double lat, lon;
	fscanf(fp, "%d\n", &(view->plancount));		// scan through file looking for start of file list
	for (int k=0; k<view->plancount; k++) {
		fscanf(fp, "%d", &dbindex);
		if (dbindex = -1)		// GPS indicator
			fscanf(fp, "%lf %lf", &lat, &lon);
	}

	int c;
	FILE *fp2 = fopen(view->orgDir + "\\OpenedPlan", "w");
	while ((c=fgetc(fp)) != EOF)
		fputc(c, fp2);
	fclose(fp);
	fclose(fp2);
	doc->OnFileLoad("OpenedPlan");
	DeleteFile(view->orgDir + "\\" + "OpenedPlan");

	fp = fopen(filename, "r");
	fscanf(fp, "%d\n", &(view->plancount));		// second pass through file to actually load data
	for (k=0; k<view->plancount; k++) {
		fscanf(fp, "%d", &dbindex);
		if (dbindex != -1)		// GPS indicator
			view->plan[k] = dbindex;
		else {
			fscanf(fp, "%lf %lf", &lat, &lon);
			doc->OnMapInsertgpsfix(lat, lon);
			view->plan[k] = doc->dbCount - 1;
		}
	}
	fclose(fp);
}

// portable (but version specific) binary format until XML parsers are reasonable
void FlightPlan::OnFileSave() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	CString planName = openPlanName;
	// suggest a file name if plan is between two airports
	int plancount = plantable.GetSize();
	if (openPlanName == "") {
		if ((doc->db[plantable[0].index].type == 4)&&(doc->db[plantable[plancount-1].index].type == 4)) {
			dbRec dep, arr;
			memcpy(&dep, &doc->db[plantable[0].index], sizeof(dbRec));
			memcpy(&arr, &doc->db[plantable[plancount-1].index], sizeof(dbRec));
			CString depICAO, arrICAO;	
			if ((doc->lookUpAirport(depICAO, dep.lat, dep.lon))  && (doc->lookUpAirport(arrICAO, arr.lat, arr.lon)) ) 
				planName = depICAO + "-" + arrICAO;
		}
	}

	static char BASED_CODE szFilter[] = "Plan Files (*.pln)|*.pln|All Files (*.*)|*.*||";
	CFileDialog d(FALSE, ".pln", planName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;

	// use last directory, if available
	char currentDir[260], buffer[300];
	GetCurrentDirectory(260, currentDir);
	GetPrivateProfileString("Plan", "lastPlanDirectory", "", buffer, 256, "nav.ini");
	if (0 != strcmp(buffer, "")) {
		SetCurrentDirectory(buffer);
		d.m_ofn.lpstrInitialDir = buffer;
	}

	if (d.DoModal() != IDOK) {
		SetCurrentDirectory(currentDir);
		return;
	}
	CString filename = d.GetPathName();

	// save directory for future default
	CString dirname = filename;
	int index = dirname.ReverseFind('\\');
	dirname = dirname.Left(index);
	WritePrivateProfileString("Plan", "lastPlanDirectory", dirname, "nav.ini");

	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		SetCurrentDirectory(currentDir);
		return;
	}

	// build table without TOC/BOD so we can ignore them
	CArray<planRec, planRec> exportTable;
	exportTable.SetSize(0,50);	
	for (int k=0; k<plantable.GetSize(); k++)
		if (plantable[k].index >= 0)
			exportTable.Add(plantable[k]);
	plancount = exportTable.GetSize();

	int version = PLANVERSION;
    fwrite(&version, sizeof(version), 1, fp);
    fwrite(&plancount, sizeof(plancount), 1, fp);

	tPlanRec pr;
	dbRec dbr;
	char sourceFile[256];
	for (k=0; k<plancount; k++) {
		dbr = doc->db[exportTable[k].index]; 
		pr.type = dbr.type;
		pr.lat = exportTable[k].lat;
		pr.lon = exportTable[k].lon;
		pr.alt = exportTable[k].alt; // note: in feet
		pr.freq = dbr.freq;
		strncpy(pr.id, dbr.id, 6);
		strncpy(pr.name, dbr.name, 31);
		pr.magdev = dbr.magdev;
		pr.approach = dbr.approach;
		pr.range = dbr.range;
		pr.windVel = exportTable[k].windVel;
		pr.windheading = exportTable[k].windheading;
		pr.heading = exportTable[k].heading;
		pr.dist = exportTable[k].dist;
		pr.Tdist = exportTable[k].Tdist;
		pr.ete = exportTable[k].ete;
		pr.Tete = exportTable[k].Tete;
		pr.fuel = exportTable[k].fuel;
		pr.Tfuel = exportTable[k].Tfuel;
		pr.correctedheading = exportTable[k].correctedheading;
		pr.correctedvelocity = exportTable[k].correctedvelocity;
		fwrite(&pr, sizeof(tPlanRec), 1, fp);
		if (9 != dbr.type)
			strncpy(sourceFile, doc->dbFiles[dbr.file], 256);
		else
			strcpy(sourceFile, "no file");
		fwrite(sourceFile, 256, 1, fp);
	}

	fclose(fp);
	exportTable.RemoveAll();
	exportTable.FreeExtra();
	SetCurrentDirectory(currentDir);
}

void FlightPlan::OnFileExportACSGPS() {
	int plancount = plantable.GetSize();

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	CString defaultName = "";
	if (plancount > 1) {
		CString id1 = readList(0, 1);
		CString id2 = readList(plancount-1, 1);
		if ((id1 != "") && (id2 != "")) 
			defaultName = id1 + "-" + id2;
	}

	static char BASED_CODE szFilter[] = "ACS-GPS Files (*.prg)|*.prg|All Files (*.*)|*.*||";
	CFileDialog d(FALSE, ".prg", defaultName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;

	// use last directory, if available
	char buffer[300];
	GetPrivateProfileString("Plan", "lastACSGPSdirectory", "", buffer, 256, "nav.ini");
	char currentDir[260];
	GetCurrentDirectory(260, currentDir);
	if (0 != strcmp(buffer, "")) {
		SetCurrentDirectory(buffer);
		d.m_ofn.lpstrInitialDir = buffer;
	}

	if (d.DoModal() != IDOK)
		return;

	CString filename = d.GetPathName();
	CString dirname = d.GetPathName();
	int index = dirname.ReverseFind('\\');
	dirname = dirname.Left(index);
	WritePrivateProfileString("Plan", "lastACSGPSdirectory", dirname, "nav.ini");
	SetCurrentDirectory(currentDir);

	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}

	CString name, type, id;
	double lat, lon;

	for (int k=0; k<plantable.GetSize(); k++) {
		type = readList(k, 2);
		id = readList(k, 1);
		if (plantable[k].index < 0) {
			fprintf(fp, "%s\n", type);
		}
		else
			fprintf(fp, "%s\n", id);

		name = readList(k, 0);
		if (plantable[k].index < 0) 
			fprintf(fp, "%s %s\n", type, id);
		else if ((type == "VOR")||(type == "NDB"))
			fprintf(fp, "%s %s\n", name, readList(k, 3));
		else
			fprintf(fp, "%s\n", name);

		lon = plantable[k].lon;
		if (lon < 0)
			fprintf(fp, "W ");
		else
			fprintf(fp, "E ");
		lon = fabs(lon);				// extract degrees
		fprintf(fp, "%d* ", (int) lon);
		lon = (lon - (int) lon) * 60;	// extract minutes.seconds
		fprintf(fp, "%5.2fm\n", lon);

		lat = plantable[k].lat;
		if (lat < 0)
			fprintf(fp, "S ");
		else
			fprintf(fp, "N ");
		lat = fabs(lat);				// extract degrees
		fprintf(fp, "%d* ", (int) lat);
		lat = (lat - (int) lat) * 60;	// extract minutes.seconds
		fprintf(fp, "%5.2fm\n", lat);

		fprintf(fp, "\n");
	}
	fclose(fp);
}

void FlightPlan::OnFileExportSquawkBox() {
	int plancount = plantable.GetSize();
	if (plancount < 2) {
		AfxMessageBox("Need at least 2 items in plan to export");
		return;
	}

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	int notAirports = TRUE;
	if (plantable[0].index >= 0)
		if (plantable[plancount-1].index >= 0)
			if (doc->db[plantable[0].index].type == 4)
				if (doc->db[plantable[plancount-1].index].type == 4)
					notAirports = FALSE;
	if (notAirports) {
		AfxMessageBox("Export failed.\nThe first and last items in the plan must be airports");
		return;
	}

	static char BASED_CODE szFilter[] = "SBPlan Files (*.sbp)|*.sbp|All Files (*.*)|*.*||";
	CFileDialog d(FALSE, ".sbp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;
	if (d.DoModal() != IDOK)
		return;
	CString filename = d.GetPathName();

	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}

	CString name, type, icao;

	name = readList(0, 0);
	icao = readList(0, 1);
	if (icao != "");
		name = icao;
	fprintf(fp, "%s:", name.GetBuffer(1));

	int index = plantable.GetSize()-1;
	name = readList(index, 0);
	icao = readList(index, 1);
	if (icao != "");
		name = icao;
	fprintf(fp, "%s:", name.GetBuffer(1));
	fprintf(fp, "%d\n", cruisespeed);

	for (int k=0; k<plantable.GetSize(); k++) {
		type = readList(k, 2);
		name = readList(k, 1);
		if (type == "VOR")
			fprintf(fp, "V:");
		else if (type == "NDB")
			fprintf(fp, "N:");
		else if (type == "Waypoint")
			fprintf(fp, "I:");
		else if (type == "Airport") {
			if ((k == 0) && (readList(k, 1) != ""))
				continue;
			if ((k == plantable.GetSize()-1) && (readList(k, 1) != ""))
				continue;
			fprintf(fp, "A:");
		} else 
			fprintf(fp, "X:");
		if (type != "Airport") {
			fprintf(fp, "%s:", name.GetBuffer(1));
		} else {
			name = readList(k, 0);
			icao = readList(k, 1);
			if (icao != "");
				name = icao;
			fprintf(fp, "%s:", name.GetBuffer(0));
		}
		fprintf(fp, "%lf:", plantable[k].lat);
		fprintf(fp, "%lf:", plantable[k].lon);
		if (k==0) {
			fprintf(fp, "%d:", cruisespeed);
			fprintf(fp, "%0.0lf:", plantable[k].alt);
			fprintf(fp, "%0.0lf:", plantable[k+1].alt);
			fprintf(fp, "%0.0lf\n", (plantable[k+1].heading != 0.0)?plantable[k+1].heading:360);
		} else {
			fprintf(fp, "0:");
			if ((int)plantable[k].alt != (int)plantable[k-1].alt)
				fprintf(fp, "%0.0lf:", plantable[k].alt);
			else
				fprintf(fp, "0:");
			if (k<plantable.GetSize()-1) {
				if ((int)plantable[k+1].alt != (int)plantable[k].alt)
					fprintf(fp, "%0.0lf:", plantable[k+1].alt);
				else
					fprintf(fp, "0:");
				if (plantable[k+1].heading != plantable[k].heading)
					fprintf(fp, "%0.0lf\n", (plantable[k+1].heading != 0.0)?plantable[k+1].heading:360);
				else
					fprintf(fp, "0\n");
			} else {
				fprintf(fp, "0:");
				fprintf(fp, "0\n");
			}
		}
	}
	fprintf(fp, "\n");
	fclose(fp);
}

void FlightPlan::ExportFile(CString filename) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}

	// find minimum size for columns
	int locSize   = sizeof("Location");
	int idSize	  = sizeof("ID");
	int freqSize  = sizeof("Freq");
	int headSize  = sizeof("Course");
	int mheadSize = sizeof("Mag Crs");
	int recipSize = sizeof("(recip)");
	int distSize  = sizeof("Distance");
	int tdisSize  = sizeof("Total");
	int eteSize   = sizeof("ETE");
	int teteSize  = sizeof("Total");
	int fuelSize  = sizeof("Fuel");
	int tfuelSize = sizeof("Total");

	// print title
	if (doc->db[plantable[0].index].type == 4) {
		if (doc->db[plantable[plantable.GetSize()-1].index].type == 4) {
			fprintf(fp, "%s to %s\n\n", doc->db[plantable[0].index].name, doc->db[plantable[plantable.GetSize()-1].index].name);
		}
	}
	
	int nameSize;
	CString name;
	int dbtype, dbtypeLast;
	for (int k=0; k<plantable.GetSize(); k++) {
		if (plantable[k].index >= 0)
			dbtype = doc->db[plantable[k].index].type;
		else 
			dbtype = plantable[k].index; // toc, bod, toc/bob
		CString bigName;
		name = readList(k, 0);
		name.TrimRight();
		nameSize = name.GetLength() + 1;
		if (nameSize > locSize)
			locSize = nameSize;

		name = readList(k, 1);
		nameSize = name.GetLength() + 1;
		if (nameSize > idSize)
			idSize = nameSize;

		name = readList(k, 3);
		nameSize = name.GetLength() + 1;
		if (nameSize > freqSize)
			freqSize = nameSize;

		name = readList(k, 4);
		nameSize = name.GetLength() +1;
		if (nameSize > headSize)
			headSize = nameSize;

		name = readList(k, 5);
		nameSize = name.GetLength() + 1;
		if (nameSize > distSize)
			distSize = nameSize;

		name = readList(k, 6);
		nameSize = name.GetLength() + 1;
		if (nameSize > tdisSize)
			tdisSize = nameSize;

		name = readList(k, 7);
		nameSize = name.GetLength() + 1;
		if (nameSize > eteSize)
			eteSize = nameSize;

		name = readList(k, 8);
		nameSize = name.GetLength() + 1;
		if (nameSize > teteSize)
			teteSize = nameSize;

		name = readList(k, 9);
		nameSize = name.GetLength() + 1;
		if (nameSize > fuelSize)
			fuelSize = nameSize;

		name = readList(k, 10);
		nameSize = name.GetLength() + 1;
		if (nameSize > tfuelSize)
			tfuelSize = nameSize;
	}
	int disttotal = max(distSize, tdisSize);
	int etetotal =  max(eteSize,  teteSize);
	int fueltotal = max(fuelSize, tfuelSize);

	// do column headings
	pspace(locSize-1, fp, '-');   fprintf(fp, " ");
	pspace(idSize-1, fp, '-');    fprintf(fp, " ");
	pspace(freqSize-1, fp, '-');  fprintf(fp, " ");
	pspace(mheadSize-1, fp, '-'); fprintf(fp, " ");
	pspace(disttotal-1, fp, '-'); fprintf(fp, " ");
	pspace(etetotal-1, fp, '-');  fprintf(fp, " ");
	pspace(fueltotal-1, fp, '-'); fprintf(fp, " ");
	fprintf(fp, "\n");
	pspace(locSize -   fprintf(fp, "From"), fp);
	pspace(idSize -    fprintf(fp, "ID"), fp);
	pspace(freqSize -  fprintf(fp, "Freq"), fp);
	pspace(mheadSize - fprintf(fp, "Course"), fp);
	pspace(disttotal - fprintf(fp, "Distance"), fp);
	pspace(etetotal -  fprintf(fp, "ETE"), fp);
	pspace(fueltotal - fprintf(fp, "Fuel"), fp);
	fprintf(fp, "\n");
	pspace(locSize -   fprintf(fp, "To"), fp);
	pspace(idSize -    fprintf(fp, "ID"), fp);
	pspace(freqSize -  fprintf(fp, "Freq"), fp);
	pspace(mheadSize - fprintf(fp, "Mag-Crs"), fp);
	pspace(disttotal - fprintf(fp, "Total"), fp);
	pspace(etetotal -  fprintf(fp, "Total"), fp);
	pspace(fueltotal - fprintf(fp, "Total"), fp);
	fprintf(fp, "\n");
	pspace(locSize-1, fp, '-');   fprintf(fp, " ");
	pspace(idSize-1, fp, '-');    fprintf(fp, " ");
	pspace(freqSize-1, fp, '-');  fprintf(fp, " ");
	pspace(mheadSize-1, fp, '-'); fprintf(fp, " ");
	pspace(disttotal-1, fp, '-'); fprintf(fp, " ");
	pspace(etetotal-1, fp, '-');  fprintf(fp, " ");
	pspace(fueltotal-1, fp, '-'); fprintf(fp, " ");
	fprintf(fp, "\n");

	// do data
	for (k=1; k<plantable.GetSize(); k++) {
		if (plantable[k].index >= 0)
			dbtype = doc->db[plantable[k].index].type;
		else 
			dbtype = plantable[k].index; // toc, bod, toc/bod
		if (plantable[k-1].index > 0)
			dbtypeLast = doc->db[plantable[k-1].index].type;
		else 
			dbtypeLast = plantable[k-1].index; // toc, bod, toc/bod

		name = readList(k-1, 0);
		pspace(locSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		if ((dbtypeLast <= 4) || (dbtypeLast == 10)) {
			name = readList(k-1, 1);
			pspace(idSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);
		} else if (dbtypeLast == 9) {
			name = "GPS";
			pspace(idSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);
		}

		name = readList(k-1, 3);
		pspace(freqSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		//  calculate true heading
		char mbuf[30];
		int magHeading, trueHeading;
		view->gcDist(plantable[k-1].lat, plantable[k-1].lon, plantable[k].lat, plantable[k].lon);
		trueHeading = (int) view->bearing;
		sprintf(mbuf, "%d", trueHeading);
		name = mbuf;
		name.TrimRight();
		pspace(mheadSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		name = readList(k, 5);	// distance
		pspace(disttotal - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		name = readList(k, 7);	// ETE
		pspace(etetotal - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		name = readList(k, 9);	// fuel
		pspace(fueltotal - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		fprintf(fp, "\n"); // --------------------------

		name = readList(k, 0);
		pspace(locSize -   fprintf(fp, "%s", name.GetBuffer(1)), fp);

		if ((dbtype <= 4) || (dbtype == 10)) {
			name = readList(k, 1);
			pspace(idSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);
		} else if (dbtype == 9) {
			name = "GPS";
			pspace(idSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);
		}

		name = readList(k, 3);
		pspace(freqSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		// magnetic heading
		name = readList(k, 4); //  corrected (magnetic + wind) heading
		sscanf((LPCTSTR) name, "%d", &magHeading);
		pspace(mheadSize - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		name = readList(k, 6);	// total distance
		pspace(disttotal -   fprintf(fp, "%s", name.GetBuffer(1)), fp);

		name = readList(k, 8);	// total ETE
		pspace(etetotal - fprintf(fp, "%s", name.GetBuffer(1)), fp);

		name = readList(k, 10);	// total fuel
		pspace(fueltotal - fprintf(fp, "%s", name.GetBuffer(1)), fp);
		fprintf(fp, "\n"); // --------------------------------------

		pspace(locSize-1, fp, '-');   fprintf(fp, " ");
		pspace(idSize-1, fp, '-');    fprintf(fp, " ");
		pspace(freqSize-1, fp, '-');  fprintf(fp, " ");
		pspace(mheadSize-1, fp, '-'); fprintf(fp, " ");
		pspace(disttotal-1, fp, '-'); fprintf(fp, " ");
		pspace(etetotal-1, fp, '-');  fprintf(fp, " ");
		pspace(fueltotal-1, fp, '-'); fprintf(fp, " ");
		fprintf(fp, "\n");
	}

	fprintf(fp, "\n");
	fclose(fp);
}

void FlightPlan::OnFileExport() {
	if (plantable.GetSize() < 2) {
		AfxMessageBox("Need at least 2 items in plan to export");
		return;
	}
	static char BASED_CODE szFilter[] = "Text Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog d(FALSE, ".pln", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;
	if (d.DoModal() != IDOK)
		return;
	CString filename = d.GetPathName();
	ExportFile(filename);
}

void FlightPlan::pspace(int s, FILE *fp, char c) {
	if (s<0) 
		return;
	for (int i=0; i<s; i++)
		fprintf(fp, "%c", c);
}

void FlightPlan::OnFlytoplan() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	int selected = getSel();
	if (selected == -1)
		return;
	view->FlyToWaypoint(selected + 1);
}

struct  tplanHeader {
	int Id;      		// 135040; - valid id
	int ListPtr;      	// absolute pointer to bgl list table
};

void FlightPlan::OnFileExportSuperFlightPlanner() {	
	int plancount = plantable.GetSize();
	if (plancount < 2) {
		AfxMessageBox("Need at least 2 items in plan to export");
		return;
	}

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	static char BASED_CODE szFilter[] = "SFP Plan Files (*.pln)|*.pln|All Files (*.*)|*.*||";
	CFileDialog d(FALSE, ".pln", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;
	if (d.DoModal() != IDOK)
		return;
	CString filename = d.GetPathName();

	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}

	int shrink = 0;
	CString name;
	tPlanRec planRec;
	tplanHeader planHeader;
	planHeader.Id = 135040;
    planHeader.ListPtr = sizeof(tplanHeader) + sizeof(tPlanRec)*plancount;
    fwrite(&planHeader, sizeof(tplanHeader), 1, fp);

    for (int i=0; i<plancount; i++) {
		memset(&planRec, 0x00, sizeof(tPlanRec));

		if (plantable[i].index < 0) { // TOC, BOD, or both
			// skip them and let SFP regenerate them
			shrink++;
			continue;
		} 
		// navaid database data
		if (doc->db[plantable[i].index].type > 10) // not currently supported by SFP, so use GPS fix
			planRec.type = 9;
		else
			planRec.type = doc->db[plantable[i].index].type;
		planRec.freq = doc->db[plantable[i].index].freq;
		memcpy(planRec.id, &doc->db[plantable[i].index].id, 6);
		memcpy(planRec.name, &doc->db[plantable[i].index].name, 31);
		planRec.magdev = doc->db[plantable[i].index].magdev;
		planRec.approach = doc->db[plantable[i].index].approach;
		planRec.range = doc->db[plantable[i].index].range;
		planRec.lat = plantable[i].lat;
		planRec.lon = plantable[i].lon;
		planRec.alt = plantable[i].alt;
		// flight plan data
		planRec.windVel = plantable[i].windVel;
		planRec.windheading = plantable[i].windheading;
		planRec.heading = plantable[i].heading;
		planRec.dist = plantable[i].dist;
		planRec.Tdist = plantable[i].Tdist;
		planRec.ete = plantable[i].ete;
		planRec.Tete = plantable[i].Tete;
		planRec.fuel = plantable[i].fuel;
		planRec.Tfuel = plantable[i].Tfuel;
		planRec.correctedheading = plantable[i].correctedheading;
		planRec.correctedvelocity = plantable[i].correctedvelocity;
        fwrite(&planRec, sizeof(tPlanRec), 1, fp);
    }

	if (shrink > 0) {
		fseek(fp, 0, 0);
		planHeader.ListPtr = sizeof(tplanHeader) + sizeof(tPlanRec)*(plancount-shrink);
		fwrite(&planHeader, sizeof(tplanHeader), 1, fp);
	}
	fclose(fp);
}

void FlightPlan::OnFileImportSuperFlightPlanner() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	static char BASED_CODE szFilter[] = "SFP Plan Files (*.pln)|*.pln|All Files (*.*)|*.*||";
	CFileDialog d(TRUE, ".pln", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;
	if (d.DoModal() != IDOK)
		return;
	CString filename = d.GetPathName();

	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		char buf[256];
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}
	alternateID = CString("");
	alternateName = CString("");

	tPlanRec planRec;
	tplanHeader planHeader;
	planHeader.Id = 135040;
    fread(&planHeader, sizeof(tplanHeader), 1, fp);
	if (planHeader.Id != 135040) {
		fclose(fp);
		char buf[256];
		sprintf(buf, "%s does not appear to be a valid SFP plan", filename);
		AfxMessageBox(buf);
		return;
	}

	CArray<tPlanRec, tPlanRec> it;
	it.SetSize(0,50);	
	for (int p=sizeof(tplanHeader); p<planHeader.ListPtr; p+=sizeof(tPlanRec)) {
        fread(&planRec, sizeof(tPlanRec), 1, fp);
		if (planRec.type > 10) // unsupported type
			continue;
		it.Add(planRec);
	}
	fclose(fp);

	view->plancount = 0;
	int index, oldCount=doc->dbCount;
	for (int i=0; i<it.GetSize(); i++) {
		if (-1 == (index=doc->findIndex(it[i].type, it[i].lat, it[i].lon,
						(int)it[i].alt, it[i].freq, it[i].id, it[i].name,
						it[i].magdev, it[i].approach, it[i].range))) { 
			// insert navaid into database
			doc->addDB(it[i].type, it[i].lat, it[i].lon,
							(int)it[i].alt, it[i].freq, it[i].id, it[i].name,
							it[i].magdev, it[i].approach, it[i].range, "unknown");
			index = doc->dbCount - 1;
		} 
		view->plan[view->plancount] = index;
		view->plancount += 1;
	}
	if (doc->dbCount > oldCount)
		doc->Index();

	view->abort = TRUE;
	view->Invalidate(TRUE);
	update();
	it.RemoveAll();
	it.FreeExtra();
}

void FlightPlan::OnFileExportRadarContact() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	// build table without TOC/BOD so we can ignore them
	CArray<planRec, planRec> exportTable;
	exportTable.SetSize(0,50);	
	planRec r;
	for (int k=0; k<plantable.GetSize(); k++) {
		if (plantable[k].index >= 0) {
			// check to see if we need to replace a runway with an airport
			if (( (k==0) || (k==plantable.GetSize()-1) ) && (doc->db[plantable[k].index].type == 8)) {
				int apt = doc->findNearestNavaid(doc->db[plantable[k].index].lat, doc->db[plantable[k].index].lon, 4);
				memcpy(&r, &plantable[k], sizeof(dbRec));
				r.index = apt;
				r.alt = doc->db[apt].alt;
				r.lat = doc->db[apt].lat;
				r.lon = doc->db[apt].lon;
				exportTable.Add(r);
			} else
				exportTable.Add(plantable[k]);
		}
	}

	int plancount = exportTable.GetSize();

	if (plancount < 2) {
		AfxMessageBox("Need at least 2 items in plan to export");
		return;
	}
	
	int notAirports = TRUE;
	if (doc->db[exportTable[0].index].type == 4)
		if (doc->db[exportTable[plancount-1].index].type == 4)
			notAirports = FALSE;
	if (notAirports) {
		AfxMessageBox("Export failed.\nThe first and last items in the plan must be airports");
		return;
	}

	char buf[256];
	dbRec dep, arr, alt;
	int altFound = FALSE;
	memcpy(&dep, &doc->db[exportTable[0].index], sizeof(dbRec));
	memcpy(&arr, &doc->db[exportTable[plancount-1].index], sizeof(dbRec));
	CString defaultName, depICAO, arrICAO, depName, arrName, altICAO;
	depName = CString(dep.name);
	depName.TrimRight();
	arrName = CString(arr.name);
	arrName.TrimRight();

	// build default file name
	if (doc->lookUpAirport(depICAO, dep.lat, dep.lon)) 
		defaultName = depICAO + "-";
	else {
		sprintf(buf, "Export failed\n, No ICAO name found for %s", depName);
		AfxMessageBox(buf);
		return;
	}
	if (doc->lookUpAirport(arrICAO, arr.lat, arr.lon)) 
		defaultName = defaultName + arrICAO;
	else {
		sprintf(buf, "Export failed\n, No ICAO name found for %s", arrName);
		AfxMessageBox(buf);
		return;
	}

	static char BASED_CODE szFilter[] = "Radar Contact Plan Files (*.apl)|*.apl|All Files (*.*)|*.*||";
	CFileDialog d(FALSE, ".apl", defaultName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;

	// use last directory, if available
	char currentDir[260], buffer[300];
	GetCurrentDirectory(260, currentDir);
	GetPrivateProfileString("Plan", "lastRC99directory", "", buffer, 256, "nav.ini");
	if (0 != strcmp(buffer, "")) {
		SetCurrentDirectory(buffer);
		d.m_ofn.lpstrInitialDir = buffer;
	}

	if (d.DoModal() != IDOK) {
		SetCurrentDirectory(currentDir);
		return;
	}
	CString filename = d.GetPathName();

	// save directory for future default
	CString dirname = filename;
	int index = dirname.ReverseFind('\\');
	dirname = dirname.Left(index);
	WritePrivateProfileString("Plan", "lastRC99directory", dirname, "nav.ini");

	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		return;
	}

	CWaitCursor wait;	// display wait cursor
	main->SetStatusBar("Generating file for RC99 export...");

	fprintf(fp, "; ----------------------------------------------------------------------\n");
	CTime theTime = CTime::GetCurrentTime();
	CString timeString = theTime.Format("%A %B %d, %Y at %H:%M:%S");
	fprintf(fp, "; Generated by Nav on %s\n", timeString);
	fprintf(fp, "; ----------------------------------------------------------------------\n");
	fprintf(fp, "\n");

	fprintf(fp, "ARRIVAL_ID = \"%s\"\n", arrICAO);

	int runwaysIndexList[50];
/*  alternate ID has been commented out for speed reasons (and because RC99 doesn't use it yet)
	//optional...
		// ALTERNATE_ID = "PHNG"
	// need to find an alternate arrival airport
	int found = FALSE, foundApt, bestApt, foundRW, bestRW = 0, goodRWs, bestLen = 0, goodLen;
	for (int i=0; i<doc->airportIndex.GetSize(); i++) {
		double d = view->gcDist(doc->db[doc->airportIndex[i]].lat, doc->db[doc->airportIndex[i]].lon, arr.lat, arr.lon)/1852; 
		if (d > 50) // should be not more than 50 miles away from primary arrival
			continue;
		if (d < 20) // should be at least 20 miles away from primary arrival
			continue;
		foundApt = doc->airportIndex[i];
		// try to find one with a lot of (paved) runways to handle the heavys
		foundRW = doc->lookUpRunways(foundApt, runwaysIndexList);
		goodRWs = 0;
		goodLen = 0;
	    for (int r=0; r<foundRW; r++) {
			int rwLength = (int) doc->db[runwaysIndexList[r]].range;
			int rwWidth = (int) doc->db[runwaysIndexList[r]].magdev;
			int rwType = (int) doc->db[runwaysIndexList[r]].freq; // Type1=concrete Type2=asphalt
			if (((rwType==1)||(rwType==2))&&(rwWidth>=50)&&(rwLength>=2000)) {
				goodRWs += 1;
				if (rwLength > goodLen)
					goodLen = rwLength;
			}
		}

		if ((goodRWs > bestRW) || ((goodLen>bestLen) && (goodRWs>=bestRW))){
			bestRW = foundRW;
			bestApt = foundApt;
			bestLen = goodLen;
			found = TRUE;
		}
	}
	if (found) {
		found = found | (doc->lookUpAirport(altICAO, doc->db[bestApt].lat, doc->db[bestApt].lon));
		if (found) {		
			fprintf(fp, "ALTERNATE_ID = \"%s\"\n", altICAO);
			altFound = TRUE;
			memcpy(&alt, &doc->db[bestApt], sizeof(dbRec));
		}
	}
*/
	CString holdID;
	// doc->db[exportTable[plancount-2].index].type;
	holdID = CString(doc->db[exportTable[plancount-2].index].id);
	holdID.TrimRight();
	if (holdID != "")
		fprintf(fp, "HOLD_ID = \"%s\"\n", holdID);

	fprintf(fp, "DEPARTURE_ID = \"%s\"\n", depICAO);
	fprintf(fp, "\n");

	// use maximum plantable altitude (to include TOC/BOD) for filed altitude
	int maxAlt = 0;
	for (k=0; k<plantable.GetSize(); k++)
		if((int)(plantable[k].alt) > maxAlt)
			maxAlt = (int)(plantable[k].alt);

	double tdist = exportTable[plancount-1].Tdist;
	int flightLevel = maxAlt / 1000;

	fprintf(fp, "; ----------------------------------------------------------------------\n");
	fprintf(fp, "; Begin Nav Generated variables\n");
	fprintf(fp, "; ----------------------------------------------------------------------\n");
	fprintf(fp, "GET_FLIGHT_PLAN:\n");
	fprintf(fp, "    Title \"!RC99 %s to %s (filename: %s)\"\n", depICAO, arrICAO, d.GetFileName());
	fprintf(fp, "    Description \"%s to %s, %.0lf nm at FL%d\"\n", depName, arrName, tdist, flightLevel);
//	fprintf(fp, "    Description \"Generated by Nav on %s \"\n", timeString);
	fprintf(fp, "\n");

	fprintf(fp, "    flight_planner = 5\n");
	fprintf(fp, "    heading_deviation = 15\n");
	fprintf(fp, "    altitude_deviation = 200\n");
	fprintf(fp, "    speed_deviation = 15\n");
    
	fprintf(fp, "    filed_altitude = %d\n", (int) maxAlt/10);

	fprintf(fp, "    departure_elevation = %d\n", (int)(dep.alt/.3048));
	fprintf(fp, "    arrival_elevation = %d\n", (int)(arr.alt/.3048));
	if (altFound)
		fprintf(fp, "    alternate_elevation = %d\n", (int)(alt.alt/.3048));

	// search for nearest ATIS (optional)
	double dist;
	int atisIndex = doc->findNearestNavaid(dep.lat, dep.lon, 10);
	if (-1 != atisIndex) {
		dist = view->gcDist(doc->db[atisIndex].lat, doc->db[atisIndex].lon, dep.lat, dep.lon);
		if (dist < 50000)
			fprintf(fp, "    departure_atis_freq = %d\n", (int)(doc->db[atisIndex].freq*100+.4));
	}
	atisIndex = doc->findNearestNavaid(arr.lat, arr.lon, 10);
	if (-1 != atisIndex) {
		dist = view->gcDist(doc->db[atisIndex].lat, doc->db[atisIndex].lon, arr.lat, arr.lon);
		if (dist < 50000)
			fprintf(fp, "    arrival_atis_freq = %d\n", (int)(doc->db[atisIndex].freq*100+.4));
	}

	int checkpoints = plancount - 2;
	fprintf(fp, "    number_of_checkpoints = %d\n", checkpoints);
	fprintf(fp, "    departure_latitude = %lf\n", dep.lat);
	fprintf(fp, "    departure_longitude = %lf\n", dep.lon);

	fprintf(fp, "    arrival_latitude = %lf\n", arr.lat);
	fprintf(fp, "    arrival_longitude = %lf\n", arr.lon);
	
	if (altFound) {
		fprintf(fp, "    alternate_latitude  = %lf\n", alt.lat);
		fprintf(fp, "    alternate_longitude  = %lf\n", alt.lon);
	}

    for (int i=1; i<plancount-1; i++) {
		fprintf(fp, "    checkpoint_%d_latitude = %lf\n", i, exportTable[i].lat);
		fprintf(fp, "    checkpoint_%d_longitude = %lf\n", i, exportTable[i].lon);
		fprintf(fp, "    checkpoint_%d_altitude = %d\n", i, (int)(exportTable[i].alt/10));
		int checkPointType = doc->db[exportTable[i].index].type;
		if ((checkPointType == 1) || (checkPointType == 3)) { // VOR or NDB
			double checkpointFreq = doc->db[exportTable[i].index].freq;
			fprintf(fp, "    checkpoint_%d_freq = %0.2lf\n", i, checkpointFreq);
		}
		int heading = (int)(exportTable[i].heading+0.5) % 360;
		fprintf(fp, "    checkpoint_%d_heading = %d\n", i, heading);
	}

	// departure runways
	int numRunways = doc->lookUpRunways(exportTable[0].index, runwaysIndexList);
	double magvar = doc->GetMagVar(exportTable[0].lat, exportTable[0].lon);
	if (magvar>0) magvar += .5; // help with rounding
	else if (magvar<0) magvar -= .5;
	// sort runwaysIndexList by length, RC99 seems to prefer the first one listed
    for (i=0; i<numRunways-1; i++) {
		for (int j=i+1; j<numRunways; j++) {
			if (doc->db[runwaysIndexList[j]].range > doc->db[runwaysIndexList[i]].range) {
				int t = runwaysIndexList[i];
				runwaysIndexList[i] = runwaysIndexList[j];
				runwaysIndexList[j] = t;
			}
		}
	}
    for (i=0; i<numRunways; i++) {
		int rwHeading = ((int) (doc->db[runwaysIndexList[i]].approach+magvar))%360;
		double rwLat = doc->db[runwaysIndexList[i]].lat;
		double rwLon = doc->db[runwaysIndexList[i]].lon;
		int rwAlt = (int) (doc->db[runwaysIndexList[i]].alt / .3048);
		CString rwId = CString(doc->db[runwaysIndexList[i]].id);
		rwId.TrimRight();
		int ind;
		if (-1 < (ind=rwId.ReverseFind('L')))
			rwId.SetAt(ind, '1');
		else if (-1 < (ind=rwId.ReverseFind('C')))
			rwId.SetAt(ind, '2');
		else if (-1 < (ind=rwId.ReverseFind('R')))
			rwId.SetAt(ind, '3');
		else
			rwId = rwId + "0";
		// runway is coded. 41 is 4L, 42 would be 4C, 43 would be 4R, 40 is just 4)
 		fprintf(fp, "    d_runway_%d_id = %s\n", i+1, rwId);
 		fprintf(fp, "    d_runway_%d_heading = %d\n", i+1, rwHeading);
 		fprintf(fp, "    d_runway_%d_latitude = %lf\n", i+1, rwLat);
 		fprintf(fp, "    d_runway_%d_longitude = %lf\n", i+1, rwLon);
 		fprintf(fp, "    d_runway_%d_altitude = %d\n", i+1, rwAlt);

		// treat reciprocal direction as a different runway
		rwId = CString(doc->db[runwaysIndexList[i]].id);
		CString recipId = rwId.SpanIncluding("0123456789");
		int d;
		sscanf(recipId.GetBuffer(1), "%d", &d);
		recipId.ReleaseBuffer();
		d = (d+18)%36;
		if (d == 0)
			d = 36;
		if (-1 < (ind=rwId.ReverseFind('L')))
	  		fprintf(fp, "    d_runway_%d_id = %d3\n", i+1+numRunways, d);
		else if (-1 < (ind=rwId.ReverseFind('C')))
	  		fprintf(fp, "    d_runway_%d_id = %d2\n", i+1+numRunways, d);
		else if (-1 < (ind=rwId.ReverseFind('R')))
	  		fprintf(fp, "    d_runway_%d_id = %d1\n", i+1+numRunways, d);
		else
	  		fprintf(fp, "    d_runway_%d_id = %d0\n", i+1+numRunways, d);
		fprintf(fp, "    d_runway_%d_heading = %d\n", i+1+numRunways, (rwHeading+180)%360);
 		fprintf(fp, "    d_runway_%d_latitude = %lf\n", i+1+numRunways, rwLat);
 		fprintf(fp, "    d_runway_%d_longitude = %lf\n", i+1+numRunways, rwLon);
 		fprintf(fp, "    d_runway_%d_altitude = %d\n", i+1+numRunways, rwAlt);
	}
	
	// approach runways
	numRunways = doc->lookUpRunways(exportTable[plancount-1].index, runwaysIndexList);
	magvar = doc->GetMagVar(exportTable[plancount-1].lat, exportTable[plancount-1].lon);
	if (magvar>0) magvar += .5; // help with rounding
	else if (magvar<0) magvar -= .5;
	// sort runwaysIndexList by length, RC99 seems to prefer the first one listed
    for (i=0; i<numRunways-1; i++) {
		for (int j=i+1; j<numRunways; j++) {
			if (doc->db[runwaysIndexList[j]].range > doc->db[runwaysIndexList[i]].range) {
				int t = runwaysIndexList[i];
				runwaysIndexList[i] = runwaysIndexList[j];
				runwaysIndexList[j] = t;
			}
		}
	}
    for (i=0; i<numRunways; i++) {
		int rwHeading = ((int) (doc->db[runwaysIndexList[i]].approach+magvar))%360;
		int rwHeadingNoMag = (int) doc->db[runwaysIndexList[i]].approach;
		double rwLat = doc->db[runwaysIndexList[i]].lat;
		double rwLon = doc->db[runwaysIndexList[i]].lon;
		int rwAlt = (int) (doc->db[runwaysIndexList[i]].alt / .3048);
		CString rwId = CString(doc->db[runwaysIndexList[i]].id);
		int eIls, wIls;
		doc->lookUpILS(runwaysIndexList[i], eIls, wIls);
		rwId.TrimRight();
		int ind;
		if (-1 < (ind=rwId.ReverseFind('L')))
			rwId.SetAt(ind, '1');
		else if (-1 < (ind=rwId.ReverseFind('C')))
			rwId.SetAt(ind, '2');
		else if (-1 < (ind=rwId.ReverseFind('R')))
			rwId.SetAt(ind, '3');
		else
			rwId = rwId + "0";
 		fprintf(fp, "    a_runway_%d_id = %s\n", i+1, rwId);
 		fprintf(fp, "    a_runway_%d_heading = %d\n", i+1, rwHeading);
 		fprintf(fp, "    a_runway_%d_latitude = %lf\n", i+1, rwLat);
 		fprintf(fp, "    a_runway_%d_longitude = %lf\n", i+1, rwLon);
 		fprintf(fp, "    a_runway_%d_altitude = %d\n", i+1, rwAlt);
		// check for ils
		if ((rwHeadingNoMag < 180.0) && (eIls != -1)) {
			fprintf(fp, "    a_runway_%d_freq = %.2lf\n", i+1, doc->db[eIls].freq);
		} else if ((rwHeadingNoMag >= 180.0) && (wIls != -1)) {
			fprintf(fp, "    a_runway_%d_freq = %.2lf\n", i+1, doc->db[wIls].freq);
		}

		// treat reciprocal direction as a different runway
		rwHeadingNoMag = (rwHeadingNoMag+180)%360;
		rwId = CString(doc->db[runwaysIndexList[i]].id);
		CString recipId = rwId.SpanIncluding("0123456789");
		int d;
		sscanf(recipId.GetBuffer(1), "%d", &d);
		recipId.ReleaseBuffer();
		d = (d+18)%36;
		if (d == 0)
			d = 36;
		if (-1 < (ind=rwId.ReverseFind('L')))
	  		fprintf(fp, "    a_runway_%d_id = %d3\n", i+1+numRunways, d);
		else if (-1 < (ind=rwId.ReverseFind('C')))
	  		fprintf(fp, "    a_runway_%d_id = %d2\n", i+1+numRunways, d);
		else if (-1 < (ind=rwId.ReverseFind('R')))
	  		fprintf(fp, "    a_runway_%d_id = %d1\n", i+1+numRunways, d);
		else
	  		fprintf(fp, "    a_runway_%d_id = %d0\n", i+1+numRunways, d);
		fprintf(fp, "    a_runway_%d_heading = %d\n", i+1+numRunways, (rwHeading+180)%360);
 		fprintf(fp, "    a_runway_%d_latitude = %lf\n", i+1+numRunways, rwLat);
 		fprintf(fp, "    a_runway_%d_longitude = %lf\n", i+1+numRunways, rwLon);
 		fprintf(fp, "    a_runway_%d_altitude = %d\n", i+1+numRunways, rwAlt);
		// check for ils
		if ((rwHeadingNoMag < 180.0) && (eIls != -1)) {
			fprintf(fp, "    a_runway_%d_freq = %.2lf\n", i+1+numRunways, doc->db[eIls].freq);
		} else if ((rwHeadingNoMag >= 180.0) && (wIls != -1)) {
			fprintf(fp, "    a_runway_%d_freq = %.2lf\n", i+1+numRunways, doc->db[wIls].freq);
		}
	}
	
	fprintf(fp, "RETURN");
	fclose(fp);

	WriteBriefing(filename, exportTable[0].index, exportTable[plancount-1].index);

	exportTable.RemoveAll();
	exportTable.FreeExtra();

	SetCurrentDirectory(currentDir);
	main->SetStatusBar("Ready");
}

void FlightPlan::WriteBriefing(CString name, int dep, int arr) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	CString fs2kPath = GetFS2000Path();
	if (fs2kPath == "") // FS2000 not found in registry, so skip briefing
		return;

	// extract plan base name
	int index;
	if (-1 != (index = name.ReverseFind('\\'))) {
		name = name.Right(name.GetLength() - index - 1);
	}
	if (-1 != (index = name.ReverseFind('.'))) {
		name = name.Left(index);
	}

	CString briefName = fs2kPath + "\\adv\\other\\" + name + ".brf";
    ExportFile(briefName);

	FILE *fp = fopen(briefName, "a");
	CString details, line;
	int len;
	doc->formatData(details, dep);
	line = "";
	len = details.GetLength();
	for (int i=0; i<len; i++)
		if ('\r' != details[i])
			line += details[i];
	fprintf(fp, "\n%s\n", line);
	details = "";
	doc->formatData(details, arr);
	line = "";
	len = details.GetLength();
	for (i=0; i<len; i++)
		if ('\r' != details[i])
			line += details[i];
	fprintf(fp, "\n%s\n", line);
	fclose(fp);
}

void FlightPlan::selectPlanItem(int index) {
	update();
	sel = planToSel(index);
	update();
}

double FlightPlan::parseFS2000position(char dir, int deg, double minsec) {
	double sign = 1.0;
	if ((dir == 'S') || (dir == 'W'))
		sign = -1.0;
	double fract = minsec/60.0;
	return sign * (deg + fract);
}

void FlightPlan::formatFS2000position(char *buf, double decimal, int latFlag) {
	char dir;
	if (decimal < 0) {
		if (latFlag)
			dir = 'S';
		else
			dir = 'W';
	} else {
		if (latFlag)
			dir = 'N';
		else
			dir = 'E';
	}
	decimal = fabs(decimal);
	int deg = (int) decimal;
	decimal = decimal - deg;
	double min = 60 * decimal;
	if (min >= 59.995) {
		min = 0.0;
		deg += 1;
	}
	sprintf(buf, "%c%d* %.2lf'", dir, deg, min);
}

void FlightPlan::OnFileExportFS2000() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	// build table without TOC/BOD so we can ignore them
	CArray<planRec, planRec> exportTable;
	exportTable.SetSize(0,50);	
	planRec r;
	for (int k=0; k<plantable.GetSize(); k++) {
		if (plantable[k].index >= 0) {
			// check to see if we need to replace a runway with an airport
			if (( (k==0) || (k==plantable.GetSize()-1) ) && (doc->db[plantable[k].index].type == 8)) {
				int apt = doc->findNearestNavaid(doc->db[plantable[k].index].lat, doc->db[plantable[k].index].lon, 4);
				memcpy(&r, &plantable[k], sizeof(dbRec));
				r.index = apt;
				r.alt = doc->db[apt].alt;
				r.lat = doc->db[apt].lat;
				r.lon = doc->db[apt].lon;
				exportTable.Add(r);
			} else
				exportTable.Add(plantable[k]);
		}
	}

	int plancount = exportTable.GetSize();

	if (plancount < 2) {
		AfxMessageBox("Need at least 2 items in plan to export");
		return;
	}
	
	int notAirports = TRUE;
	if (doc->db[exportTable[0].index].type == 4)
		if (doc->db[exportTable[plancount-1].index].type == 4)
			notAirports = FALSE;
	if (notAirports) {
		AfxMessageBox("Export failed.\nThe first and last items in the plan must be airports");
		return;
	}

	char buf[256];
	dbRec dep, arr, alt;
	int altFound = FALSE;
	memcpy(&dep, &doc->db[exportTable[0].index], sizeof(dbRec));
	memcpy(&arr, &doc->db[exportTable[plancount-1].index], sizeof(dbRec));
	CString defaultName, depICAO, arrICAO, depName, arrName, altICAO;
	depName = CString(dep.name);
	depName.TrimRight();
	arrName = CString(arr.name);
	arrName.TrimRight();

	// build default file name
	if (doc->lookUpAirport(depICAO, dep.lat, dep.lon)) 
		defaultName = depICAO + "-";
	else {
		sprintf(buf, "Export failed\n, No ICAO name found for %s", depName);
		AfxMessageBox(buf);
		return;
	}
	if (doc->lookUpAirport(arrICAO, arr.lat, arr.lon)) 
		defaultName = defaultName + arrICAO;
	else {
		sprintf(buf, "Export failed\n, No ICAO name found for %s", arrName);
		AfxMessageBox(buf);
		return;
	}
	if (openPlanName.GetLength() > 0) {// overide if a plan is open
		int index = openPlanName.ReverseFind('\\');
		if (index == -1)
			defaultName = openPlanName;
		else
			defaultName = openPlanName.Right(openPlanName.GetLength() - index -1);
	}

	static char BASED_CODE szFilter[] = "FS2000 Plan Files (*.pln)|*.pln|All Files (*.*)|*.*||";
	CFileDialog d(FALSE, ".pln", defaultName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;
	// try to find fs2000 installation here to set default path	
	char currentDir[260], initialDir[260];
	GetCurrentDirectory(260, currentDir);

	CString fs2kPath = GetFS2000Path();
	if (fs2kPath != "") {
		sprintf(initialDir, "%s\\Pilots", fs2kPath);
		d.m_ofn.lpstrInitialDir = initialDir;
	}

	if (d.DoModal() != IDOK) {
		SetCurrentDirectory(currentDir);
		return;
	}
	CString filename = d.GetPathName();

	FILE *fp = fopen(filename, "w");
	if (fp == NULL) {
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		SetCurrentDirectory(currentDir);
		return;
	}

	fprintf(fp, "[flightplan]\n");
	fprintf(fp, "title=%s to %s\n", depICAO, arrICAO);
	fprintf(fp, "description=%s, %s\n", depICAO, arrICAO);
	fprintf(fp, "type=IFR\n");
	fprintf(fp, "routetype=1\n");

	// use maximum plantable altitude (to include TOC/BOD) for filed altitude
	int maxAlt = 0;
	for (k=0; k<plantable.GetSize(); k++)
		if((int)(plantable[k].alt) > maxAlt)
			maxAlt = (int)(plantable[k].alt);
	fprintf(fp, "cruising_altitude=%d\n", maxAlt);

	int runwaysIndexList[50];
	int found = FALSE, bestRW = 0, bestLen = 0; 
//	int	foundApt, bestApt, foundRW, goodRWs, goodLen;
	
	// check if an alternate arrival airport exists from loading a fs2000 plan
	if ((alternateID.GetLength() > 0) && (alternateName.GetLength() > 0)) {
		alt.alt = (int) (alternateAlt*.3048);
		alt.lat = alternateLat;
		alt.lon = alternateLon;
		altICAO = alternateID;
		strncpy(alt.id, alternateID.GetBuffer(1), 6);
		strncpy(alt.name, alternateName.GetBuffer(1), 31);
		found = TRUE;
	} else { // too slow to search for alternate this way
		/*
		for (int i=0; i<doc->airportIndex.GetSize(); i++) {
			double d = view->gcDist(doc->db[doc->airportIndex[i]].lat, doc->db[doc->airportIndex[i]].lon, arr.lat, arr.lon)/1852; 
			if (d > 50) // should be not more than 50 miles away from primary arrival
				continue;
			if (d < 20) // should be at least 20 miles away from primary arrival
				continue;
			foundApt = doc->airportIndex[i];
			// try to find one with a lot of (paved) runways to handle the heavys
			foundRW = doc->lookUpRunways(foundApt, runwaysIndexList);
			goodRWs = 0;
			goodLen = 0;
			for (int r=0; r<foundRW; r++) {
				int rwLength = (int) doc->db[runwaysIndexList[r]].range;
				int rwWidth = (int) doc->db[runwaysIndexList[r]].magdev;
				int rwType = (int) doc->db[runwaysIndexList[r]].freq; // Type1=concrete Type2=asphalt
				if (((rwType==1)||(rwType==2))&&(rwWidth>=50)&&(rwLength>=2000)) {
					goodRWs += 1;
					if (rwLength > goodLen)
						goodLen = rwLength;
				}
			}

			if ((goodRWs > bestRW) || ((goodLen>bestLen) && (goodRWs>=bestRW))){
				bestRW = foundRW;
				bestApt = foundApt;
				bestLen = goodLen;
				found = TRUE;
			}
		}
		if (found) {
			found = found | (doc->lookUpAirport(altICAO, doc->db[bestApt].lat, doc->db[bestApt].lon));
			if (found) {		
				altFound = TRUE;
				memcpy(&alt, &doc->db[bestApt], sizeof(dbRec));
			}
		}
			*/
	}
	char latb[30], lonb[30];
	formatFS2000position(latb, dep.lat, 1);
	formatFS2000position(lonb, dep.lon, 0);
	fprintf(fp, "departure_id=%s, %s, %s, +%09.2lf\n", depICAO, latb, lonb, (double) dep.alt/.3048);

	// find likely looking departure runway
	int numRunways = doc->lookUpRunways(exportTable[0].index, runwaysIndexList);
    for (int i=0; i<numRunways-1; i++) { // sort runwaysIndexList by length
		for (int j=i+1; j<numRunways; j++) {
			if (doc->db[runwaysIndexList[j]].range > doc->db[runwaysIndexList[i]].range) {
				int t = runwaysIndexList[i];
				runwaysIndexList[i] = runwaysIndexList[j];
				runwaysIndexList[j] = t;
			}
		}
	}
	CString rwId = CString(doc->db[runwaysIndexList[0]].id);
	rwId.TrimRight();
	fprintf(fp, "departure_position=%s\n", rwId.GetBuffer(1));

	formatFS2000position(latb, arr.lat, 1);
	formatFS2000position(lonb, arr.lon, 0);
	fprintf(fp, "destination_id=%s, %s, %s, +%09.2lf\n", arrICAO, latb, lonb, (double) arr.alt/.3048);
	if (found) {
		formatFS2000position(latb, alt.lat, 1);
		formatFS2000position(lonb, alt.lon, 0);
		fprintf(fp, "alternate_id=%s, %s, %s, +%09.2lf\n", altICAO, latb, lonb, (double) alt.alt/.3048);
	}
	fprintf(fp, "departure_name=%s\n", dep.name);
	fprintf(fp, "destination_name=%s\n", arr.name);
	if (found) 
		fprintf(fp, "alternate_name=%s\n", alt.name);

	// print waypoints
	char c;
	CString id;
    for (i=0; i<exportTable.GetSize(); i++) {
		formatFS2000position(latb, exportTable[i].lat, 1);
		formatFS2000position(lonb, exportTable[i].lon, 0);
		int type = doc->db[exportTable[i].index].type;
		if (i == 0)
			fprintf(fp, "waypoint.%d=%s, A, %s, %s, +%09.2lf,\n", i, depICAO, latb, lonb, (double) dep.alt/.3048);
		else if (i == exportTable.GetSize()-1)
			fprintf(fp, "waypoint.%d=%s, A, %s, %s, +%09.2lf,\n", i, arrICAO, latb, lonb, (double) arr.alt/.3048);
		else {
			if (type == 1) 
				c = 'N';
			else if (type == 3) 
				c = 'V';
			else
				c = 'I';
			id = CString(doc->db[exportTable[i].index].id);
			id.TrimRight();
			if (id.GetLength() == 0) {
				id = CString(doc->db[exportTable[i].index].name);
				id.TrimRight();
			}
			if (id.GetLength() == 0) 
				id = "mark";
			fprintf(fp, "waypoint.%d=%s, %c, %s, %s, +%09.2lf,\n", i, id.GetBuffer(1), c, latb, lonb, 0.0);
		}
	}

	fclose(fp);
	exportTable.RemoveAll();
	exportTable.FreeExtra();
	SetCurrentDirectory(currentDir);
}

int readline(FILE *fp, char* buffer, int bufLen) {
	int c;
	for (int i=0; i<bufLen; i++) {
		c = fgetc(fp);
		if ((c == '\n') || (c == EOF)) {
			memset(&buffer[i], NULL, bufLen-i); // zero rest of buffer
			if ((i == 0) && (c == EOF))
				return FALSE; // no more to read 
			else
				return TRUE;
		} else
			buffer[i] = c;
	}
	return FALSE; // buffer overflow
}

CString FlightPlan::GetFS2000Path() {
	CString result = "";
	HKEY key; 
	if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\Microsoft Games\\Flight Simulator\\7.0", 0, KEY_READ, &key)) {
		unsigned char pathbuf[1024];
		DWORD type, length = 1024;
		if (ERROR_SUCCESS ==  RegQueryValueEx(key, "path", NULL, &type, pathbuf, &length)) {
			result =  pathbuf;
			RegCloseKey(key);
			return result;
		}
		RegCloseKey(key);
	}
	return result; // silently fail
}

void FlightPlan::OnFileImportFS2000() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	static char BASED_CODE szFilter[] = "FS2000 Plan Files (*.pln)|*.pln|All Files (*.*)|*.*||";
	CFileDialog d(TRUE, ".pln", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	d.m_ofn.hwndOwner = m_hWnd;

	// try to find fs2000 installation here to set default path	
	char currentDir[260], initialDir[260];
	GetCurrentDirectory(260, currentDir);

	CString fs2kPath = GetFS2000Path();
	if (fs2kPath != "") {
		sprintf(initialDir, "%s\\Pilots", fs2kPath);
		d.m_ofn.lpstrInitialDir = initialDir;
	}

	if (d.DoModal() != IDOK) {
		SetCurrentDirectory(currentDir);
		return;
	}
	CString filename = d.GetPathName();

	FILE *fp = fopen(filename, "r");
	char buf[200];
	if (fp == NULL) {
		sprintf(buf, "Could not open %s", filename);
		AfxMessageBox(buf);
		SetCurrentDirectory(currentDir);
		return;
	}

	char rBuffer[1024];
	int rBufLen = 1024;
	readline(fp, rBuffer, 1024);
	if (0 != strcmp("[flightplan]", rBuffer)) {
		fclose(fp);
		char buf[256];
		sprintf(buf, "%s\ndoes not appear to be a valid FS2000 plan", filename);
		AfxMessageBox(buf);
		SetCurrentDirectory(currentDir);
		return;
	}

	openPlanName = filename;

	char id[64], type[64], latDir, lonDir, junk[64], depname[64], arrname[64], altname[64];
	int latDeg, lonDeg, wpt, ntype;
	double latMS, lonMS, alt, lat, lon;

	while (readline(fp, rBuffer, 1024)) {
		if (1 == sscanf(rBuffer, "cruising_altitude=%lf", &alt)) 
			cruisealtitude = (long) alt;
	}
	fseek(fp, 0, SEEK_SET);

	while (readline(fp, rBuffer, 1024)) {
		if (1 == sscanf(rBuffer, "departure_name=%s", junk)) {
			int skip = strlen("departure_name=");
			strncpy(depname, rBuffer+skip, 64-skip);
		}
	}
	fseek(fp, 0, SEEK_SET);

	while (readline(fp, rBuffer, 1024)) {
		if (1 == sscanf(rBuffer, "destination_name=%s", junk)) {
			int skip = strlen("destination_name=");
			strncpy(arrname, rBuffer+skip, 64-skip);
		}
	}
	fseek(fp, 0, SEEK_SET);

	while (readline(fp, rBuffer, 1024)) {
		if (1 == sscanf(rBuffer, "alternate_name=%s", junk)) {
			int skip = strlen("alternate_name=");
			strncpy(altname, rBuffer+skip, 64-skip);
			alternateName = CString(altname);
		}
	}
	fseek(fp, 0, SEEK_SET);

	while (readline(fp, rBuffer, 1024)) {
		if (8 == sscanf(rBuffer, "alternate_id=%s %c%d* %lf', %c%d* %lf', +%lf", 
			id, &latDir, &latDeg, &latMS, &lonDir, &lonDeg, &lonMS, &alt)) {
			alternateLat = parseFS2000position(latDir, latDeg, latMS);
			alternateLon = parseFS2000position(lonDir, lonDeg, lonMS);
			alternateAlt = alt;
			alternateID = CString(id).SpanExcluding(","); // delete trailing comma
		}
	}
	fseek(fp, 0, SEEK_SET);

	view->plancount = 0;
	int index, oldCount;
	CString csid;
	oldCount = doc->dbCount;
	while (readline(fp, rBuffer, 1024)) {
		alt = 0;
		if (10 == sscanf(rBuffer, "waypoint.%d=%s %s %c%d* %lf', %c%d* %lf', +%lf", 
			  &wpt, id, type, &latDir, &latDeg, &latMS, &lonDir, &lonDeg, &lonMS, &alt)
			  || (9 == sscanf(rBuffer, "waypoint.%d=%s %s %c%d* %lf', %c%d* %lf'", 
			  &wpt, id, type, &latDir, &latDeg, &latMS, &lonDir, &lonDeg, &lonMS) )) {
			lat = parseFS2000position(latDir, latDeg, latMS);
			lon = parseFS2000position(lonDir, lonDeg, lonMS);
			csid = CString(id).SpanExcluding(","); // delete trailing comma
			if ('A' == type[0])
				ntype = 4;
			else if ('N' == type[0])
				ntype = 1;
			else if ('V' == type[0])
				ntype = 3;
			else 
				ntype = 13;

			if (-1 == (index=doc->findIndex(ntype, lat, lon, (int) (alt*.3048), 
				        0, csid.GetBuffer(1), "", 0, 0, 0, 10000, 1))) { // find minimum within 10 km max
				// insert navaid into database
				if (wpt == 0)
					doc->addDB(ntype, lat, lon, (int) (alt*.3048), 0, csid.GetBuffer(1), 
					     depname, 0, 0, 0, "unknown");
				else if ((wpt > 0) && (ntype == 4))
					doc->addDB(ntype, lat, lon, (int) (alt*.3048), 0, csid.GetBuffer(1), 
					     arrname, 0, 0, 0, "unknown");
				else
					doc->addDB(ntype, lat, lon, (int) (alt*.3048), 0, csid.GetBuffer(1), 
					     csid.GetBuffer(1), 0, 0, 0, "unknown");
				index = doc->dbCount - 1;
			} 
			view->plan[view->plancount] = index;
			view->plancount += 1;
		}
	}
	if (doc->dbCount > oldCount)
		doc->Index();
	fseek(fp, 0, SEEK_SET);


	fclose(fp);
	SetCurrentDirectory(currentDir);
	view->abort = TRUE;
	view->Invalidate(TRUE);
	update();
}

// set plantable flags to non-zero if plan items are or have available a DP or STAR
void FlightPlan::checkDpStar() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	int item, size = plantable.GetSize();
	int dpApt = -1, starApt = -1;
	CString icao;
	DpStarAirport *dpDpAirport, *dpStarAirport;
	dpDpAirport = dpStarAirport = NULL;
	starName = dpName = "";

	if (4 == doc->db[plantable[0].index].type) 
		dpApt = plantable[0].index;
	else if (8 == doc->db[plantable[0].index].type)  // find airport
		dpApt = doc->findNearestNavaid(plantable[0].lat, plantable[0].lon, 4);
	if (dpApt != -1) {
		doc->lookUpAirport(icao, doc->db[dpApt].lat, doc->db[dpApt].lon, dpApt);
		dpDpAirport = doc->dpStar.lookupAirport(icao);
	}

	if (4 == doc->db[plantable[size-1].index].type)
		starApt = plantable[size-1].index;
	else if (8 == doc->db[plantable[size-1].index].type)  // find airport
		starApt = doc->findNearestNavaid(plantable[size-1].lat, plantable[size-1].lon, 4);
	if (starApt != -1) {
		doc->lookUpAirport(icao, doc->db[starApt].lat, doc->db[starApt].lon, starApt);
		dpStarAirport = doc->dpStar.lookupAirport(icao);
	}

	for (item=0; item<size; item++) // reset all dpStar flags
		plantable[item].dpStar = -100;
	wptDp = wptStar = -1;

	if (dpDpAirport != NULL) {
		int maxLength = 0;
		int maxLengthP = 0;
		DpStarRoute* maxRoute;
		DpStarRoute* dpStarRoute = dpDpAirport->firstDP();
		if (dpStarRoute != NULL) { // mark airport if any route exists
			plantable[0].dpStar = dpApt;
			wptDp = 0; // remember last waypoint of DP
		}
		while (dpStarRoute != NULL) { // check each route
			int wpt = 0;
			int pWpt = 0;
			int rtOK = 1;
			DpStarWaypoint *w = dpStarRoute->firstWaypoint();
			while (w != NULL) {
				wpt++;
dpAgain:
				pWpt++;
				if ((wpt >= size)||(pWpt >= size)) {
					rtOK = 0;
					break;
				}
				if (plantable[pWpt].index < 0)  // skip toc, bod, toc/bod
					goto dpAgain;
				if (!(view->gcDist(w->latitude, w->longitude, plantable[pWpt].lat, plantable[pWpt].lon) < 7000)) {
					rtOK = 0;
					break;
				}
				w = dpStarRoute->nextWaypoint();
			}
			if (rtOK) { // this route matched the plan
				if (wpt > maxLength) { // longest one yet
					maxLength = wpt;
					maxRoute = dpStarRoute;
					maxLengthP = pWpt;
				}
			}

			dpStarRoute = dpDpAirport->nextDP();
		}
		if (maxLength > 0) {
			dpName = maxRoute->name; // save matching route name
			for (int j=1; j<=maxLengthP; j++)
				plantable[j].dpStar = dpApt;
			wptDp = maxLengthP; // remember last waypoint of DP
		}
	}

	if (dpStarAirport != NULL) {
		int maxRtStart, maxLength = 0;
		int maxLengthP = 0;
		DpStarRoute* maxRoute;
		DpStarRoute* dpStarRoute = dpStarAirport->firstSTAR();
		if (dpStarRoute != NULL) { // mark airport if any route exists
			plantable[size-1].dpStar = starApt;
			wptStar = size-1; // remember 1st waypoint of STAR
		}
		while (dpStarRoute != NULL) { // check each route
			int  count = 0;
			int rtOK = 1;
			int wpt, pWpt, rtStart = size - 1;

			DpStarWaypoint *w = dpStarRoute->firstWaypoint();
			while (w != NULL) {
				rtStart--; // countdown once for each waypoint
				if (rtStart < 0) {
					rtOK = 0;
					break;
				}
				if (plantable[rtStart].index < 0)  // skip toc, bod, toc/bod
					rtStart--;
				w = dpStarRoute->nextWaypoint();
			}
			
			wpt = pWpt = rtStart -1;
			w = dpStarRoute->firstWaypoint();
			while ((w != NULL) && rtOK) {
				count++;
				wpt++;
starAgain:
				pWpt++;
				if ((wpt >= size)||(pWpt >= size)) {
					rtOK = 0;
					break;
				}
				if (plantable[pWpt].index < 0)  // skip toc, bod, toc/bod
					goto starAgain;
				if (!(view->gcDist(w->latitude, w->longitude, plantable[pWpt].lat, plantable[pWpt].lon) < 7000)) {
					rtOK = 0;
					break;
				}
				w = dpStarRoute->nextWaypoint();
			}
			if (rtOK) { // this route matched the plan
				if (count > maxLength) { // longest one yet
					maxLength = count;
					maxRoute = dpStarRoute;
					maxLengthP = pWpt;
					maxRtStart = rtStart;
				}
			}
			dpStarRoute = dpStarAirport->nextSTAR();
		}
		if (maxLength > 0) {
			starName = maxRoute->name; // save matching route name
			for (int j=maxRtStart; j<size-1; j++)
				plantable[j].dpStar = starApt;
			wptStar = maxRtStart; // remember 1st waypoint of STAR
		}
	}
}

// get the index of the image associated with this waypoint
int FlightPlan::getImage(int k) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	int dbtype, image;
	if (plantable[k].index >= 0)
		dbtype = doc->db[plantable[k].index].type;
	else 
		dbtype = plantable[k].index; // toc, bod, toc/bod

	// set plan item icon
	if ((view->flyToWaypoint-1) == k)
		image = 1;			// target
	else {
		if (1 == dbtype)		
			image = 3 + (plantable[k].dpStar==-100 ? 0 : 11);		// ndb
		else if (3 == dbtype)	
			image = 2 + (plantable[k].dpStar==-100 ? 0 : 11);		// vor
		else if ((4 == dbtype)||(8 == dbtype))	
			image = 4 + (plantable[k].dpStar==-100 ? 0 : 11);		// airport
		else if (13 == dbtype)	
			image = 5 + (plantable[k].dpStar==-100 ? 0 : 11);		// waypoint
		else if ((5 == dbtype)||(6 == dbtype)||(7 == dbtype))	
			image = 6 + (plantable[k].dpStar==-100 ? 0 : 11);		// marker
		else if (2 == dbtype)	
			image = 7 + (plantable[k].dpStar==-100 ? 0 : 11);		// ils
		else if (14 == dbtype)	
			image = 8 + (plantable[k].dpStar==-100 ? 0 : 11);		// route
		else if (9 == dbtype)	
			image = 9 + (plantable[k].dpStar==-100 ? 0 : 11);		// gps
		else if (-1 == dbtype)	
			image = 10 + (plantable[k].dpStar==-100 ? 0 : 11);		// toc
		else if (-2 == dbtype)	
			image = 11 + (plantable[k].dpStar==-100 ? 0 : 11);		// bod
		else if (-3 == dbtype)	
			image = 12 + (plantable[k].dpStar==-100 ? 0 : 11);		// toc/bod
		else
			image = 0;		// nothing
	}
	return image;
}

int FlightPlan::enableDPSTARButton() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	int size = plantable.GetSize();
	if (size == 0)
		return FALSE;
	int selected = getSel();

	int dbtypeDep, dbtypeArr, dep, arr;
	if (plantable[0].index >= 0)
		dbtypeDep = doc->db[plantable[0].index].type;
	else 
		dbtypeDep = plantable[0].index; // toc, bod, toc/bod
	dep = (dbtypeDep==4)||(dbtypeDep==8);

	if (plantable[size-1].index >= 0)
		dbtypeArr = doc->db[plantable[size-1].index].type;
	else 
		dbtypeArr = plantable[size-1].index; // toc, bod, toc/bod
	arr = (dbtypeArr==4)||(dbtypeArr==8);

	// enable button if a departure or arrival airport with DP or STAR is selected
	if (dep && (selected == 0) && (plantable[0].dpStar!=-100)) return TRUE;
	if (arr && (selected == size-1) && (plantable[size-1].dpStar!=-100)) return TRUE;

	// or enable button if the currrent plan can be saved as a DP or STAR
	if ((dep && !arr) || (arr && !dep)) {
		// make sure it's not already entered as a dp/star
		int allMarked = TRUE;
		for (int i=0; i<size; i++) 
			if (plantable[i].dpStar==-100)
				allMarked = FALSE;
		return !allMarked; // is a possible DP or STAR if at least 1 is unmarked 
	}

	return FALSE;
}

void FlightPlan::OnDpstar() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	int dbtypeDep, dbtypeArr, dep, arr;
	int dpApt = -1, starApt = -1;
	CString icao;

	int size = plantable.GetSize();
	if (size == 0)
		return;

	if (plantable[0].index >= 0)
		dbtypeDep = doc->db[plantable[0].index].type;
	else 
		dbtypeDep = plantable[0].index; // toc, bod, toc/bod
	dep = (dbtypeDep==4)||(dbtypeDep==8);

	if (plantable[size-1].index >= 0)
		dbtypeArr = doc->db[plantable[size-1].index].type;
	else 
		dbtypeArr = plantable[size-1].index; // toc, bod, toc/bod
	arr = (dbtypeArr==4)||(dbtypeArr==8);


	int selected = getSel();

	if (dep && (selected == 0) && (plantable[0].dpStar!=-100)) { // select an existing DP
		if (4 == dbtypeDep) 
			dpApt = plantable[0].index;
		else if (8 == dbtypeDep)  // find airport
			dpApt = doc->findNearestNavaid(plantable[0].lat, plantable[0].lon, 4);
		doc->lookUpAirport(icao, doc->db[dpApt].lat, doc->db[dpApt].lon, dpApt);

		SelectDpStar dialog(this);
		dialog.isStar = FALSE;
		dialog.icao = icao;
		dialog.dpStarName = dpName;
		dialog.airportIndex = dpApt;
		dialog.fp = this;
		if (dialog.DoModal() != IDOK)
			return;
		mergeDpStar(icao, dialog.result, FALSE);
		return;
	}

	if (arr && (selected == size-1) && (plantable[size-1].dpStar!=-100)) { // select an existing STAR
		if (4 == dbtypeArr) 
			starApt = plantable[size-1].index;
		else if (8 == dbtypeArr)  // find airport
			starApt = doc->findNearestNavaid(plantable[size-1].lat, plantable[size-1].lon, 4);
		doc->lookUpAirport(icao, doc->db[starApt].lat, doc->db[starApt].lon, starApt);

		SelectDpStar dialog(this);
		dialog.isStar = TRUE;
		dialog.icao = icao;
		dialog.dpStarName = starName;
		dialog.airportIndex = starApt;
		dialog.fp = this;
		if (dialog.DoModal() != IDOK)
			return;
		mergeDpStar(icao, dialog.result, TRUE);
		return;
	}

	if (size == 1)
		return;

	if (dep && !arr) { // save the new DP
		if (4 == dbtypeDep) 
			dpApt = plantable[0].index;
		else if (8 == dbtypeDep)  // find airport
			dpApt = doc->findNearestNavaid(plantable[0].lat, plantable[0].lon, 4);
		doc->lookUpAirport(icao, doc->db[dpApt].lat, doc->db[dpApt].lon, dpApt);

		CreateDpStar dialog(this);
		dialog.m_prompt = "Save the current plan as a DP named:";
		if (dialog.DoModal() != IDOK)
			return;

		DpStarAirport a(icao);
		DpStarRoute d(dialog.m_routename); 
		for (int w=1; w<size; w++) {
			if (plantable[w].index < 0)
				continue;
			d.addWaypoint(DpStarWaypoint(doc->db[plantable[w].index].id, doc->db[plantable[w].index].lat, doc->db[plantable[w].index].lon));
		}
		a.addDP(d);

		doc->dpStar.addAirport(a);
		// save new additions separately for later merging into standard database
		DpStar mine;
		mine.readFile("mydpstar.xml");
		mine.addAirport(a);
		mine.writeFile("mydpstar.xml");
		update();
	}

	if (arr && !dep) { // save the new STAR
		if (4 == dbtypeArr) 
			starApt = plantable[size-1].index;
		else if (8 == dbtypeArr)  // find airport
			starApt = doc->findNearestNavaid(plantable[size-1].lat, plantable[size-1].lon, 4);
		doc->lookUpAirport(icao, doc->db[starApt].lat, doc->db[starApt].lon, starApt);

		CreateDpStar dialog(this);
		dialog.m_prompt = "Save the current plan as a STAR named:";
		if (dialog.DoModal() != IDOK)
			return;

		DpStarAirport a(icao);
		DpStarRoute d(dialog.m_routename); // fetch real name here
		for (int w=0; w<size-1; w++) {
			if (plantable[w].index < 0)
				continue;
			d.addWaypoint(DpStarWaypoint(doc->db[plantable[w].index].id, doc->db[plantable[w].index].lat, doc->db[plantable[w].index].lon));
		}
		a.addSTAR(d);

		doc->dpStar.addAirport(a);
		// save new additions separately for later merging into standard database
		DpStar mine;
		mine.readFile("mydpstar.xml");
		mine.addAirport(a);
		mine.writeFile("mydpstar.xml");
		update();
	}
}

// get the index of the current selection
int FlightPlan::getSel() { 
	int size = m_list.GetItemCount();
	for (int i=0; i<size; i++) {
		if(m_list.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
			return i;
	}
	return -1; // no selection
}

// convert the current selection index into a view->plan index
int FlightPlan::selToPlan(int selected) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	int save = plantable[selected].index;
	if (save < 0)
		return -1; // selected item is a toc/bod/etc, so it's not in the view->plan

	int pIndex = selected; 
	for (int z=0; z<selected; z++) 
		if (plantable[z].index<0) // decrement count for any preceeding toc/bod/etc
			pIndex--;
	ASSERT(view->plan[pIndex] == save);
	return pIndex;
}

// convert the view->plan index into a current selection index
int FlightPlan::planToSel(int pIndex) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();

	int save = view->plan[pIndex];
	int selected = pIndex; 
	int add = 0;
	for (int z=0; z<=pIndex+add; z++) 
		if (plantable[z].index<0) // increment count for any preceeding toc/bod/etc
			add++;
	selected += add;
	ASSERT(plantable[selected].index == save);
	return selected;
}

// Merge the selected DP/STAR route into the current plan
void FlightPlan::mergeDpStar(CString icao, CString route, int isStar) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	// check to see if the current dp or star was selected again
	if (!isStar && (route == dpName))
		return;
	if (isStar && (route == starName))
		return;

	// find range of waypoints to delete
	int start; // first waypoint to delete
	int end;   // first waypoint to keep after the deletion 
	if (!isStar) {
		start = 1;
		if (wptDp == -1)
			end = start;
		else
			end = selToPlan(wptDp) + 1;
	} else {
		end = selToPlan(plantable.GetSize() - 1);
		if (wptStar == -1)
			start = end;
		else
			start = selToPlan(wptStar);
	}

	// delete old route
	for (int j=start; j<end; j++) {
		view->windtable.RemoveKey(plantable[start].index);
		if ((view->flyToWaypoint-1) >= start)
			view->stopFlyTo();
		for (int i=start+1; i<view->plancount; i++) 
			view->plan[i-1] = view->plan[i];
		view->plancount -=1;
	}

	// find the correct route
	if (route != "None") {
		DpStarAirport *dpDpAirport = doc->dpStar.lookupAirport(icao);
		DpStarRoute* dpStarRoute;
		if (!isStar) 
			dpStarRoute = dpDpAirport->firstDP();
		else
			dpStarRoute = dpDpAirport->firstSTAR();
		while (dpStarRoute != NULL) {
			if (dpStarRoute->name == route)
				break;
			if (!isStar) 
				dpStarRoute = dpDpAirport->nextDP();
			else
				dpStarRoute = dpDpAirport->nextSTAR();
		}

		// insert waypoints
		DpStarWaypoint *w = dpStarRoute->firstWaypoint();
		while (w != NULL) {
			// see if the waypoint is in the database (prefer VORs if more than one match)
			int index = doc->findNearbyNavaid(w->latitude, w->longitude, w->name, 3);
			if (index == -1) { // no, must create it
				doc->OnMapInsertgpsfix(w->latitude, w->longitude, w->name, w->name);
				index = doc->dbCount-1;
			}
			// move existing plan items to make room
			for (int k=view->plancount-1; k>=start; k--)
				view->plan[k+1] = view->plan[k];
			view->plancount = view->plancount + 1;

			view->plan[start] = index;
			start++;
			w = dpStarRoute->nextWaypoint();
		}	
	}

	update(); 
	sel = isStar ? plantable.GetSize()-1: 0;
	view->Invalidate(TRUE);
}

