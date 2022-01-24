// serbgl.cpp : implementation file
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
#include "serbgl.h"
#include "folder_dialog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SerBgl dialog


SerBgl::SerBgl(CWnd* pParent /*=NULL*/)
	: CDialog(SerBgl::IDD, pParent)
{
	//{{AFX_DATA_INIT(SerBgl)
	m_recurse = FALSE;
	m_drive = _T("");
	m_skip = FALSE;
	m_useAFD = FALSE;
	//}}AFX_DATA_INIT
}


void SerBgl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SerBgl)
	DDX_Check(pDX, IDC_CHECK1, m_recurse);
	DDX_Text(pDX, IDC_DRIVE, m_drive);
	DDV_MaxChars(pDX, m_drive, 1000);
	DDX_Check(pDX, IDC_CHECK2, m_skip);
	DDX_Check(pDX, IDC_CHECK3, m_useAFD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SerBgl, CDialog)
	//{{AFX_MSG_MAP(SerBgl)
	ON_BN_CLICKED(IDC_CHOOSE, OnChoose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SerBgl message handlers

void SerBgl::OnChoose() {
	UpdateData(TRUE);
	char currentDir[260];
	GetCurrentDirectory(260, currentDir);
	SetCurrentDirectory(m_drive);
    CString pathSelected = m_drive;
    CFolderDialog dlg(&pathSelected);
    dlg.m_ofn.lpstrTitle = _T("Scenery directory...");
    if (dlg.DoModal() == IDOK) 
		m_drive = pathSelected; 
	SetCurrentDirectory(currentDir);
	UpdateData(FALSE);
}
