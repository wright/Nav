// mainfrm.cpp : implementation of the CMainFrame class
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

#include "mainfrm.h"

#include <afxtempl.h>
#include "Dpstar.h"
#include "Navdoc.h"
#include "flightpl.h" 
#include "Navview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars
	
// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_FILE_OPEN,
		ID_SEPARATOR,
	ID_FILE_PRINT,
		ID_SEPARATOR,
	ID_VIEW_ZOOMIN,
	ID_VIEW_ZOOMOUT,
		ID_SEPARATOR,
	ID_VIEW_VOR,
	ID_VIEW_ILS,
	ID_VIEW_NDB,
	ID_VIEW_AIRPORTS,
	ID_VIEW_MARKERS,
	ID_MAP_ATIS,
	ID_VIEW_WAYPOINTS,
	ID_VIEW_ROUTES,
		ID_SEPARATOR,
	ID_VIEW_ID,
	ID_VIEW_FREQUENCY,
	ID_MAP_SHOW_PLANDATAONLY,
		ID_SEPARATOR,
	ID_VIEW_DATABASE,
	ID_VIEW_SEARCH,
	ID_EDIT_FLIGHTPLAN,
		ID_SEPARATOR,
	ID_FS6_LINK,
	ID_FS6_FLYTO
};

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
		!m_wndToolBar.SetButtons(buttons,
		  sizeof(buttons)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::SetStatusBar(CString s) {
	m_wndStatusBar.SetWindowText(s);
	m_wndStatusBar.UpdateWindow();
}

void CMainFrame::OnClose() {
	CNavView *view = (CNavView *) GetActiveView() ;
	if (view != NULL) {
		if (view->drawing) { // don't process close when drawing
			view->abort = TRUE;
			// re-send the close message twice (doesn't close if you only send once)
			PostMessage(WM_CLOSE);
			PostMessage(WM_CLOSE);
			return;
		}
	}
	CFrameWnd::OnClose();
}
