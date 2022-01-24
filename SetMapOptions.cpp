// SetMapOptions.cpp : implementation file
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
#include "SetMapOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetMapOptions dialog


SetMapOptions::SetMapOptions(CWnd* pParent /*=NULL*/)
	: CDialog(SetMapOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetMapOptions)
	m_blue = 0;
	m_green = 0;
	m_red = 0;
	m_mag = 0;
	//}}AFX_DATA_INIT
}


void SetMapOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetMapOptions)
	DDX_Text(pDX, IDC_BLUE, m_blue);
	DDV_MinMaxByte(pDX, m_blue, 0, 255);
	DDX_Text(pDX, IDC_GREEN, m_green);
	DDV_MinMaxByte(pDX, m_green, 0, 255);
	DDX_Text(pDX, IDC_RED, m_red);
	DDV_MinMaxByte(pDX, m_red, 0, 255);
	DDX_Text(pDX, IDC_MAG, m_mag);
	DDV_MinMaxInt(pDX, m_mag, 0, 1000000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SetMapOptions, CDialog)
	//{{AFX_MSG_MAP(SetMapOptions)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetMapOptions message handlers

void SetMapOptions::OnClose() {
	OnOK();
}

void SetMapOptions::OnChange() {
	CHOOSECOLOR c;
	COLORREF custom[16];
    c.lStructSize = sizeof(CHOOSECOLOR);     
	c.hwndOwner = NULL; 
	c.hInstance = NULL;
	c.lpCustColors = custom;
	c.rgbResult = RGB(m_red, m_green, m_blue); 
	c.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT | CC_SOLIDCOLOR;
	
	if (ChooseColor(&c)) {
		m_red =   GetRValue(c.rgbResult);
		m_green = GetGValue(c.rgbResult);
		m_blue =  GetBValue(c.rgbResult);
		CEdit *r = (CEdit *) GetDlgItem(IDC_RED);
		CEdit *g = (CEdit *) GetDlgItem(IDC_GREEN);
		CEdit *b = (CEdit *) GetDlgItem(IDC_BLUE);
		char buf[20];
		sprintf(buf, "%d", m_red);
		r->SetWindowText(buf);
		sprintf(buf, "%d", m_green);
		g->SetWindowText(buf);
		sprintf(buf, "%d", m_blue);
		b->SetWindowText(buf);
	}
}

