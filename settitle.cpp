// settitle.cpp : implementation file
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
#include "settitle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetTitle dialog


SetTitle::SetTitle(CWnd* pParent /*=NULL*/)
	: CDialog(SetTitle::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetTitle)
	m_title = _T("");
	//}}AFX_DATA_INIT
}


void SetTitle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetTitle)
	DDX_Text(pDX, IDC_TITLE, m_title);
	DDV_MaxChars(pDX, m_title, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SetTitle, CDialog)
	//{{AFX_MSG_MAP(SetTitle)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SetTitle message handlers
