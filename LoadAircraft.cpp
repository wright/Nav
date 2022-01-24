// LoadAircraft.cpp : implementation file
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
#include "LoadAircraft.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LoadAircraft dialog


LoadAircraft::LoadAircraft(CWnd* pParent /*=NULL*/)
	: CDialog(LoadAircraft::IDD, pParent)
{
	//{{AFX_DATA_INIT(LoadAircraft)
	//}}AFX_DATA_INIT
}


void LoadAircraft::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LoadAircraft)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LoadAircraft, CDialog)
	//{{AFX_MSG_MAP(LoadAircraft)
	ON_LBN_DBLCLK(IDC_CRAFT, OnDblclkCraft)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LoadAircraft message handlers

BOOL LoadAircraft::OnInitDialog() {
	CDialog::OnInitDialog();
	CListBox *list = (CListBox *) GetDlgItem(IDC_CRAFT);
	
	char buf[50];
	int craftNumber	= GetPrivateProfileInt("Plan", "craftnumber", 0, "nav.ini");
	if (craftNumber < 1) return TRUE;
	for (int i=1; i<=craftNumber; i++) {
		sprintf(buf, "%d", i);
		CString newcraft = CString("craft") + buf;
		GetPrivateProfileString("Plan", (LPCTSTR) newcraft, "%$#@!42", buf, 50, "nav.ini");
		if (CString(buf) == "%$#@!42") continue; // bad read, don't name any aircraft "%$#@!42"
		list->AddString(buf);
	}	
	list->SetCurSel(list->FindStringExact(-1, (LPCTSTR) selected)); 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void LoadAircraft::OnOK() {
	CListBox *list = (CListBox *) GetDlgItem(IDC_CRAFT);
	list->GetText(list->GetCurSel(), selected);
	
	CDialog::OnOK();
}

void LoadAircraft::OnDblclkCraft() {
	OnOK();	
}
