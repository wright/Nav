// Export.cpp : implementation file
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
#include "Export.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExport dialog


CExport::CExport(CWnd* pParent /*=NULL*/)
	: CDialog(CExport::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExport)
	m_center = _T("");
	m_state = _T("");
	m_headings = FALSE;
	//}}AFX_DATA_INIT
}


void CExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExport)
	DDX_Text(pDX, IDC_CENTER, m_center);
	DDV_MaxChars(pDX, m_center, 12);
	DDX_Text(pDX, IDC_STATE, m_state);
	DDV_MaxChars(pDX, m_state, 2);
	DDX_Check(pDX, IDC_HEADINGS, m_headings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExport, CDialog)
	//{{AFX_MSG_MAP(CExport)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExport message handlers
