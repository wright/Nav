// InsertGPSDialog.cpp : implementation file
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
#include "InsertGPSDialog.h"
#include <afxtempl.h>
#include "mainfrm.h"
#include "Dpstar.h"
#include "navdoc.h"
#include "flightpl.h"
#include "navview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// InsertGPSDialog dialog


InsertGPSDialog::InsertGPSDialog(CWnd* pParent /*=NULL*/)
	: CDialog(InsertGPSDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(InsertGPSDialog)
	m_id = _T("");
	m_name = _T("");
	//}}AFX_DATA_INIT
}


void InsertGPSDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(InsertGPSDialog)
	DDX_Text(pDX, IDC_ID, m_id);
	DDV_MaxChars(pDX, m_id, 5);
	DDX_Text(pDX, IDC_NAME, m_name);
	DDV_MaxChars(pDX, m_name, 26);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(InsertGPSDialog, CDialog)
	//{{AFX_MSG_MAP(InsertGPSDialog)
	ON_BN_CLICKED(ID_SENDTOPLAN, OnSendtoplan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// InsertGPSDialog message handlers

void InsertGPSDialog::OnSendtoplan() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	UpdateData(TRUE);
	int newIndex = doc->OnMapInsertgpsfix(lat, lon, m_name, m_id);
	view->SendToPlan(newIndex);
	view->Invalidate(TRUE);
	CDialog::OnCancel();
}

void InsertGPSDialog::OnOK() {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();

	UpdateData(TRUE);
	doc->OnMapInsertgpsfix(lat, lon, m_name, m_id);
	view->Invalidate(TRUE);
	CDialog::OnOK();
}
