// Transition.cpp : implementation file
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
#include "Transition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Transition dialog


Transition::Transition(CWnd* pParent /*=NULL*/)
	: CDialog(Transition::IDD, pParent)
{
	//{{AFX_DATA_INIT(Transition)
	m_trans = 0;
	//}}AFX_DATA_INIT
}


void Transition::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Transition)
	DDX_Text(pDX, IDC_TRANS, m_trans);
	DDV_MinMaxInt(pDX, m_trans, 1, 100);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Transition, CDialog)
	//{{AFX_MSG_MAP(Transition)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Transition message handlers
