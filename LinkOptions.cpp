// LinkOptions.cpp : implementation file
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
#include "LinkOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinkOptions dialog


LinkOptions::LinkOptions(CWnd* pParent /*=NULL*/)
	: CDialog(LinkOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(LinkOptions)
	m_flysetmode = FALSE;
	m_enablenetwork = FALSE;
	m_flyupdate = 0.0;
	m_update = 0.0;
	m_callsign = _T("");
	//}}AFX_DATA_INIT
}


void LinkOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LinkOptions)
	DDX_Check(pDX, IDC_SETNAVMODE, m_flysetmode);
	DDX_Check(pDX, IDC_ENABLENETWORK, m_enablenetwork);
	DDX_Text(pDX, IDC_FLYUPDATE, m_flyupdate);
	DDV_MinMaxDouble(pDX, m_flyupdate, 0., 3600.);
	DDX_Text(pDX, IDC_UPDATE, m_update);
	DDV_MinMaxDouble(pDX, m_update, 0., 3600.);
	DDX_Text(pDX, IDC_CALLSIGN, m_callsign);
	DDV_MaxChars(pDX, m_callsign, 30);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LinkOptions, CDialog)
	//{{AFX_MSG_MAP(LinkOptions)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LinkOptions message handlers
