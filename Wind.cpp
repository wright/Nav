// Wind.cpp : implementation file
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
#include "Wind.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Wind dialog


Wind::Wind(CWnd* pParent /*=NULL*/)
	: CDialog(Wind::IDD, pParent)
{
	//{{AFX_DATA_INIT(Wind)
	m_direction = 0;
	m_speed = 0;
	m_global = FALSE;
	//}}AFX_DATA_INIT
}


void Wind::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Wind)
	DDX_Text(pDX, IDC_DIRECTION, m_direction);
	DDV_MinMaxInt(pDX, m_direction, 0, 359);
	DDX_Text(pDX, IDC_SPEED, m_speed);
	DDV_MinMaxInt(pDX, m_speed, 0, 359);
	DDX_Check(pDX, IDC_GLOBAL, m_global);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Wind, CDialog)
	//{{AFX_MSG_MAP(Wind)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Wind message handlers

BOOL Wind::OnInitDialog() {
	CDialog::OnInitDialog();
	CButton *button = (CButton *) GetDlgItem(IDC_GLOBAL);
	
	if (noselection == TRUE) {
		m_global = TRUE;
		button->SetCheck(TRUE);
		button->EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
