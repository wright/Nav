// Aircraft.cpp : implementation file
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
#include "Aircraft.h"
#include "LoadAircraft.h"
#include "mainfrm.h"
#include "Dpstar.h"
#include "Navdoc.h"
#include "flightpl.h" 
#include "Navview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Aircraft dialog


Aircraft::Aircraft(CWnd* pParent /*=NULL*/)
	: CDialog(Aircraft::IDD, pParent)
{
	//{{AFX_DATA_INIT(Aircraft)
	m_craft = _T("");
	m_cruisespeed = 0;
	m_descentspeed = 0;
	m_climbspeed = 0;
	m_descentfuel = 0.0;
	m_cruisefuel = 0.0;
	m_climbfuel = 0.0;
	m_climbrate = 0;
	m_descentrate = 0;
	m_cruisealtitude = 0;
	//}}AFX_DATA_INIT
}


void Aircraft::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Aircraft)
	DDX_Text(pDX, IDC_CRAFT, m_craft);
	DDX_Text(pDX, IDC_CS, m_cruisespeed);
	DDV_MinMaxInt(pDX, m_cruisespeed, 0, 5000);
	DDX_Text(pDX, IDC_DS, m_descentspeed);
	DDV_MinMaxInt(pDX, m_descentspeed, 0, 5000);
	DDX_Text(pDX, IDC_US, m_climbspeed);
	DDV_MinMaxInt(pDX, m_climbspeed, 0, 5000);
	DDX_Text(pDX, IDC_DF, m_descentfuel);
	DDV_MinMaxDouble(pDX, m_descentfuel, 0., 1000000.);
	DDX_Text(pDX, IDC_CF, m_cruisefuel);
	DDV_MinMaxDouble(pDX, m_cruisefuel, 0., 1000000.);
	DDX_Text(pDX, IDC_UF, m_climbfuel);
	DDV_MinMaxDouble(pDX, m_climbfuel, 0., 1000000.);
	DDX_Text(pDX, IDC_UR, m_climbrate);
	DDV_MinMaxInt(pDX, m_climbrate, 0, 32000);
	DDX_Text(pDX, IDC_DR, m_descentrate);
	DDV_MinMaxInt(pDX, m_descentrate, 0, 32000);
	DDX_Text(pDX, IDC_CA, m_cruisealtitude);
	DDV_MinMaxLong(pDX, m_cruisealtitude, 0, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Aircraft, CDialog)
	//{{AFX_MSG_MAP(Aircraft)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_SAVEAS, OnSaveas)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Aircraft message handlers

void Aircraft::OnLoad() {
	LoadAircraft d(this);
	d.selected = m_craft;
	if (d.DoModal() == IDOK) {
		m_craft = d.selected;
		WritePrivateProfileString("Plan", "lastCraft", (LPCTSTR) m_craft, "nav.ini");
		char buf[20];  // read craft characteristics
		GetPrivateProfileString((LPCTSTR) m_craft, "cruisealtitude", "9000", buf, 20, "nav.ini");
		sscanf(buf, "%ld", &m_cruisealtitude);
		GetPrivateProfileString((LPCTSTR) m_craft, "climbfuel", "14.1", buf, 20, "nav.ini");
		sscanf(buf, "%lf", &m_climbfuel);
		GetPrivateProfileString((LPCTSTR) m_craft, "cruisefuel", "14.1", buf, 20, "nav.ini");
		sscanf(buf, "%lf", &m_cruisefuel);
		GetPrivateProfileString((LPCTSTR) m_craft, "descentfuel", "14.1", buf, 20, "nav.ini");
		sscanf(buf, "%lf", &m_descentfuel);
		GetPrivateProfileString((LPCTSTR) m_craft, "climbspeed", "76", buf, 20, "nav.ini");
		sscanf(buf, "%d", &m_climbspeed);
		GetPrivateProfileString((LPCTSTR) m_craft, "cruisespeed", "140", buf, 20, "nav.ini");
		sscanf(buf, "%d", &m_cruisespeed);
		GetPrivateProfileString((LPCTSTR) m_craft, "descentspeed", "140", buf, 20, "nav.ini");
		sscanf(buf, "%d", &m_descentspeed);
		GetPrivateProfileString((LPCTSTR) m_craft, "climbrate", "1500", buf, 20, "nav.ini");
		sscanf(buf, "%d", &m_climbrate);
		GetPrivateProfileString((LPCTSTR) m_craft, "descentrate", "1500", buf, 20, "nav.ini");
		sscanf(buf, "%d", &m_descentrate);
		UpdateData(FALSE);
	}
}

void Aircraft::OnSaveas() {
	UpdateData(TRUE);

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	view->addCraft(m_craft, m_climbspeed, (int)m_climbfuel, m_climbrate, m_cruisespeed, 
		(int)m_cruisefuel, m_cruisealtitude, m_descentspeed, (int)m_descentfuel, m_descentrate);
}

BOOL Aircraft::OnInitDialog() {
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Aircraft::OnOK() {
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
