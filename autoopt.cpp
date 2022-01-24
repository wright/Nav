// autoopt.cpp : implementation file
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
#include "autoopt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AutoOpt dialog


AutoOpt::AutoOpt(CWnd* pParent /*=NULL*/)
	: CDialog(AutoOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(AutoOpt)
	m_airports = FALSE;
	m_continue = FALSE;
	m_ideal = 0.0;
	m_ndb = FALSE;
	m_vor = FALSE;
	m_waypoints = FALSE;
	//}}AFX_DATA_INIT
}


void AutoOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AutoOpt)
	DDX_Check(pDX, IDC_AIRPORTS, m_airports);
	DDX_Check(pDX, IDC_CONTINUE, m_continue);
	DDX_Text(pDX, IDC_IDEAL, m_ideal);
	DDV_MinMaxDouble(pDX, m_ideal, 1., 10000.);
	DDX_Check(pDX, IDC_NDB, m_ndb);
	DDX_Check(pDX, IDC_VOR, m_vor);
	DDX_Check(pDX, IDC_WAYPOINTS, m_waypoints);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AutoOpt, CDialog)
	//{{AFX_MSG_MAP(AutoOpt)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// AutoOpt message handlers
