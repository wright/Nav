//MapOptions.cpp:implementationfile
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
#include "MapOptions.h"
#include "SetMapOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//MapOptionsdialog


MapOptions::MapOptions(CWnd*pParent/*=NULL*/)
	:CDialog(MapOptions::IDD,pParent)
{
	//{{AFX_DATA_INIT(MapOptions)
	m_decimation=0;
	m_showall = FALSE;
	//}}AFX_DATA_INIT
}


void MapOptions::DoDataExchange(CDataExchange*pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MapOptions)
	DDX_Text(pDX,IDC_DECIMATION,m_decimation);
	DDV_MinMaxInt(pDX,m_decimation,0,2048);
	DDX_Check(pDX, IDC_SHOWALL, m_showall);
	//}}AFX_DATA_MAP
	update();
}


BEGIN_MESSAGE_MAP(MapOptions,CDialog)
	//{{AFX_MSG_MAP(MapOptions)
	ON_BN_CLICKED(IDC_CIL1,OnCil1)
	ON_BN_CLICKED(IDC_CIL2,OnCil2)
	ON_BN_CLICKED(IDC_CIL3,OnCil3)
	ON_BN_CLICKED(IDC_CIL4,OnCil4)
	ON_BN_CLICKED(IDC_CIL6,OnCil6)
	ON_BN_CLICKED(IDC_CIL7,OnCil7)
	ON_BN_CLICKED(IDC_CIL8,OnCil8)
	ON_BN_CLICKED(IDC_CIL9,OnCil9)
	ON_BN_CLICKED(IDC_CIL10,OnCil10)
	ON_BN_CLICKED(IDC_CIL13,OnCil13)
	ON_BN_CLICKED(IDC_CIL14,OnCil14)
	ON_BN_CLICKED(IDC_CIL15,OnCil15)
	ON_BN_CLICKED(IDC_IPB1,OnIpb1)
	ON_BN_CLICKED(IDC_IPB2,OnIpb2)
	ON_BN_CLICKED(IDC_IPB3,OnIpb3)
	ON_BN_CLICKED(IDC_NPB1,OnNpb1)
	ON_BN_CLICKED(IDC_NPB2,OnNpb2)
	ON_BN_CLICKED(IDC_NPB3,OnNpb3)
	ON_BN_CLICKED(IDC_RIV1,OnRiv1)
	ON_BN_CLICKED(IDC_RIV2,OnRiv2)
	ON_BN_CLICKED(IDC_RIV3,OnRiv3)
	ON_BN_CLICKED(IDC_RIV4,OnRiv4)
	ON_BN_CLICKED(IDC_RIV5,OnRiv5)
	ON_BN_CLICKED(IDC_RIV6,OnRiv6)
	ON_BN_CLICKED(IDC_RIV7,OnRiv7)
	ON_BN_CLICKED(IDC_RIV8,OnRiv8)
	ON_BN_CLICKED(IDC_RIV10,OnRiv10)
	ON_BN_CLICKED(IDC_RIV11,OnRiv11)
	ON_BN_CLICKED(IDC_RIV12,OnRiv12)
	ON_BN_CLICKED(IDC_SHOWALL, OnShowall)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//MapOptionsmessagehandlers

void MapOptions::OnIpb1() {
	edit(1);
}

void MapOptions::OnIpb2() {
	edit(2);
}

void MapOptions::OnIpb3() {
	edit(3);
}

void MapOptions::OnNpb1() {
	edit(16+1);
}

void MapOptions::OnNpb2() {
	edit(16+2);
}

void MapOptions::OnNpb3() {
	edit(16+3);
}

void MapOptions::OnCil1() {
	edit(32+1);
}

void MapOptions::OnCil2() {	
	edit(32+2);
}

void MapOptions::OnCil3() {
	edit(32+3);
}

void MapOptions::OnCil4() {
	edit(32+4);
}

void MapOptions::OnCil6(){
	edit(32+6);
}

void MapOptions::OnCil7() {
	edit(32+7);
}

void MapOptions::OnCil8() {
	edit(32+8);
}

void MapOptions::OnCil9() {
	edit(32+9);
}

void MapOptions::OnCil10() {
	edit(32+10);
}

void MapOptions::OnCil13() {
	edit(32+13);
}

void MapOptions::OnCil14() {
	edit(32+14);
}

void MapOptions::OnCil15() {
	edit(32+15);
}

void MapOptions::OnRiv1() {
	edit(48+1);
}

void MapOptions::OnRiv2() {
	edit(48+2);
}

void MapOptions::OnRiv3() {
	edit(48+3);
}

void MapOptions::OnRiv4() {
	edit(48+4);
}

void MapOptions::OnRiv5() {
	edit(48+5);
}

void MapOptions::OnRiv6() {
	edit(48+6);
}

void MapOptions::OnRiv7() {
	edit(48+7);
}

void MapOptions::OnRiv8() {
	edit(48+8);
}

void MapOptions::OnRiv10() {
	edit(48+10);
}

void MapOptions::OnRiv11() {
	edit(48+11);
}

void MapOptions::OnRiv12() {
	edit(48+12);
}

void MapOptions::edit(int index) {
	SetMapOptions d;

	d.m_mag = v[index].mag;
	d.m_red = v[index].r;
	d.m_green = v[index].g;
	d.m_blue = v[index].b;
	if (d.DoModal() == IDOK) {
		v[index].mag = d.m_mag;
		v[index].r = d.m_red;
		v[index].g = d.m_green;
		v[index].b = d.m_blue;
	}
}

void MapOptions::OnClose() {
	OnOK();
}

void MapOptions::OnShowall() {
	m_showall = !m_showall;
	update();	
}

void MapOptions::update() {
	GetDlgItem(IDC_DECIMATION)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL1)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL2)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL3)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL4)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL6)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL7)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL8)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL9)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL10)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL13)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL14)->EnableWindow(!m_showall);
	GetDlgItem(IDC_CIL15)->EnableWindow(!m_showall);
	GetDlgItem(IDC_IPB1)->EnableWindow(!m_showall);
	GetDlgItem(IDC_IPB2)->EnableWindow(!m_showall);
	GetDlgItem(IDC_IPB3)->EnableWindow(!m_showall);
	GetDlgItem(IDC_NPB1)->EnableWindow(!m_showall);
	GetDlgItem(IDC_NPB2)->EnableWindow(!m_showall);
	GetDlgItem(IDC_NPB3)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV1)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV2)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV3)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV4)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV5)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV6)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV7)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV8)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV10)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV11)->EnableWindow(!m_showall);
	GetDlgItem(IDC_RIV12)->EnableWindow(!m_showall);
}
