// FTDensity.cpp : implementation file
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
#include "FTDensity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFTDensity dialog


CFTDensity::CFTDensity(CWnd* pParent /*=NULL*/)
	: CDialog(CFTDensity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFTDensity)
	m_density = 0;
	//}}AFX_DATA_INIT
}


void CFTDensity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFTDensity)
	DDX_Text(pDX, IDC_DENSITY, m_density);
	DDV_MinMaxInt(pDX, m_density, 1, 600);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFTDensity, CDialog)
	//{{AFX_MSG_MAP(CFTDensity)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFTDensity message handlers
