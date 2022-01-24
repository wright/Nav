// CenterAt.cpp : implementation file
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
#include "CenterAt.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCenterAt dialog


CCenterAt::CCenterAt(CWnd* pParent /*=NULL*/)
	: CDialog(CCenterAt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCenterAt)
	m_lat = 0.0;
	m_lon = 0.0;
	m_latd = 0;
	m_latm = 0;
	m_lats = 0.0;
	m_lond = 0;
	m_lonm = 0;
	m_lons = 0.0;
	//}}AFX_DATA_INIT
}


void CCenterAt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCenterAt)
	DDX_Text(pDX, IDC_DDEGLAT, m_lat);
	DDV_MinMaxDouble(pDX, m_lat, -90., 90.);
	DDX_Text(pDX, IDC_DDEGLON, m_lon);
	DDV_MinMaxDouble(pDX, m_lon, -180., 180.);
	DDX_Text(pDX, IDC_LATD, m_latd);
	DDV_MinMaxInt(pDX, m_latd, -90, 90);
	DDX_Text(pDX, IDC_LATM, m_latm);
	DDV_MinMaxInt(pDX, m_latm, 0, 59);
	DDX_Text(pDX, IDC_LATS, m_lats);
	DDV_MinMaxDouble(pDX, m_lats, 0., 60.);
	DDX_Text(pDX, IDC_LOND, m_lond);
	DDV_MinMaxInt(pDX, m_lond, -180, 180);
	DDX_Text(pDX, IDC_LONM, m_lonm);
	DDV_MinMaxInt(pDX, m_lonm, 0, 59);
	DDX_Text(pDX, IDC_LONS, m_lons);
	DDV_MinMaxDouble(pDX, m_lons, 0., 60.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCenterAt, CDialog)
	//{{AFX_MSG_MAP(CCenterAt)
	ON_BN_DOUBLECLICKED(IDC_RADIODD, OnDoubleclickedRadiodd)
	ON_BN_DOUBLECLICKED(IDC_RADIODMSD, OnDoubleclickedRadiodmsd)
	ON_EN_UPDATE(IDC_DDEGLAT, OnUpdateDdeglat)
	ON_EN_UPDATE(IDC_DDEGLON, OnUpdateDdeglon)
	ON_EN_UPDATE(IDC_LATD, OnUpdateLatd)
	ON_EN_UPDATE(IDC_LATM, OnUpdateLatm)
	ON_EN_UPDATE(IDC_LOND, OnUpdateLond)
	ON_EN_UPDATE(IDC_LONM, OnUpdateLonm)
	ON_EN_UPDATE(IDC_LONS, OnUpdateLons)
	ON_EN_UPDATE(IDC_LATS, OnUpdateLats)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCenterAt message handlers

BOOL CCenterAt::OnInitDialog() {
	double rem;

	m_latd = (int) m_lat;
	rem = fabs(m_lat - m_latd);
	m_latm = (int) (60*rem);
	rem = 60*rem - m_latm;
	m_lats = 60 * rem;

	m_lond = (int) m_lon;
	rem = fabs(m_lon - m_lond);
	m_lonm = (int) (60*rem);
	rem = 60*rem - m_lonm;
	m_lons = 60 * rem;

	CButton *pRad;
	pRad = (CButton *) GetDlgItem(IDC_RADIODD);
	pRad->SetCheck(1);

	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCenterAt::OnOK() {
	CDialog::OnOK();

	CButton *pRad;
	pRad = (CButton *) GetDlgItem(IDC_RADIODD);
	int res = pRad->GetCheck();
	if (res != 1) {
		double rem;
		rem = (m_latm + (m_lats / 60.0)) / 60.0;
		if (m_latd >= 0)
			m_lat = m_latd + rem;
		else
			m_lat = m_latd - rem;

		rem = (m_lonm + (m_lons / 60.0)) / 60.0;
		if (m_lond >= 0)
			m_lon = m_lond + rem;
		else
			m_lon = m_lond - rem;
	}
}

void CCenterAt::OnDoubleclickedRadiodd() {
	OnOK();	
}

void CCenterAt::OnDoubleclickedRadiodmsd() {
	OnOK();	
}

void CCenterAt::OnUpdateDdeglat() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(0);
}

void CCenterAt::OnUpdateDdeglon() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(1);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(0);
}

void CCenterAt::OnUpdateLatd() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(1);
}

void CCenterAt::OnUpdateLatm() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(1);
}

void CCenterAt::OnUpdateLats() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(1);
}

void CCenterAt::OnUpdateLond() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(1);
}

void CCenterAt::OnUpdateLonm() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(1);
}

void CCenterAt::OnUpdateLons() {
	((CButton *) GetDlgItem(IDC_RADIODD))->SetCheck(0);
	((CButton *) GetDlgItem(IDC_RADIODMSD))->SetCheck(1);
}
