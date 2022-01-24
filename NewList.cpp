// NewList.cpp : implementation file
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
#include "NewList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NewList dialog


NewList::NewList(CWnd* pParent /*=NULL*/)
	: CDialog(NewList::IDD, pParent)
{
	//{{AFX_DATA_INIT(NewList)
	m_name = _T("");
	//}}AFX_DATA_INIT
}


void NewList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NewList)
	DDX_Text(pDX, IDC_NAME, m_name);
	DDV_MaxChars(pDX, m_name, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NewList, CDialog)
	//{{AFX_MSG_MAP(NewList)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NewList message handlers
