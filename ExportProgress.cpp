// ExportProgress.cpp : implementation file
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
#include "ExportProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ExportProgress dialog


ExportProgress::ExportProgress(CWnd* pParent /*=NULL*/)
	: CDialog(ExportProgress::IDD, pParent)	{
	//{{AFX_DATA_INIT(ExportProgress)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	cancelled = FALSE;
}


void ExportProgress::DoDataExchange(CDataExchange* pDX)	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ExportProgress)
	DDX_Control(pDX, IDC_RUNWAYS, m_runways);
	DDX_Control(pDX, IDC_NAVAIDS, m_navaids);
	DDX_Control(pDX, IDC_AIRPORTS, m_airports);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ExportProgress, CDialog)
	//{{AFX_MSG_MAP(ExportProgress)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ExportProgress message handlers

void ExportProgress::OnCancel() {
	// TODO: Add extra cleanup here
	cancelled = TRUE;
	
	CDialog::OnCancel();
}
