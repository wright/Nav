// CreateDpStar.cpp : implementation file
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
#include "nav.h"
#include "CreateDpStar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CreateDpStar dialog


CreateDpStar::CreateDpStar(CWnd* pParent /*=NULL*/)
	: CDialog(CreateDpStar::IDD, pParent)
{
	//{{AFX_DATA_INIT(CreateDpStar)
	m_prompt = _T("");
	m_routename = _T("");
	//}}AFX_DATA_INIT
}


void CreateDpStar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CreateDpStar)
	DDX_Text(pDX, IDC_PROMPT, m_prompt);
	DDX_Text(pDX, IDC_ROUTENAME, m_routename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CreateDpStar, CDialog)
	//{{AFX_MSG_MAP(CreateDpStar)
	ON_EN_CHANGE(IDC_ROUTENAME, OnChangeRoutename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CreateDpStar message handlers

void CreateDpStar::OnChangeRoutename()  {
	CString text;
	GetDlgItemText(IDC_ROUTENAME, text); 
	CButton *okButton = (CButton *) GetDlgItem(IDOK);
		okButton->EnableWindow(text.GetLength() > 0);
}

BOOL CreateDpStar::OnInitDialog() {
	CDialog::OnInitDialog();
	if (m_prompt.Find("STAR") == -1)
		this->SetWindowText("Create Departure Procedure");
	else
		this->SetWindowText("Create Standard Approach Route");
	CButton *okButton = (CButton *) GetDlgItem(IDOK);
		okButton->EnableWindow(FALSE);
	
	return TRUE;  
}
