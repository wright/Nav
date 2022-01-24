// details.cpp : implementation file
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
#include "details.h"
#include "mainfrm.h"
#include "Dpstar.h"
#include "navdoc.h"
#include "flightpl.h"
#include "navview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Details dialog


Details::Details(CWnd* pParent /*=NULL*/)
	: CDialog(Details::IDD, pParent)
{
	//{{AFX_DATA_INIT(Details)
	m_text = _T("");
	//}}AFX_DATA_INIT
}


void Details::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Details)
	DDX_Text(pDX, IDC_TEXT, m_text);
	//}}AFX_DATA_MAP

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	if ((doc->db[index].type == 11)||(doc->db[index].type == 12))
		GetDlgItem(IDC_PLAN)->EnableWindow(FALSE);
	if ((doc->db[index].type == 11)||(doc->db[index].type == 12))
		GetDlgItem(IDC_CENTER)->EnableWindow(FALSE);
}


BEGIN_MESSAGE_MAP(Details, CDialog)
	//{{AFX_MSG_MAP(Details)
	ON_BN_CLICKED(IDC_PLAN, OnPlan)
	ON_BN_CLICKED(IDC_CENTER, OnCenter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Details message handlers

void Details::OnPlan() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	
	view->SendToPlan(index);
	CDialog::OnOK();
}


void Details::OnCenter() {	
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	double lat = doc->db[index].lat;
	double lon = doc->db[index].lon;
	if (doc->db[index].type == 14) { // center of route
		lat += doc->db[index].approach;
		lon += doc->db[index].range;
		lat /= 2.0;
		lon /= 2.0;
	} 
	
	view->CenterAt(lat, lon);	
	view->Invalidate(TRUE);
	CDialog::OnOK();
}
