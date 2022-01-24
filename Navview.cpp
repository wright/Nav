// Navview.cpp : implementation of the CNavView class
//
/*
    Copyright (C) 2000 Ted Wright 
    NASA Business: wright@grc.nasa.gov, Remainder: wright@en.com

    Modifications (C) 1998 Larry G. Fortna (fortna@magicnet.net)
    This file was modified to add sections 10 and 15.  Changes
	were also made to allow compilation by Microsoft Developer
	Studio Version 4.0.

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

#include <afxtempl.h>
#include "Dpstar.h"
#include "Navdoc.h"
#include "flightpl.h" 
#include "Navview.h"

#include "math.h"
#include "settitle.h"
#include "datadial.h"
#include "search.h"
#include "mainfrm.h"
#include "details.h"
#include "autoopt.h"
#include "direct.h"
#include "FTDensity.h"
#include "Transition.h"
#include <afxdlgs.h>
#include "CenterAt.h"
#include "LinkOptions.h"
#include "Fs6State.h"
#include "Fs98State.h"
#include "Fs2000State.h"
#include "FS6IPC_Client.h"
#include "InsertGpsFixDial.h"

#define root2		 1.414213562
#define pi			 3.1415946536
#define degToRad     (pi / 180.0)
#define degToMeters  (10001750.0 / 90.0)
#define earthRadius	 (180 * degToMeters / pi)
#define WM_NAV (WM_USER+500)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavView

IMPLEMENT_DYNCREATE(CNavView, CView)

#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(CNavView, CView)
	//{{AFX_MSG_MAP(CNavView)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_WM_LBUTTONDOWN()
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_GRIDLINES, OnViewGridlines)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GRIDLINES, OnUpdateViewGridlines)
	ON_COMMAND(ID_VIEW_NDB, OnViewNdb)
	ON_COMMAND(ID_VIEW_ILS, OnViewIls)
	ON_COMMAND(ID_VIEW_VOR, OnViewVor)
	ON_COMMAND(ID_VIEW_AIRPORTS, OnViewAirports)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AIRPORTS, OnUpdateViewAirports)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ILS, OnUpdateViewIls)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NDB, OnUpdateViewNdb)
	ON_UPDATE_COMMAND_UI(ID_VIEW_VOR, OnUpdateViewVor)
	ON_COMMAND(ID_VIEW_ID, OnViewId)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ID, OnUpdateViewId)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FREQUENCY, OnUpdateViewFrequency)
	ON_COMMAND(ID_VIEW_FREQUENCY, OnViewFrequency)
	ON_COMMAND(ID_EDIT_CHANGEFONT, OnEditChangefont)
	ON_COMMAND(ID_EDIT_SETTITLE, OnEditSettitle)
	ON_COMMAND(ID_VIEW_SEARCH, OnViewSearch)
	ON_COMMAND(ID_VIEW_DATABASE, OnViewDatabase)
	ON_COMMAND(ID_EDIT_SAVEOPTIONS, OnEditSaveoptions)
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_VIEW_FLATWORLD, OnViewFlatworld)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FLATWORLD, OnUpdateViewFlatworld)
	ON_COMMAND(ID_EDIT_FLIGHTPLAN, OnEditFlightplan)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FLIGHTPLAN, OnUpdateEditFlightplan)
	ON_COMMAND(ID_PLAN_PLANOPTIONS, OnPlanPlanoptions)
	ON_COMMAND(ID_MAP_SHOW_PLANDATAONLY, OnMapShowPlandataonly)
	ON_UPDATE_COMMAND_UI(ID_MAP_SHOW_PLANDATAONLY, OnUpdateMapShowPlandataonly)
	ON_COMMAND(ID_VIEW_MARKERS, OnViewMarkers)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MARKERS, OnUpdateViewMarkers)
	ON_COMMAND(ID_MAP_SHOW_PLANINFORMATION, OnMapShowPlaninformation)
	ON_UPDATE_COMMAND_UI(ID_MAP_SHOW_PLANINFORMATION, OnUpdateMapShowPlaninformation)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_UPDATE_COMMAND_UI(ID_MAP_CLEARFLIGHTTRACK, OnUpdateMapClearflighttrack)
	ON_COMMAND(ID_MAP_CLEARFLIGHTTRACK, OnMapClearflighttrack)
	ON_COMMAND(ID_MAP_SETFLIGHTTRACKDENSITY, OnMapSetflighttrackdensity)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT, OnUpdateFileImport)
	ON_COMMAND(ID_MAP_LARGESYMBOLS, OnMapLargesymbols)
	ON_UPDATE_COMMAND_UI(ID_MAP_LARGESYMBOLS, OnUpdateMapLargesymbols)
	ON_COMMAND(ID_MAP_SETTRANSITION, OnMapSettransition)
	ON_COMMAND(ID_MAP_ZOOMTOFIT, OnMapZoomtofit)
	ON_UPDATE_COMMAND_UI(ID_MAP_ZOOMTOFIT, OnUpdateMapZoomtofit)
	ON_COMMAND(ID_MAP_SHOW_MAPSYMBOLS_AIRPORTZONES, OnMapShowMapsymbolsAirportzones)
	ON_UPDATE_COMMAND_UI(ID_MAP_SHOW_MAPSYMBOLS_AIRPORTZONES, OnUpdateMapShowMapsymbolsAirportzones)
	ON_COMMAND(ID_MAP_SHOW_MAPS, OnMapShowMaps)
	ON_UPDATE_COMMAND_UI(ID_MAP_SHOW_MAPS, OnUpdateMapShowMaps)
	ON_COMMAND(ID_MAP_CENTERAT, OnMapCenterat)
	ON_COMMAND(ID_MAP_ATIS, OnMapAtis)
	ON_UPDATE_COMMAND_UI(ID_MAP_ATIS, OnUpdateMapAtis)
	ON_COMMAND(ID_FS6_LINK, OnFs6Link)
	ON_UPDATE_COMMAND_UI(ID_FS6_LINK, OnUpdateFs6Link)
	ON_COMMAND(ID_FS6_SET, OnFs6Set)
	ON_COMMAND(ID_FS6_FS6LINKOPTIONS, OnFs6Fs6linkoptions)
	ON_COMMAND(ID_FS6_FLYTO, OnFs6Flyto)
	ON_UPDATE_COMMAND_UI(ID_FS6_FLYTO, OnUpdateFs6Flyto)
	ON_COMMAND(ID_HELPVIEWDOCS, OnHelpviewdocs)
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_WAYPOINTS, OnViewWaypoints)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WAYPOINTS, OnUpdateViewWaypoints)
	ON_COMMAND(ID_VIEW_ROUTES, OnViewRoutes)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ROUTES, OnUpdateViewRoutes)
	ON_COMMAND(ID_MAP_SHOW_FANCYMAPS, OnMapShowFancymaps)
	ON_UPDATE_COMMAND_UI(ID_MAP_SHOW_FANCYMAPS, OnUpdateMapShowFancymaps)
	ON_COMMAND(ID_SET_FANCYMAP_OPTIONS, OnSetFancymapOptions)
	ON_UPDATE_COMMAND_UI(ID_SET_FANCYMAP_OPTIONS, OnUpdateSetFancymapOptions)
	ON_UPDATE_COMMAND_UI(ID_PLAN0, OnUpdatePlan0)
	ON_COMMAND(ID_PLAN0, OnPlan0)
	ON_COMMAND(ID_FILE_STARTWITHLASTLIST, OnFileStartwithlastlist)
	ON_UPDATE_COMMAND_UI(ID_FILE_STARTWITHLASTLIST, OnUpdateFileStartwithlastlist)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_MAP_STARTWITHLASTVIEW, OnMapStartwithlastview)
	ON_UPDATE_COMMAND_UI(ID_MAP_STARTWITHLASTVIEW, OnUpdateMapStartwithlastview)
	ON_COMMAND(ID_FLIGHTSIM_MAPFOLLOWSAIRCRAFT_NONE, OnFlightsimMapfollowsaircraftNone)
	ON_UPDATE_COMMAND_UI(ID_FLIGHTSIM_MAPFOLLOWSAIRCRAFT_NONE, OnUpdateFlightsimMapfollowsaircraftNone)
	ON_COMMAND(ID_FLIGHTSIM_MAPFOLLOWSAIRCRAFT_LOCAL, OnFlightsimMapfollowsaircraftLocal)
	ON_UPDATE_COMMAND_UI(ID_FLIGHTSIM_MAPFOLLOWSAIRCRAFT_LOCAL, OnUpdateFlightsimMapfollowsaircraftLocal)
	//}}AFX_MSG_MAP
//	ON_COMMAND(WM_DROPFILES, CView::OnDropFiles)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_INITIATELINKDRAWING, DoLinkDrawing)
	ON_COMMAND(ID_INCREMENTFLYTOWAYPOINT, IncrementFlytoWaypoint)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

struct path_t {
	long time;			// timestamp
    double lat;			// latitude
    double lon;        	// longitude
};

CArray<path_t, path_t> fltPath;
CRITICAL_SECTION crit;
static int tryToLink;
static int linked;

SOCKET sockfd;                 /* network socket */
HANDLE netRead;                /* network reading thread */
struct sockaddr_in their_addr; /* broadcast address */
struct hostent* phe;           /* my ip addresses */

drawData::drawData(struct netData *nD, struct sockaddr_in *from) {
	memset(&nData, 0, sizeof(struct netData));
	memset(&source, 0, sizeof(struct sockaddr_in));
	if (nD != NULL) 
		memcpy(&nData, nD, sizeof(struct netData));
	else
		nData.callsign[0] = NULL;
	if (from != NULL)
		memcpy(&source, from, sizeof(struct sockaddr_in));
	drawn = FALSE;
	timeStamp = 0;
};

drawData::drawData(drawData *dd) {
	memcpy(&nData, &dd->nData, sizeof(struct netData));
	memcpy(&source, &dd->source, sizeof(struct sockaddr_in));
	drawn = dd->drawn;
	timeStamp = dd->timeStamp;
};

drawDataHolder::~drawDataHolder() {
	deleteAll();
}

void drawDataHolder::add(drawData *dd) {
	int replaced = FALSE;
	for (int i=0; i<data.GetSize(); i++) {
		if (0==memcmp(&data[i]->source,&dd->source,sizeof(struct sockaddr_in))) {
			memcpy(data[i],dd,sizeof(drawData));
			delete dd;
			replaced = TRUE;
			break;
		}
	}
	if (!replaced)
		data.Add(dd);
}

void drawDataHolder::deleteItem(int index) {
	ASSERT(index >= 0);
	ASSERT(index < data.GetSize());
	delete data[index];
	data.RemoveAt(index);
}

void drawDataHolder::deleteAll() {
	for (int i=data.GetSize(); i>0; i--)
		deleteItem(i-1);
	data.FreeExtra();
}

int drawDataHolder::GetSize() {
	return data.GetSize();
}

drawDataHolder drawn;
drawDataHolder undrawn;

UINT LinkDrawThread( LPVOID pParam ) {
	WaitForInputIdle(GetCurrentProcess(), 10000);
	CNavView *pView = (CNavView *) pParam;
	time_t timeNow, nextTime, lastTime = 0;
	while (pView->linkDrawing) {
		nextTime = lastTime + (time_t) pView->linkUpdateTime; 
		time(&timeNow); 
		if ((linked || pView->networkInitialized) && timeNow>nextTime) {
			pView->PostMessage(WM_COMMAND, ID_INITIATELINKDRAWING, 0);
			lastTime = timeNow;
		}
		Sleep(100);
	}
	return 0;
}

// define a structure for storing flight data
struct {
  DWORD dwLatLo;			// latitude  (low dword)
  DWORD dwLatHi;			// latitude  (high dword)
  DWORD dwLonLo;			// longitude (low dword)
  DWORD dwLonHi;			// longitude (high dword)
  DWORD dwAltLo;			// altitude  (low dword)
  DWORD dwAltHi;			// altitude  (high dword)
  DWORD dwHeading;			// heading
  DWORD dwTAS;				// true airspeed (m/s * 256)
  WORD  wMagVar;			// magnetic variation 
  DWORD	dwAPMaster;         // autopilot master switch (0 or 1)
  DWORD dwAPWingLeveler;    // autopilot wing leveler enable (0 or 1)
  DWORD dwAPHeadingHold;    // autopilot heading hold enable (0 or 1)
  DWORD dwAPHeading;        // autopilot heading to hold (actual heading * 0x10000/360.0)
  DWORD dwAPAttitudeHold;   // autopilot attitude (pitch & bank) hold enable (0 or 1)
  DWORD dwAPAltitude;       // autopilot altitude to hold (actual altitude in meters * 0x10000)
  DWORD dwAPAltitudeHold;   // autopilot altitude hold enable (0 or 1)
} flightData;

// Define a structure to hold data specific to each version of FS
// This structure helps ease compatibility with both FS95 and FS98.
struct FSInfo {
  const char* pszWindowClass;	// name of the window class
  // state data offsets
  DWORD offset_dwLatLo;
  DWORD offset_dwLatHi;
  DWORD offset_dwLonLo;
  DWORD offset_dwLonHi;
  DWORD offset_dwAltLo;
  DWORD offset_dwAltHi;
  DWORD offset_dwHeading;
  DWORD offset_dwTAS;
  DWORD offset_wMagVar;
  DWORD offset_dwAPMaster;           
  DWORD offset_dwAPWingLeveler;      
  DWORD offset_dwAPHeadingHold;      
  DWORD offset_dwAPHeading;          
  DWORD offset_dwAPAttitudeHold;     
  DWORD offset_dwAPAltitude;     
  DWORD offset_dwAPAltitudeHold;     
};

// FS95 information
FSInfo fs95Info = {
  "FS6MAIN",					// name of the window class
  // state data offsets
  FIELD_OFFSET(FS6StateData, dwLatLo),                            
  FIELD_OFFSET(FS6StateData, dwLatHi),                                
  FIELD_OFFSET(FS6StateData, dwLonLo),
  FIELD_OFFSET(FS6StateData, dwLonHi),
  FIELD_OFFSET(FS6StateData, dwAltLo),
  FIELD_OFFSET(FS6StateData, dwAltHi),
  FIELD_OFFSET(FS6StateData, dwHeading),
  FIELD_OFFSET(FS6StateData, dwTAS),
  FIELD_OFFSET(FS6StateData, wMagVar),
  FIELD_OFFSET(FS6StateData, dwAPMaster),                            
  FIELD_OFFSET(FS6StateData, dwAPWingLeveler),                                
  FIELD_OFFSET(FS6StateData, dwAPHeadingHold),
  FIELD_OFFSET(FS6StateData, dwAPHeading),
  FIELD_OFFSET(FS6StateData, dwAPAttitudeHold),
  FIELD_OFFSET(FS6StateData, dwAPAltitude),
  FIELD_OFFSET(FS6StateData, dwAPAltitudeHold),
};

// FS98 information
FSInfo fs98Info = {
  "FS98MAIN",					// name of the window class
  // state data offsets
  FIELD_OFFSET(FS98StateData, dwLatLo),                            
  FIELD_OFFSET(FS98StateData, dwLatHi),                                
  FIELD_OFFSET(FS98StateData, dwLonLo),
  FIELD_OFFSET(FS98StateData, dwLonHi),
  FIELD_OFFSET(FS98StateData, dwAltLo),
  FIELD_OFFSET(FS98StateData, dwAltHi),
  FIELD_OFFSET(FS98StateData, dwHeading),
  FIELD_OFFSET(FS98StateData, dwTAS),
  FIELD_OFFSET(FS98StateData, wMagVar),
  FIELD_OFFSET(FS98StateData, dwAPMaster),                            
  FIELD_OFFSET(FS98StateData, dwAPWingLeveler),                                
  FIELD_OFFSET(FS98StateData, dwAPHeadingHold),
  FIELD_OFFSET(FS98StateData, dwAPHeading),
  FIELD_OFFSET(FS98StateData, dwAPAttitudeHold),
  FIELD_OFFSET(FS98StateData, dwAPAltitude),
  FIELD_OFFSET(FS98StateData, dwAPAltitudeHold),
};

// FS2000 information
FSInfo fs2000Info = {
  "FS98MAIN",	// name of the window class (same as FS98 for some reason)
  // state data offsets
  FIELD_OFFSET(FS2000StateData, dwLatLo),                            
  FIELD_OFFSET(FS2000StateData, dwLatHi),                                
  FIELD_OFFSET(FS2000StateData, dwLonLo),
  FIELD_OFFSET(FS2000StateData, dwLonHi),
  FIELD_OFFSET(FS2000StateData, dwAltLo),
  FIELD_OFFSET(FS2000StateData, dwAltHi),
  FIELD_OFFSET(FS2000StateData, dwHeading),
  FIELD_OFFSET(FS2000StateData, dwTAS),
  FIELD_OFFSET(FS2000StateData, wMagVar),
  // not yet working:
  FIELD_OFFSET(FS2000StateData, dwAPMaster),                            
  FIELD_OFFSET(FS2000StateData, dwAPWingLeveler),                                
  FIELD_OFFSET(FS2000StateData, dwAPHeadingHold),
  FIELD_OFFSET(FS2000StateData, dwAPHeading),
  FIELD_OFFSET(FS2000StateData, dwAPAttitudeHold),
  FIELD_OFFSET(FS2000StateData, dwAPAltitude),
  FIELD_OFFSET(FS2000StateData, dwAPAltitudeHold),
};

void debugMessage(char *message) {
	FILE *fp;
	fp = fopen("debug.out", "a");
	fprintf(fp, "%s\n", message);
	fclose(fp);
}

BOOL CheckFSTitle(HWND hWnd, char *psz){ // by Peter Dowson <pete@millhays.demon.co.uk>
	char chWork[64];
	if (GetWindowText(hWnd, chWork,64) == 0)
		return FALSE;
	return (strstr(chWork, psz) != NULL);
}

UINT LinkThread( LPVOID pParam ) {
	HANDLE hShut = OpenEvent(EVENT_ALL_ACCESS, TRUE,"shutdownevent");
	CNavView *pView = (CNavView *) pParam;
	WaitForInputIdle(GetCurrentProcess(), 10000);
	FS6IPC_Client *ipcClient;		// create the IPC client
	FSInfo* pFSInfo;				// set this pointer to either FS95 or FS98 state offsets 
	HWND hWndFs6 = NULL;
	struct netData sendData;
	while(tryToLink == TRUE) {
		if (linked == FALSE) {	// look for FS95, FS98, FS2000 program
			if (hWndFs6 = FindWindowEx(NULL, NULL, fs95Info.pszWindowClass, NULL))
				pFSInfo = &fs95Info;
			else if (hWndFs6 = FindWindowEx(NULL, NULL, "FS2KMAIN", NULL))
				pFSInfo = &fs2000Info;
			else if (hWndFs6 = FindWindowEx(NULL, NULL, fs98Info.pszWindowClass, NULL))
				pFSInfo = CheckFSTitle(hWndFs6, "2000") ? &fs2000Info : &fs98Info;
			if (hWndFs6 != NULL) {		// found it
				linked = TRUE;
				pView->linkFirst = TRUE;
				ipcClient = new FS6IPC_Client;
				ipcClient->Start(hWndFs6);
			} else {					// wait a while
				if (WAIT_OBJECT_0==WaitForSingleObject(hShut, 5000))
					break;
			}
		} else {						// try to get data from FS
			// get data
//			WaitForInputIdle(GetCurrentProcess(),60000); //???
			int waitMS = (DWORD)(pView->linkUpdateTime * 1000);
			if (waitMS < 1000)
				waitMS = 1000;
			if (WAIT_OBJECT_0==WaitForSingleObject(hShut, waitMS))
				break;
			ipcClient->BeginRequest();
			EnterCriticalSection(&crit);
			ipcClient->ReadStateData(pFSInfo->offset_dwLatLo, 6 * sizeof(DWORD), &flightData.dwLatLo);
			ipcClient->ReadStateData(pFSInfo->offset_dwHeading, sizeof(DWORD), &flightData.dwHeading);
			ipcClient->ReadStateData(pFSInfo->offset_dwTAS, sizeof(DWORD), &flightData.dwTAS);
			ipcClient->ReadStateData(pFSInfo->offset_wMagVar, sizeof(WORD), &flightData.wMagVar);
			if (ipcClient->SendRequest()) {
				// process the data we just read
				LeaveCriticalSection(&crit);
				double lat, lon;
				int intPart = flightData.dwLatHi;
				if (intPart >= 0)
					lat = intPart + (double)flightData.dwLatLo / (double)0x100000000;
				else		 
					lat = intPart - (double)flightData.dwLatLo / (double)0x100000000;
				lat *= 45.0 / 5000875.0;

				intPart = flightData.dwLonHi;
				if (intPart >= 0)
					lon = intPart + (double)flightData.dwLonLo / (double)0x100000000;
				else
					lon = intPart - (double)flightData.dwLonLo / (double)0x100000000;
				lon *= 90.0 / (256.0 * 4194304.0);

				pView->lastLatRead = lat;
				pView->lastLonRead = lon;
				double alt = ((double)flightData.dwAltHi + (double)flightData.dwAltLo / (double)0x100000000) * 3.28084;
				double head = 360 * (double)flightData.dwHeading / (double)0x100000000;
				double tas = ((double)flightData.dwTAS / 256.0) * 3600 / (degToMeters/60); // kts
				double mag = ((double)flightData.wMagVar * 360.0) / 0x10000;
				if (mag > 180) mag = mag-360;
				mag *= -1.0;

				// check if we need to go to the next plan waypoint
				if (pView->flyToWaypoint > 0) {
					if (pView->flyToWaypoint != pView->plancount) {
						double dist = pView->gcDist(lat, lon, pView->flyingLat, pView->flyingLon);
						if (dist < 2000) // less than 2 km is close enough
							pView->PostMessage(WM_COMMAND, ID_INCREMENTFLYTOWAYPOINT, 0);
					}
				}
				
				// send network packet
				sendData.heading = head;
				sendData.alt = alt;
				sendData.tas = tas;
				sendData.lat = lat;
				sendData.lon = lon;
				sendData.mag = mag;
				strncpy(sendData.callsign, pView->callsign, 30);
				if (pView->networkInitialized) 
					pView->netSend((char*) &sendData, sizeof(netData));

				// add to the draw queue
				undrawn.add(new drawData(&sendData));

			} else {
				linked = FALSE; // FS did not respond
				LeaveCriticalSection(&crit);
				ipcClient->Stop();
				delete ipcClient;
			}
		}
	}
	if (linked == TRUE) {
		linked = FALSE;
		ipcClient->Stop();
		delete ipcClient;
	}
	return(0);
}

UINT FlyThread( LPVOID pParam ) {
	CNavView *pView = (CNavView *) pParam;
	WaitForInputIdle(GetCurrentProcess(), 10000);
	FSInfo* pFSInfo;	  // set this pointer to either FS95, FS98 or FS2000 state offsets 
	HWND hWndFs6 = NULL;
	while (pView->flying) {
		if (hWndFs6 == NULL) { // initialization
			if (hWndFs6 = FindWindowEx(NULL, NULL, fs95Info.pszWindowClass, NULL))
				pFSInfo = &fs95Info;
			else if (hWndFs6 = FindWindowEx(NULL, NULL, "FS2KMAIN", NULL))
				pFSInfo = &fs2000Info;
			else if (hWndFs6 = FindWindowEx(NULL, NULL, fs98Info.pszWindowClass, NULL))
				pFSInfo = CheckFSTitle(hWndFs6, "2000") ? &fs2000Info : &fs98Info;
		}
		if (hWndFs6 == NULL) {		
			pView->hflythread = 0;
			pView->flying = FALSE;
			AfxMessageBox("Can't find FlightSim. Is it running?");
			return(0);
		}
		FS6IPC_Client ipcClient;	
		EnterCriticalSection(&crit);
		ipcClient.Start(hWndFs6);
		ipcClient.ReadStateData(pFSInfo->offset_dwLatLo, 6 * sizeof(DWORD), &flightData.dwLatLo);
		ipcClient.ReadStateData(pFSInfo->offset_wMagVar, sizeof(WORD), &flightData.wMagVar);
		if (ipcClient.SendRequest()) {
			double lat, lon;
			int intPart = flightData.dwLatHi;
			if (intPart >= 0)
				lat = intPart + (double)flightData.dwLatLo / (double)0x100000000;
			else		 
				lat = intPart - (double)flightData.dwLatLo / (double)0x100000000;
			lat *= 45.0 / 5000875.0;
			intPart = flightData.dwLonHi;
			if (intPart >= 0)
				lon = intPart + (double)flightData.dwLonLo / (double)0x100000000;
			else
				lon = intPart - (double)flightData.dwLonLo / (double)0x100000000;
			lon *= 90.0 / (256.0 * 4194304.0);
			double mag = ((double)flightData.wMagVar * 360.0) / 0x10000;
			if (mag > 180) mag = mag-360;
			mag *= -1.0;
			double dist = pView->gcDist(lat, lon, pView->flyingLat, pView->flyingLon);
			double course = pView->bearing+mag;
			if (course < 0) course += 360;
			if (course > 360) course -= 360;

			flightData.dwAPHeading = (DWORD) (course * 0x10000 / 360.0);
			ipcClient.BeginRequest();
			ipcClient.WriteStateData(pFSInfo->offset_dwAPHeading, sizeof(DWORD), &flightData.dwAPHeading);
			if (pView->flyToWaypoint > 0) {
				flightData.dwAPAltitude = (DWORD) (pView->flyingAlt * .3048 * 0x10000);
				ipcClient.WriteStateData(pFSInfo->offset_dwAPAltitude, sizeof(DWORD), &flightData.dwAPAltitude);
			}
			// turn on the nav hold switches or just update the heading?
			if (pView->linkFlySetMode) {
				flightData.dwAPMaster = 1;           
				flightData.dwAPWingLeveler = 0;     
				flightData.dwAPHeadingHold = 1;  
				flightData.dwAPAttitudeHold = 0;;     
				ipcClient.WriteStateData(pFSInfo->offset_dwAPMaster,       sizeof(DWORD), &flightData.dwAPMaster);
				ipcClient.WriteStateData(pFSInfo->offset_dwAPWingLeveler,  sizeof(DWORD), &flightData.dwAPWingLeveler);
				ipcClient.WriteStateData(pFSInfo->offset_dwAPHeadingHold,  sizeof(DWORD), &flightData.dwAPHeadingHold);
				ipcClient.WriteStateData(pFSInfo->offset_dwAPAttitudeHold, sizeof(DWORD), &flightData.dwAPAttitudeHold);
				if (pView->flyToWaypoint > 0) {
					flightData.dwAPAltitudeHold = 1;;     
					ipcClient.WriteStateData(pFSInfo->offset_dwAPAltitudeHold, sizeof(DWORD), &flightData.dwAPAltitudeHold);
				}     
			}
			ipcClient.SendRequest();
		}
		ipcClient.Stop();
		LeaveCriticalSection(&crit);
		WaitForSingleObject(pView->hflyevent, (DWORD)(pView->linkFlyUpdateTime * 1000));
	}
	pView->hflythread = 0;
	return(0);
}

/////////////////////////////////////////////////////////////////////////////
// CNavView construction/destruction
	  
CNavView::CNavView() {
	orgDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	init = TRUE;
	initAfterPrint = FALSE;
	fontSet = FALSE;
	title = "";
	search = "";
	measuring = FALSE;
	planning = FALSE;
	plancount = 0;
	planOnly = FALSE;
	planInfo = TRUE;
	abort = FALSE;
	flightPaths = 0;
	flightTrackDensity = 10; // 10 seconds
	largeSymbols = TRUE;
	symbolTransition = 6;
	left = -180;
	right = 180;
	top = 90;
	bottom = -90;
	mag = 1;
	mag2 = 1;
	centerLat = 0;
	centerLon = 0;
	Mready = FALSE;
	linked = FALSE;
	linkUpdateTime = 5;
	linkFlyUpdateTime = 5;
	linkFlySetMode = FALSE;
	xLinkExtent = yLinkExtent = 0;
	flying = FALSE;
	InitializeCriticalSection(&crit);
	hflythread = 0;
	hflyevent = CreateEvent(NULL, FALSE, FALSE, "flyevent");
	hShutdown = CreateEvent(NULL, FALSE, FALSE, "shutdownevent");
	flyToWaypoint = -1;
	flyingAlt = -1;
	lastLatRead = 0.0;
	lastLonRead = 0.0;
	networkInitialized = FALSE;
	sockfd = -1;
	linkDrawing = TRUE;
	drawing = FALSE;
	centerOnAircraft = 0;

	CFont font;		// initialize default font
	font.CreateFont(-11,0,0,0,FW_NORMAL,0,0,0,ANSI_CHARSET,3,2,1,34,"Arial");
	font.GetLogFont(&lf);

	// set OS version
    OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&vi)) {
		if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
			OS = 1; // NT
		else if (vi.dwPlatformId == VER_PLATFORM_WIN32s)
			OS = 3; // Win32s
		else 
			OS = 2; // Win95
	} else
		OS = 2; // call failed, assume Win95

	// dummy values, will be overwritten
	grid = 0;
	vor = 0;
	ils = 0;
	ndb = 0;
	id = 0;
	freq = 0;
	dynscn = 1;
	dynobj = 1;
	airports = 0;
	markers = 0;
	atis = 1;
	waypoints = 1;
	routes = 1;
	flatWorld = 1;
	planVOR = 1;
	planNDB = 0;
	planAirport = 0;
	planWaypoints = 0;
	planContinue = 1;
	showAirportZones = 1;
	showMaps = 1;
	showFancyMaps = 0;
	idealDist = 149.0; // km, about 80 nm
	tryToLink = link = 1;
    forceBlackPrinting = 1;
	startWithLastList = FALSE;
	lastSceneryList = "";
	enableNetwork = FALSE;
	startWithLastView = FALSE;
	lastViewZoomFinished = FALSE;

	fm = new FancyMapper();
}

CNavView::~CNavView() {
	char buffer[20];
	sprintf(buffer, "%d", mag);
	WritePrivateProfileString("View", "lastViewZoom", buffer, "nav.ini");
	sprintf(buffer, "%lf", centerLat);
	WritePrivateProfileString("View", "lastViewLat", buffer, "nav.ini");
	sprintf(buffer, "%lf", centerLon);
	WritePrivateProfileString("View", "lastViewLon", buffer, "nav.ini");

	linkDrawing = FALSE;
	WaitForSingleObject(hlinkdrawingthread, INFINITE); 

	tryToLink = link = FALSE;
	flying = FALSE;
	SetEvent(hShutdown);
	Sleep(200);
	CloseHandle(hShutdown);

	shutdownNetwork();
	drawn.deleteAll();
	undrawn.deleteAll();
		
	if (planning)  
		delete planD;

	if (linked) {
//		linked = FALSE;
		WaitForSingleObject(hthread, INFINITE); 
		CloseHandle(hthread);
	}
	if (hflythread != 0) {
		SetEvent(hflyevent);
		WaitForSingleObject(hflythread, INFINITE); 
		CloseHandle(hflythread);
	}
	CloseHandle(hflyevent);	
	DeleteCriticalSection(&crit);
	if (flightPaths > 0) {
		fltPath.RemoveAll();
		fltPath.FreeExtra();
		flightPaths--;
	}
	windtable.RemoveAll();
	delete fm;
}

/////////////////////////////////////////////////////////////////////////////

int CNavView::LtoC(double xl, double yl, int &xc, int &yc) {
	if (flatWorld) {
		xc = (int) ((xl - left)/(right-left)*screenWidth);
		yc = (int) ((top - yl)/(top - bottom)*screenHeight);
		if ((xl<left) || (xl>right) || (yl<bottom) || (yl>top))
			return(FALSE);
		return(TRUE);
	} else {
		ASSERT(convX > 0);
		ASSERT(convY > 0);
		if (!Mready)
			Mcalc();
		double xo,yo,zo,xs,ys,zs;
		StoCe(xo,yo,zo,yl,xl);			// convert to normalized cartesian
		zs = xo*M31 + yo*M32 + zo*M33;  // rotate to viewpoint 
		xs = xo*M11 + zo*M13;			// (M12 skipped, should always be 0)
		xs *= convX * screenWidth/2; 
		xc = screenWidth/2  + (int) (xs);
		ys = xo*M21 + yo*M22 + zo*M23;
		ys *= convY * screenHeight/2;
		yc = screenHeight/2 - (int) (ys);
		if (zs < 0)
			return FALSE;				// dark side of the earth
		if (fabs(xs) > screenWidth/2)
			return(FALSE);
		if (fabs(ys) > screenHeight/2)
			return(FALSE);
		return TRUE;
	}
}

int CNavView::CtoL(int xc, int yc, double &xl, double &yl) {
	if (flatWorld) {
		xl = left+((double) xc)/screenWidth*(right-left);
		yl = top-((double) yc)/screenHeight*(top - bottom);
		return(TRUE);
	} else {	
		ASSERT(convX > 0);
		ASSERT(convY > 0);
		if (!Mready)
			Mcalc();
		double xo,yo,zo,xs,ys,zs,t;
		double nx = xc - screenWidth/2;		// center screen is 0,0
		double ny = screenHeight/2 - yc;
		xs = nx / (convX * screenWidth/2);
		ys = ny / (convY * screenHeight/2);
		t = xs*xs+ys*ys;
		if (t > 1)							// invalid point in orbit
			return(FALSE);					
		zs = sqrt(1 - t);					// positive root selects this side of earth
		xo = xs*M11 + ys*M21 + zs*M31;
		yo =          ys*M22 + zs*M32;		// (M12 skipped, should always be 0)
		zo = xs*M13 + ys*M23 + zs*M33;
		double junk;
		CtoS(xo,yo,zo,yl,xl,junk);			// convert from normalized cartesian
		fixup(xl, yl);						// normalize angles
		return(TRUE); 
	}
}

void CNavView::fixup(double &x, double &y) {  // fixup wrap-around
	if (y > 90) {
		y -= (y - 90) * 2;
		x += 180;
	} else if (y < -90) {
		y += (y + 90) * 2;
		x += 180;
	}
	while (x > 180)
		x -= 360;
	while (x < -180)
		x += 360;
}

void CNavView::OnDraw(CDC* pDC) {
	EnterCriticalSection(&crit);
	drawing = TRUE;
	int dcNumber = pDC->SaveDC();
	abort = FALSE;
	linkFirst = TRUE;
	CNavDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (planning && !pDC->IsPrinting())
		planD->update();

	CArray<idPlace, idPlace> idlist;	 
	idlist.SetSize(0,5);

	xLinkExtent = yLinkExtent = 0;
	GetClientRect(clientRect);
	pDC->FillSolidRect(clientRect, RGB(255,255,255));

	CFont font, titleFont;
	LOGFONT lfnew = lf;
	if (pDC->IsPrinting()) {	// make font larger on printer
		lfnew.lfHeight *= 3;
	}
	font.CreateFontIndirect(&lfnew);
	lfnew.lfHeight *= 2;		// double size for title
	titleFont.CreateFontIndirect(&lfnew);
		
	if (pDC->IsPrinting()) {
		screenWidth = pDC->GetDeviceCaps(HORZRES);
		screenHeight = pDC->GetDeviceCaps(VERTRES);
		Size(screenWidth, screenHeight);
		initAfterPrint = TRUE;
	} else {
		screenWidth = clientRect.right;
		screenHeight = clientRect.bottom;
		if (initAfterPrint) {
			Size(screenWidth, screenHeight);
			initAfterPrint = FALSE;
		}
	}

	CRgn clipper; // clip drawing to displayable area
	clipper.CreateRectRgn(0, 0, screenWidth, screenHeight);
	pDC->SelectClipRgn(&clipper, RGN_AND);

	if (init) {
		char buffer[256];
		float temp;
		flatWorld = GetPrivateProfileInt("View", "flatWorld", 1, "nav.ini");
 		GetPrivateProfileString("View", "idealDist", "149", buffer, 30, "nav.ini");
		sscanf(buffer, "%f", &temp);
		idealDist = (double) temp;
		grid = GetPrivateProfileInt("View", "grid", 1, "nav.ini");
		vor = GetPrivateProfileInt("View", "vor", 1, "nav.ini");
		ils = GetPrivateProfileInt("View", "ils", 0, "nav.ini");
		ndb = GetPrivateProfileInt("View", "ndb", 1, "nav.ini");
		airports = GetPrivateProfileInt("View", "airports", 1, "nav.ini");
		markers = GetPrivateProfileInt("View", "markers", 0, "nav.ini");
		atis = GetPrivateProfileInt("View", "atis", 1, "nav.ini");
		waypoints = GetPrivateProfileInt("View", "waypoints", 1, "nav.ini");
		routes = GetPrivateProfileInt("View", "routes", 0, "nav.ini");
		id = GetPrivateProfileInt("View", "id", 0, "nav.ini");
		freq = GetPrivateProfileInt("View", "freq", 0, "nav.ini");
		planVOR = GetPrivateProfileInt("View", "planVOR", 1, "nav.ini");
		planNDB = GetPrivateProfileInt("View", "planNDB", 0, "nav.ini");
		planAirport = GetPrivateProfileInt("View", "planAirport", 0, "nav.ini");
		planWaypoints = GetPrivateProfileInt("View", "planWaypoints", 0, "nav.ini");
		planContinue = GetPrivateProfileInt("View", "planContinue", 1, "nav.ini");
		showAirportZones = GetPrivateProfileInt("View", "showAirportZones", 1, "nav.ini");
		showMaps = GetPrivateProfileInt("View", "showMaps", 1, "nav.ini");
		showFancyMaps = GetPrivateProfileInt("View", "showFancyMaps", 1, "nav.ini");
		dynobj = GetPrivateProfileInt("View", "dynobj", 1, "nav.ini");
		dynscn = GetPrivateProfileInt("View", "dynscn", 1, "nav.ini");
		tryToLink = link = GetPrivateProfileInt("View", "Link", 1, "nav.ini");
 		GetPrivateProfileString("View", "linkUpdateTime", "5", buffer, 30, "nav.ini");
		sscanf(buffer, "%lf", &linkUpdateTime);
 		GetPrivateProfileString("View", "linkFlyUpdateTime", "5", buffer, 30, "nav.ini");
		sscanf(buffer, "%lf", &linkFlyUpdateTime);
		linkFlySetMode = GetPrivateProfileInt("View", "linkFlySetMode", 0, "nav.ini");
		flightTrackDensity = GetPrivateProfileInt("View", "flightTrackDensity", 10, "nav.ini");
		largeSymbols = GetPrivateProfileInt("View", "largeSymbols", 1, "nav.ini");
		forceBlackPrinting = GetPrivateProfileInt("View", "forceBlackPrinting", 1, "nav.ini");
		startWithLastList = GetPrivateProfileInt("View", "startWithLastList", 0, "nav.ini");
 		GetPrivateProfileString("View", "lastSceneryList", "", buffer, 256, "nav.ini");
		lastSceneryList = CString(buffer);
		enableNetwork = GetPrivateProfileInt("View", "enableNetwork", 0, "nav.ini");
 		GetPrivateProfileString("View", "callsign", "", callsign, 30, "nav.ini");
		startWithLastView = GetPrivateProfileInt("View", "startWithLastView", 0, "nav.ini");

		OnEditSaveoptions(); // to make sure Nav.ini exists
		// make sure planes are in Nav.ini
		int planIniInit = GetPrivateProfileInt("Plan", "initialized", FALSE, "nav.ini");
		if (!planIniInit) {
			WritePrivateProfileString("Plan", "initialized", "1", "nav.ini");
			WritePrivateProfileString("Plan", "lastCraft", "Cessna 182", "nav.ini");
			WritePrivateProfileString("Plan", "craftnumber", "0", "nav.ini");

			// data contributed by Luigi Sangiovanni
			addCraft("Airbus 300",        245, 1650, 1800, 410, 1550, 44000, 320,  136, 1800);
			addCraft("Airbus 300-600",    265, 1750, 1800, 470, 1520, 39000, 330,  767, 1800);
			addCraft("Airbus 320",        245, 1045, 1800, 420,  765, 33000, 290,  245, 1800);
			addCraft("Airbus 330",        280, 2156, 1800, 475, 1878, 39000, 330,  934, 1800);
			addCraft("Airbus 340",        290, 2587, 1800, 470, 2253, 39000, 280, 1120, 1800);
			addCraft("ATR-72",            170,  248, 1800, 470,  193, 25000, 235,   65, 1800);
			addCraft("Boeing 727-200",    272, 1625, 1800, 420, 1278, 31000, 290,  448, 1800);
			addCraft("Boeing 737",        263, 1568, 1800, 410, 1156, 37000, 310,  245, 1800);
			addCraft("Boeing 737-300",    280, 1670, 1800, 440, 1158, 35000, 290,  285, 1800);
			addCraft("Boeing 747",        240, 4580, 1800, 450, 3250, 38000, 210, 1250, 1800);
			addCraft("Boeing 747-400",    280, 8600, 1800, 480, 4750, 40000, 320, 1450, 1800);
			addCraft("Boeing 757",        290, 3120, 2200, 410, 1850, 39000, 290,  565, 2200);
			addCraft("Boeing 767",        300, 3345, 1800, 460, 1760, 37000, 290,  400, 1800);
			addCraft("Boeing 777",        320, 5900, 1800, 430, 3100, 39000, 290, 1350, 1800);
			addCraft("Cessna 182",         90,   28,  700, 145,   17, 14000, 115,    4,  700);
			addCraft("Cessna 150",         70,   14,  500, 125,   14, 07000, 105,    7,  500);
			addCraft("Cessna C310",       120,   40,  800, 145,   25, 10000, 130,   12,  500);
			addCraft("C-130 Hercules",    180, 1150, 1800, 280,  869, 14000, 200,  230, 1800);
			addCraft("Douglas DC3",       120,  220,  500, 170,   80, 12000, 140,   35,  500);
			addCraft("Douglas DC4",       115,  320,  800, 195,  234, 12000, 165,  170,  800);
			addCraft("Douglas DC8",       260, 2950, 1800, 390, 2250, 25000, 270, 1050, 1800);
			addCraft("Douglas DC9",       320,  580, 2200, 420,  321, 35000, 420,  220, 2200);
			addCraft("Embraer 120",       170,  245, 1800, 310,  153, 21000, 220,   44, 1800);
			addCraft("Learjet 35A",       320,  380, 2200, 420,  430, 32000, 420,  220, 2200);
			addCraft("MD 80",             280, 1450, 1800, 410, 1100, 31000, 260,  325, 1800);
			addCraft("MD 88",             270, 1300, 1800, 420, 1155, 33000, 270,  225, 1800);
			addCraft("MD 11",             250, 1880, 1800, 410, 1355, 37000, 280,  460, 1800);
		}

		if (enableNetwork && !networkInitialized)
			initNetwork();
		hlinkdrawingthread = AfxBeginThread(LinkDrawThread, this)->m_hThread;

		plancount = 0;
		windtable.RemoveAll();
		if (planning) { // close flight plan window, if open
			planning = FALSE;
			planD->DestroyWindow();	// let it deallocate its own memory
		}
		if (pDoc->dbCount == 0) {
			left = -180;
			right = 180;
			top = 90;
			bottom = -90;
			mag = 1;
			mag2 = 1;
			centerLat = 0;
			centerLon = 0;
			convX = convY = mag2;

			if (screenWidth > screenHeight) 
				convX *= screenHeight / (double) screenWidth;
			else 
				convY *= screenWidth / (double) screenHeight;
		}
		Size(screenWidth, screenHeight);
		
		if ((startWithLastList) && (lastSceneryList != "") && (lastSceneryList != pDoc->listFileName)) {
			pDC->RestoreDC(dcNumber);
			clipper.DeleteObject();
			pDC->SelectClipRgn(NULL);
			drawing = FALSE;
			LeaveCriticalSection(&crit);
			pDoc->OnFileLoad(lastSceneryList);
			return;
		}
	}

	if (!lastViewZoomFinished) 
		ZoomLastView();
	
	int vorsize; 
	if (largeSymbols)
		vorsize = min(50, screenWidth / 15);
	else
		vorsize = min(50, screenWidth / 40);

	CPen *oldPen;
	CPen redPen(PS_SOLID, 0, RGB(255, 0, 0));
	CPen greenPen(PS_SOLID, 0, RGB(0, 192, 0));
	CPen bluePen(PS_SOLID, 0, RGB(0, 0, 255));
	CPen grayPen(PS_SOLID, 0, RGB(192, 192, 192));
	CPen orangePen(PS_SOLID, 0, RGB(255, 192, 0));
	CPen brownPen(PS_SOLID, 0, RGB(228, 215, 190));

	int oldMode = pDC->SetBkMode(TRANSPARENT);

	CFont* oldFont;
	oldFont = (CFont *) pDC->SelectObject(&font);

	int rx1, rx2, ry1, ry2; 
	if (flatWorld) {
		LtoC(-180, 90, rx1, ry1);
		LtoC(180, -90, rx2, ry2);
		pDC->Rectangle(rx1, ry1, rx2, ry2);
	} else {
		// draw earth outline
		double cx = screenWidth  / 2;
		double cy = screenHeight / 2;
		double hx = convX * screenWidth / 2;
		double hy = convY * screenHeight / 2;
		if (mag<7) 
			pDC->Ellipse((int)(cx-hx), (int)(cy-hy), (int)(cx+hx), (int)(cy+hy));
	}
	
	char buf[30];

	if (pDoc->mapsExist && showMaps) 
		DrawMaps(pDC);

	if (fm->mapsDetected && showFancyMaps) 
		fm->DrawMaps(pDC, this, flatWorld, mag2, centerLat, centerLon);

	if (grid)
		DrawGrid(pDC);

	CBrush *oldBrush;
	int i;

	// airport zones
	if (airports && showAirportZones) {
		oldBrush = (CBrush *) pDC->SelectStockObject(LTGRAY_BRUSH);
		if (mag > symbolTransition)
			oldPen = (CPen *) pDC->SelectStockObject(NULL_PEN);
		else
			oldPen = (CPen *) pDC->SelectObject(&grayPen);


		for (i=0; i<pDoc->dbCount; i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (i == plan[q]) {
						proceed = TRUE;
						break;
					}
				if (!proceed)
					goto nextDB1;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SelectObject(oldPen);
					pDC->SelectObject(oldBrush);
					pDC->SelectObject(oldFont);
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}
	
			int x,y;
 			if ((pDoc->db[i].lon) < left) goto nextDB1;
			if ((pDoc->db[i].lon) > right) goto nextDB1;
			if ((pDoc->db[i].lat) < bottom) goto nextDB1;
			if ((pDoc->db[i].lat) > top) goto nextDB1;
			if (!LtoC(pDoc->db[i].lon, pDoc->db[i].lat, x, y)) goto nextDB1;

			// ILS, Runway and airport have zones
			if ((2 == pDoc->db[i].type) || (8 == pDoc->db[i].type) || (4 == pDoc->db[i].type)) {	
				if (mag > symbolTransition) {
					double ldist;
					int l = distToPixels(2000); // l = 2 km
					int lx = l;
 					if(flatWorld) { // calculate latitude distortion due to flat map
						ldist = 1.0/cos(pDoc->db[i].lat * degToRad);
						lx = (int) (lx * ldist);
					}
					pDC->Ellipse(x-lx, y-l, x+lx, y+l);
				} else {
					pDC->MoveTo(x-1, y-1);
					pDC->LineTo(x+3, y+3);
					pDC->MoveTo(x+2, y-1);
					pDC->LineTo(x-2, y+3);
				}
			}
			nextDB1:;
		}
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
	}

	if (planning) {
		if (OS != 3) { // not Win32s (it can't handle the wide pen for some reason)
			LOGBRUSH lb;
			lb.lbStyle = BS_SOLID;
			lb.lbColor = RGB(255, 0, 0);
			CPen widePen(PS_GEOMETRIC | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL, 2, &lb);
			oldPen = (CPen *) pDC->SelectObject(&widePen);	
		} else {  // OS is Win32s (no wide runways)
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);	// switch to black
		}
		int oldx, oldy, x, y, linevis;
		double oldLat, oldLon;
		for (int j=0; j<planD->plantable.GetSize(); j++) {
			LtoC(planD->plantable[j].lon, planD->plantable[j].lat, x, y);
			if (j > 0) {
				linevis = gcLine(oldLon, oldLat, planD->plantable[j].lon, planD->plantable[j].lat, pDC, 0);
				if (linevis && planD->plantable[j].index < 0) {
					pDC->MoveTo(x, y+3);
					pDC->LineTo(x+3, y);
					pDC->LineTo(x, y-3);
					pDC->LineTo(x-3, y);
					pDC->LineTo(x, y+3);
					CString label;
					if (planD->plantable[j].index == -1) 
						label = "TOC";
					else if (planD->plantable[j].index == -2) 
						label = "BOD";
					else if (planD->plantable[j].index == -3) 
						label = "TOC/BOD";
					double head = planD->plantable[j].correctedheading;
					if (((int) head)%180 > 90) {
						pDC->TextOut(x+10, y-10, (LPCTSTR) label);
					} else {
						pDC->TextOut(x+5, y+5, (LPCTSTR) label);
					}
				}
				if (linevis && planInfo) {
					int mx = (x+oldx)/2;
					int my = (y+oldy)/2;
					char buff[30]; 
					double dis = planD->plantable[j].dist;
					int offset = min((int)sqrt((double)distToPixels(dis*1852.0)), 15);
					double head = planD->plantable[j].correctedheading;
					CSize tSize; 
					int chars = sprintf(buff, "%1.1f nm @ %1.0f", dis, head);
					tSize = pDC->GetOutputTextExtent(buff, chars);
					if (((int) head)%180 > 90) {
						idPlace idP = {mx, my-offset, mx+tSize.cx, my-offset+tSize.cy};
						idPlacement(mx, my, idP, &idlist);
						pDC->TextOut(idP.x+2, idP.y, buff);
					} else {
						idPlace idP = {mx, my+offset, mx+tSize.cx, my+offset+tSize.cy};
						idPlacement(mx, my, idP, &idlist);
						pDC->TextOut(idP.x+2, idP.y, buff);
					}

				}
			}
			oldx = x;
			oldy = y;
			oldLat = planD->plantable[j].lat;
			oldLon = planD->plantable[j].lon;
		}
		pDC->SelectObject(oldPen);
	}
	
	// ATIS
	if (atis) {
		if (pDC->IsPrinting() && forceBlackPrinting) { // red does not print well
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
			pDC->SetTextColor(RGB(0, 0, 0));
		} else { 
			oldPen = pDC->SelectObject(&redPen);
		}
		for (i=0; i<pDoc->atisIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->atisIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}
			int x, y;
			if (!LtoC(pDoc->db[pDoc->atisIndex[i]].lon, pDoc->db[pDoc->atisIndex[i]].lat, x, y)) continue;
			if (mag > symbolTransition) {
				pDC->MoveTo(x-1, y-1);
				pDC->LineTo(x+2, y-1);
				pDC->LineTo(x+2, y+2);
				pDC->LineTo(x-1, y+2);
				pDC->LineTo(x-1, y-1);
				if (id || freq) {
					int offset = 10;
					int chars;
					char buf[50];
					CSize tSize; 
					if (freq && id)	
						chars = sprintf(buf, "ATIS %6.2f", pDoc->db[pDoc->atisIndex[i]].freq);
					else if (id) 
						chars = sprintf(buf, "ATIS");
					else if (freq) 
						chars = sprintf(buf, "%6.2f", pDoc->db[pDoc->atisIndex[i]].freq);
					tSize = pDC->GetOutputTextExtent(buf, chars); 
					idPlace idP2 = {x, y, x+1, y+1};
					idlist.Add(idP2);
					idPlace idP = {x+offset, y+offset, x+offset+tSize.cx+4, y+offset+tSize.cy};
					idPlacement(x, y, idP, &idlist);
					pDC->Rectangle(idP.x, idP.y, idP.sx, idP.sy);
					pDC->TextOut(idP.x+2, idP.y, buf);
				}
			} else {
				pDC->MoveTo(x-1, y);
				pDC->LineTo(x+2, y);
				pDC->MoveTo(x, y-1);
				pDC->LineTo(x, y+2);
			}
		}
		pDC->SelectObject(oldPen);
	}

	// Waypoints
	if (waypoints) {
		COLORREF oldColor;
		if (pDC->IsPrinting() && forceBlackPrinting) { // orange does not print well
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
			oldColor = pDC->SetTextColor(RGB(0, 0, 0));
		} else { 
			oldPen = pDC->SelectObject(&orangePen);
			oldColor = pDC->SetTextColor(RGB(192,192,192));
		}
		for (i=0; i<pDoc->waypointIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->waypointIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SelectObject(oldPen);
					pDC->SetTextColor(oldColor);
					pDC->SelectObject(oldFont);
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}

			int x, y;
			if (!LtoC(pDoc->db[pDoc->waypointIndex[i]].lon, pDoc->db[pDoc->waypointIndex[i]].lat, x, y)) continue;
			if (mag > symbolTransition) {
				pDC->MoveTo(x, y-1);
				pDC->LineTo(x+2, y+2);
				pDC->LineTo(x-2, y+2);
				pDC->LineTo(x, y-1);
				if (id) {
					int offset = 5;
					int chars;
					char buf[50];
					CSize tSize; 
					chars = sprintf(buf, "%s", pDoc->db[pDoc->waypointIndex[i]].name);
					tSize = pDC->GetOutputTextExtent(buf, chars); 
					idPlace idP2 = {x, y, x+1, y+1};
					idlist.Add(idP2);
					idPlace idP = {x+offset, y+offset, x+offset+tSize.cx+4, y+offset+tSize.cy};
					idPlacement(x, y, idP, &idlist);
					pDC->TextOut(idP.x+2, idP.y, buf);
				}
			} else {
				pDC->MoveTo(x, y);
				pDC->LineTo(x+1, y+1);
				pDC->LineTo(x-1, y+1);
				pDC->LineTo(x, y);
			}
		}
		pDC->SetTextColor(oldColor);
		pDC->SelectObject(oldPen);
	}

	// Routes
	if (routes && !planOnly) {
		COLORREF oldColor;
		if (pDC->IsPrinting() && forceBlackPrinting) { // brown does not print well
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
			oldColor = pDC->SetTextColor(RGB(0, 0, 0));
		} else { 
			oldPen = pDC->SelectObject(&brownPen);
			oldColor = pDC->SetTextColor(RGB(128,128,128));
		}
		int x, y, x2, y2, vis, xm, ym;
		double rlat1, rlon1, rlat2, rlon2;
		for (i=0; i<pDoc->routeIndex.GetSize(); i++) {

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SelectObject(oldPen);
					pDC->SetTextColor(oldColor);
					pDC->SelectObject(oldFont);
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}
			rlat1 = pDoc->db[pDoc->routeIndex[i]].lat;
			rlon1 = pDoc->db[pDoc->routeIndex[i]].lon;
			rlat2 = pDoc->db[pDoc->routeIndex[i]].approach;
			rlon2 = pDoc->db[pDoc->routeIndex[i]].range;
			if (rlat2 > 180) // negative lats are sometimes wrongly "corrected"
				rlat2 -= 360;

			vis = gcLine(rlon1, rlat1, rlon2, rlat2, pDC, FALSE);
			if (mag > symbolTransition) {
				if (id & vis) {
					LtoC(rlon1, rlat1, x, y);
					LtoC(rlon2, rlat2, x2, y2);
					xm = (x + x2)/2;
					ym = (y + y2)/2;
					int offset = 5;
					int chars;
					char buf[50];
					CSize tSize; 
					chars = sprintf(buf, "%s", pDoc->db[pDoc->routeIndex[i]].name);
					tSize = pDC->GetOutputTextExtent(buf, chars); 
					idPlace idP2 = {x, y, x+1, y+1};
					idlist.Add(idP2);
					idPlace idP3 = {x2, y2, x2+1, y2+1};
					idlist.Add(idP3);
					idPlace idP = {xm+offset, ym+offset, xm+offset+tSize.cx+4, ym+offset+tSize.cy};
					idPlacement(xm, ym, idP, &idlist);
					pDC->TextOut(idP.x+2, idP.y, buf);
				}
			}
		}
		pDC->SetTextColor(oldColor);
		pDC->SelectObject(oldPen);
	}

	if(ndb) {
		oldBrush = (CBrush *) pDC->SelectStockObject(WHITE_BRUSH);
		if (pDC->IsPrinting() && forceBlackPrinting) { // green does not print well
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
		} else { 
			oldPen = pDC->SelectObject(&greenPen);
		}
		for (i=0; i<pDoc->ndbIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->ndbIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SelectObject(oldBrush);
					pDC->SelectObject(oldPen);
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}

			int x,y;
			if (!LtoC(pDoc->db[pDoc->ndbIndex[i]].lon, pDoc->db[pDoc->ndbIndex[i]].lat, x, y)) continue;

			// draw a 3 by 3 pixel cross
			int offset = 0;
			if (mag > symbolTransition) {
				pDC->SelectObject(oldBrush);
				oldBrush = (CBrush *) pDC->SelectStockObject(LTGRAY_BRUSH);
				pDC->Ellipse(x-vorsize/6, y-vorsize/6, x+vorsize/6, y+vorsize/6);
				offset = vorsize/6-2;
				pDC->SelectObject(oldBrush);
				oldBrush = (CBrush *) pDC->SelectStockObject(WHITE_BRUSH);
				pDC->Ellipse(x-vorsize/12, y-vorsize/12, x+vorsize/12, y+vorsize/12);
			} else {
				pDC->MoveTo(x-1, y);
				pDC->LineTo(x+2, y);
				pDC->MoveTo(x, y-1);
				pDC->LineTo(x, y+2);
			}
			if ((largeSymbols) && (mag > symbolTransition)) {
				if (id || freq) {
					int chars, chars2;
					CString s1, s2;
					char buf2[50];
					CSize tSize, tSize2;
					s1 = pDoc->db[pDoc->ndbIndex[i]].name;
					s1.TrimRight();
					s2 = pDoc->db[pDoc->ndbIndex[i]].id;
					s2.TrimRight();
					if (freq && id)	{
						chars = sprintf(buf, "%s", (LPCTSTR) s1);
						chars2 = sprintf(buf2, "%6.2f %s", pDoc->db[pDoc->ndbIndex[i]].freq, (LPCTSTR) s2);
						tSize = pDC->GetOutputTextExtent(buf, chars); 
						tSize2 = pDC->GetOutputTextExtent(buf2, chars2);
						if (tSize2.cx > tSize.cx)
							tSize.cx = tSize2.cx;
					} else if (id) {
						chars = sprintf(buf, "%s", (LPCTSTR) s1);
						chars2 = sprintf(buf2, "%s", (LPCTSTR) s2);
						tSize = pDC->GetOutputTextExtent(buf, chars); 
						tSize2 = pDC->GetOutputTextExtent(buf2, chars2); 
						if (tSize2.cx > tSize.cx)
							tSize.cx = tSize2.cx;
					} else if (freq) {
						chars = sprintf(buf, "%6.2f", pDoc->db[pDoc->ndbIndex[i]].freq);
						tSize = pDC->GetOutputTextExtent(buf, chars); 
						tSize2.cx = tSize2.cy = 0;
					}
					idPlace idP2 = {x, y, x+1, y+1};
					idlist.Add(idP2);
					idPlace idP = {x-offset, y+offset/2, x-offset+tSize.cx+4, y+offset/2+tSize.cy+tSize2.cy};
					idPlacement(x, y, idP, &idlist);
					CRect box = CRect(idP.x, idP.y, idP.x+tSize.cx+4, idP.y+tSize.cy+tSize2.cy);
					pDC->Rectangle(box);
					CPoint closest;  //find closest corner
					int cornerDist = (int) closestCorner(CPoint(x,y), box, closest);
					if (cornerDist>vorsize/6) 
						drawDist(pDC, cornerDist-vorsize/6, closest, CPoint(x,y));
					pDC->TextOut(idP.x+2, idP.y, buf);
					if (!freq || id) 
						pDC->TextOut(idP.x+2, idP.y+tSize2.cy, buf2);
				}
			} else {
				if (id || freq) {
					if (freq && id)
						sprintf(buf, "%s (%6.2f)", pDoc->db[pDoc->ndbIndex[i]].id, pDoc->db[pDoc->ndbIndex[i]].freq);
					else if (id)
						sprintf(buf, "%s", pDoc->db[pDoc->ndbIndex[i]].id);
					else if (freq)
						sprintf(buf, "%6.2f", pDoc->db[pDoc->ndbIndex[i]].freq);
					pDC->TextOut(x, y+offset+4, buf);
				}
			}
		}
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
	}

	if (vor) { 
		if (pDC->IsPrinting() && forceBlackPrinting) { // blue does not print well
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
		} else { 
			oldPen = pDC->SelectObject(&bluePen);
		}
		oldBrush = (CBrush *) pDC->SelectStockObject(NULL_BRUSH);
		for (i=0; i<pDoc->vorIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->vorIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SelectObject(oldBrush);
					pDC->SelectObject(oldPen);
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}

			int x,y;
			if (!LtoC(pDoc->db[pDoc->vorIndex[i]].lon, pDoc->db[pDoc->vorIndex[i]].lat, x, y)) continue;

			pDC->MoveTo(x-1, y);
			pDC->LineTo(x+2, y);
			pDC->MoveTo(x, y-1);
			pDC->LineTo(x, y+2);
			// draw a 3 by 3 pixel box
			int offset = 0;
//			pDC->MoveTo(x-2, y-2);
//			pDC->LineTo(x+1, y-2);
//			pDC->LineTo(x+1, y+1);
//			pDC->LineTo(x-2, y+1);
//			pDC->LineTo(x-2, y-2);
			if (mag > symbolTransition) {
				pDC->Ellipse(x-vorsize, y-vorsize, x+vorsize, y+vorsize);
				pDC->MoveTo(x,y);
				double lx = (vorsize * -sin(pDoc->db[pDoc->vorIndex[i]].magdev * degToRad));
				double ly = (vorsize * -cos(pDoc->db[pDoc->vorIndex[i]].magdev * degToRad));
				pDC->LineTo(x + (int) lx, y + (int) ly);
				int tick = 30;
				if (largeSymbols) 
					tick = 10;
				for (int a = 0; a<180; a+=tick) {
					lx = vorsize * sin((pDoc->db[pDoc->vorIndex[i]].magdev + a) * degToRad);
					ly = vorsize * cos((pDoc->db[pDoc->vorIndex[i]].magdev + a) * degToRad);
					if (a%30 == 0) {	 // big ticks (30 degrees)
						pDC->MoveTo(x + (int) lx, y + (int) ly);
						pDC->LineTo(x + (int) (lx * .80), y + (int) (ly * .80));
						pDC->MoveTo(x - (int) lx, y - (int) ly);
						pDC->LineTo(x - (int) (lx * .80), y - (int) (ly * .80));
					} else {			 // medium ticks (10 degrees)
						pDC->MoveTo(x + (int) lx, y + (int) ly);
						pDC->LineTo(x + (int) (lx * .9), y + (int) (ly * .9));
						pDC->MoveTo(x - (int) lx, y - (int) ly);
						pDC->LineTo(x - (int) (lx * .9), y - (int) (ly * .9));
					}
					if (largeSymbols) {	 // small ticks (5 degrees)
						lx = vorsize * sin((pDoc->db[pDoc->vorIndex[i]].magdev + a + tick/2) * degToRad);
						ly = vorsize * cos((pDoc->db[pDoc->vorIndex[i]].magdev + a + tick/2) * degToRad);
						pDC->MoveTo(x + (int) lx, y + (int) ly);
						pDC->LineTo(x + (int) (lx * .96), y + (int) (ly * .96));
						pDC->MoveTo(x - (int) lx, y - (int) ly);
						pDC->LineTo(x - (int) (lx * .96), y - (int) (ly * .96));
					}
				}
				offset = vorsize - 2;
			}
			if ((largeSymbols) && (mag > symbolTransition)) {
				if (id || freq) {
					int chars, chars2;
					CString s1, s2;
					char buf2[50];
					CSize tSize, tSize2;
					oldBrush = (CBrush *) pDC->SelectStockObject(WHITE_BRUSH);
					s1 = pDoc->db[pDoc->vorIndex[i]].name;
					s1.TrimRight();
					s2 = pDoc->db[pDoc->vorIndex[i]].id;
					s2.TrimRight();
					if (freq && id)	{
						chars = sprintf(buf, "%s", (LPCTSTR) s1);
						chars2 = sprintf(buf2, "%6.2f %s", pDoc->db[pDoc->vorIndex[i]].freq, (LPCTSTR) s2);
						tSize = pDC->GetOutputTextExtent(buf, chars); 
						tSize2 = pDC->GetOutputTextExtent(buf2, chars2);
						if (tSize2.cx > tSize.cx)
							tSize.cx = tSize2.cx;
					} else if (id) {
						chars = sprintf(buf, "%s", (LPCTSTR) s1);
						chars2 = sprintf(buf2, "%s", (LPCTSTR) s2);
						tSize = pDC->GetOutputTextExtent(buf, chars); 
						tSize2 = pDC->GetOutputTextExtent(buf2, chars2); 
						if (tSize2.cx > tSize.cx)
							tSize.cx = tSize2.cx;
					} else if (freq) {
						chars = sprintf(buf, "%6.2f", pDoc->db[pDoc->vorIndex[i]].freq);
						tSize = pDC->GetOutputTextExtent(buf, chars); 
						tSize2.cx = tSize2.cy = 0;
					}
					idPlace idP2 = {x, y, x+1, y+1};
					idlist.Add(idP2);
					idPlace idP = {x-offset, y+offset/2, x-offset+tSize.cx+4, y+offset/2+tSize.cy+tSize2.cy};
					idPlacement(x, y, idP, &idlist);
					CRect box = CRect(idP.x, idP.y, idP.x+tSize.cx+4, idP.y+tSize.cy+tSize2.cy);
					pDC->Rectangle(box);
					CPoint closest;  //find closest corner
					int cornerDist = (int) closestCorner(CPoint(x,y), box, closest);
					if (cornerDist>vorsize) 
						drawDist(pDC, cornerDist-vorsize, closest, CPoint(x,y));
					pDC->TextOut(idP.x+2, idP.y, buf);
					if (!freq || id) 
						pDC->TextOut(idP.x+2, idP.y+tSize2.cy, buf2);
					pDC->SelectObject(oldBrush);
				}
			} else {
				if (id || freq) {
					if (freq && id)
						sprintf(buf, "%s (%6.2f)", pDoc->db[pDoc->vorIndex[i]].id, pDoc->db[pDoc->vorIndex[i]].freq);
					else if (id)
						sprintf(buf, "%s", pDoc->db[pDoc->vorIndex[i]].id);
					else if (freq)
						sprintf(buf, "%6.2f", pDoc->db[pDoc->vorIndex[i]].freq);
					pDC->TextOut(x, y+offset+4, buf);
				}
			}
		}
		pDC->SelectObject(oldBrush);
		pDC->SelectObject(oldPen);
	}
	
	if (ils) {	
		if (pDC->IsPrinting() && forceBlackPrinting) { // red does not print well
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
		} else { 
			oldPen = pDC->SelectObject(&redPen);
		}
		for (i=0; i<pDoc->ilsIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->ilsIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SelectObject(oldPen);
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}

			int x,y;
			if (!LtoC(pDoc->db[pDoc->ilsIndex[i]].lon, pDoc->db[pDoc->ilsIndex[i]].lat, x, y)) continue;

			// draw a 3 by 3 pixel diagonal cross
			pDC->MoveTo(x-1, y-1);
			pDC->LineTo(x+3, y+3);
			pDC->MoveTo(x+2, y-1);
			pDC->LineTo(x-2, y+3);
			if (mag > symbolTransition) {
				double ldist; 
				int l = distToPixels(2000); // l = 2km
				int lx = (int) (l * sin(pDoc->db[pDoc->ilsIndex[i]].approach * degToRad));
				int ly = (int) (l * -cos(pDoc->db[pDoc->ilsIndex[i]].approach * degToRad));
 				if(flatWorld) { // calculate latitude distortion due to flat map
					ldist = 1.0/cos(pDoc->db[pDoc->ilsIndex[i]].lat * degToRad);
					lx = (int) (lx * ldist);
				}
				pDC->MoveTo(x, y);
				pDC->LineTo(x-lx-ly/10, y-ly+lx/10);
				pDC->LineTo(x-lx+ly/10, y-ly-lx/10);
				pDC->LineTo(x, y);
				if (id || freq) {
					int chars;
					CSize tSize; 
					if (freq && id)
						chars = sprintf(buf, "%s (%6.2f)", pDoc->db[pDoc->ilsIndex[i]].id, pDoc->db[pDoc->ilsIndex[i]].freq);
					else if (id)
						chars = sprintf(buf, "%s", pDoc->db[pDoc->ilsIndex[i]].id);
					else if (freq)
						chars = sprintf(buf, "%6.2f", pDoc->db[pDoc->ilsIndex[i]].freq);
					tSize = pDC->GetOutputTextExtent(buf, chars);
					int cx = (int) (x-lx);
					int cy = (int) (y-ly);
					idPlace idP = {(int) (x-1.5*lx), (int) (y-1.5*ly), (int) (x-1.5*lx)+tSize.cx+4, (int) (y-1.5*ly)+tSize.cy};
					idPlacement(cx, cy, idP, &idlist, 1);
					pDC->Rectangle(idP.x, idP.y, idP.x+tSize.cx+4, idP.y+tSize.cy);
					// find closest corner
					int ccx = idP.x;
					int ccy = idP.y;
					double dn, d = sqrt((double)(idP.x-cx)*(idP.x-cx)+(idP.y-cy)*(idP.y-cy));
					if ((dn = sqrt((double)(idP.x-cx)*(idP.x-cx)+(idP.y+tSize.cy-cy)*(idP.y+tSize.cy-cy))) < d) {
						d = dn;
						ccx = idP.x;
						ccy = idP.y+tSize.cy;
					}
					if ((dn = sqrt((double)(idP.x+tSize.cx+4-cx)*(idP.x+tSize.cx+4-cx)+(idP.y-cy)*(idP.y-cy))) < d) {
						d = dn;
						ccx = idP.x+tSize.cx+4;
						ccy = idP.y;
					}
					if ((dn = sqrt((double)(idP.x+tSize.cx+4-cx)*(idP.x+tSize.cx+4-cx)+(idP.y+tSize.cy-cy)*(idP.y+tSize.cy-cy))) < d) {
						d = dn;
						ccx = idP.x+tSize.cx+4;
						ccy = idP.y+tSize.cy;
					}
					pDC->MoveTo(cx, cy);
					pDC->LineTo(ccx, ccy);
					pDC->TextOut(idP.x+2, idP.y, buf);
				}
			}
		}	
		pDC->SelectObject(oldPen);
	}	
		
	if (airports && (mag > symbolTransition)) {	// Runway
		for (i=0; i<pDoc->runwayIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->runwayIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}

			int x,y;
			if (!LtoC(pDoc->db[pDoc->runwayIndex[i]].lon, pDoc->db[pDoc->runwayIndex[i]].lat, x, y)) continue;

			double ldist;
			int l = distToPixels(pDoc->db[pDoc->runwayIndex[i]].range * 0.3048);
			int lx = (int) ((double) l * sin(pDoc->db[pDoc->runwayIndex[i]].approach * degToRad));
			int ly = (int) ((double) l * -cos(pDoc->db[pDoc->runwayIndex[i]].approach * degToRad));
 			if(flatWorld) { // calculate latitude distortion due to flat map
				ldist = 1.0/cos(pDoc->db[pDoc->runwayIndex[i]].lat * degToRad);
				lx = (int) (lx * ldist);
			}

			if (OS != 3) { // not Win32s (it can't handle the wide pen for some reason)
				// Pen for runway width (when mag > 10)
				int rwWidth = 1;
				if (mag > 10) 
					rwWidth = max(1, distToPixels(pDoc->db[pDoc->runwayIndex[i]].magdev * 0.3048));
				LOGBRUSH lb;
				lb.lbStyle = BS_SOLID;
				lb.lbColor = RGB(0, 0, 0);
				CPen widePen(PS_GEOMETRIC | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL, rwWidth, &lb);
				
				if (mag > 10) 
					oldPen = (CPen *) pDC->SelectObject(&widePen);	
				else 
					oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);	// switch to black
				pDC->MoveTo(x-lx/2, y-ly/2);		// lat and lon are center of runway
				pDC->LineTo(x+lx/2, y+ly/2);
				pDC->SelectObject(oldPen);

			} else {  // OS is Win32s (no wide runways)
				oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);	// switch to black
				pDC->MoveTo(x-lx/2, y-ly/2);		// lat and lon are center of runway
				pDC->LineTo(x+lx/2, y+ly/2);
				pDC->SelectObject(oldPen);
			}
		}	
	}
	
	if (airports && id && (mag > symbolTransition)) {	// airport name
		for (i=0; i<pDoc->airportIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->airportIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}

			int x,y;
			if (!LtoC(pDoc->db[pDoc->airportIndex[i]].lon, pDoc->db[pDoc->airportIndex[i]].lat, x, y)) continue;
			int chars;
			CSize tSize;
			int l = distToPixels(2500);	// l = 2.5km
			l = max(l, 10);
			chars=sprintf(buf, "%s", pDoc->db[pDoc->airportIndex[i]].name);
			tSize = pDC->GetOutputTextExtent(buf, chars); 
			idPlace idP = {x, y+l+1, x+tSize.cx, y+l+1+tSize.cy};
			idPlacement(x, y, idP, &idlist);
			pDC->TextOut(idP.x, idP.y, buf);
		}
	}

	if (markers && (mag > symbolTransition)) {	// inner, middle and outer markers
		oldBrush = (CBrush *) pDC->SelectStockObject(LTGRAY_BRUSH);
		for (i=0; i<pDoc->markerIndex.GetSize(); i++) {
			if (planOnly) {
				int proceed = FALSE;
				for (int q=0; q<plancount; q++)
					if (pDoc->markerIndex[i] == plan[q]) {
						proceed = TRUE;
						break;
				}
				if (!proceed)
					continue;
			}

			if (!pDC->IsPrinting()) {
				if (drawMessageLoop() == TRUE) { // clean up and exit
					pDC->SelectObject(oldBrush);
					pDC->SetBkMode(oldMode);
					pDC->RestoreDC(dcNumber);
					clipper.DeleteObject();
					pDC->SelectClipRgn(NULL);
					drawing = FALSE;
					LeaveCriticalSection(&crit);
					return;
				}
			}

			int x,y;
			if (!LtoC(pDoc->db[pDoc->markerIndex[i]].lon, pDoc->db[pDoc->markerIndex[i]].lat, x, y)) continue;

			if (5 == pDoc->db[pDoc->markerIndex[i]].type)
				oldPen = pDC->SelectObject(&orangePen);
			else if (6 == pDoc->db[pDoc->markerIndex[i]].type)
				oldPen = pDC->SelectObject(&greenPen);
			else if (7 == pDoc->db[pDoc->markerIndex[i]].type)
				oldPen = pDC->SelectObject(&redPen);
			pDC->Rectangle(CRect(x-2, y-2, x+2, y+2));
			pDC->SelectObject(oldPen);
		} 
 		pDC->SelectObject(oldBrush);
	}	

	if (pDC->IsPrinting() && forceBlackPrinting) { // orange does not print well
		oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
	} else { 
		oldPen = pDC->SelectObject(&orangePen);
	}
	for (i=0; i<pDoc->gpsIndex.GetSize(); i++) {		// gps
		if (planOnly) {
			int proceed = FALSE;
			for (int q=0; q<plancount; q++)
				if (pDoc->gpsIndex[i] == plan[q]) {
					proceed = TRUE;
					break;
			}
			if (!proceed)
				continue;
		}

		if (!pDC->IsPrinting()) {
			if (drawMessageLoop() == TRUE) { // clean up and exit
				pDC->SelectObject(oldPen);
				pDC->SetBkMode(oldMode);
				pDC->RestoreDC(dcNumber);
				clipper.DeleteObject();
				pDC->SelectClipRgn(NULL);
				drawing = FALSE;
				LeaveCriticalSection(&crit);
				return;
			}
		}

		int x,y;
		if (!LtoC(pDoc->db[pDoc->gpsIndex[i]].lon, pDoc->db[pDoc->gpsIndex[i]].lat, x, y)) continue;

		pDC->MoveTo(x, y-5);
		pDC->LineTo(x, y+5);
		pDC->MoveTo(x-5, y);
		pDC->LineTo(x+5, y);
		pDC->MoveTo(x-5, y);
		pDC->LineTo(x, y-5);
		pDC->LineTo(x+5, y);
		pDC->LineTo(x, y+5);
		pDC->LineTo(x-5, y);
		if (id) {
			sprintf(buf, "GPS fix %s", pDoc->db[pDoc->gpsIndex[i]].id);
			pDC->TextOut(x-5, y+6, buf);
		}
	}
	pDC->SelectObject(oldPen);

	pDC->SelectObject(oldFont);
	if (title != "") {
		oldFont = (CFont *) pDC->SelectObject(&titleFont);
		int chars;
		chars = sprintf(buf, "%s", title);
		CSize tSize = pDC->GetOutputTextExtent(buf, chars); 
		pDC->TextOut(screenWidth/2-tSize.cx/2, 1, buf); // center top
		pDC->SelectObject(oldFont);
	}
 
	if (flying) { // draw FlyTo target
		int x,y;
		if (LtoC(flyingLon, flyingLat, x, y)) {
			CBrush redBrush(RGB(255, 0, 0));
			oldPen = pDC->SelectObject(&redPen);
			oldBrush = (CBrush *) pDC->SelectObject(&redBrush);
			pDC->Ellipse(x-8, y-8, x+8, y+8);
			pDC->SelectObject(oldBrush);
			oldBrush = (CBrush *) pDC->SelectStockObject(WHITE_BRUSH);
			pDC->Ellipse(x-6, y-6, x+6, y+6);
			pDC->SelectObject(oldBrush);
			oldBrush = (CBrush *) pDC->SelectObject(&redBrush);
			pDC->Ellipse(x-3, y-3, x+3, y+3);
			pDC->SelectObject(oldBrush);
			pDC->SelectObject(oldPen);
		}
	}

	if (flightPaths > 0) { // draw imported flight path
		int x, y;
		if (pDC->IsPrinting() && forceBlackPrinting) { // red does not print well
			oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
		} else { 
			oldPen = pDC->SelectObject(&redPen);
		}
		int starttime = flightTrackDensity;
	  	for (int k=0; k<fltPath.GetSize(); k++) {
			double lat = fltPath[k].lat;
			double lon = fltPath[k].lon;
			LtoC(lon, lat, x, y);
			if (k == 0)
				pDC->MoveTo(x, y);
			else {
				if (k >= starttime) {
					starttime = k + flightTrackDensity;
					pDC->LineTo(x, y);
				}
			}
		}
		pDC->SelectObject(oldPen);
	}

	if (!pDC->IsPrinting()) { // draw center mark on screen
		int centerX = screenWidth/2;
		int centerY = screenHeight/2;
		pDC->MoveTo(centerX-2, centerY);
		pDC->LineTo(centerX+3, centerY);
		pDC->MoveTo(centerX, centerY-2);
		pDC->LineTo(centerX, centerY+3);
	}

	if (id) {
		idlist.RemoveAll();
		idlist.FreeExtra();
	}

	clipper.DeleteObject();
	pDC->SelectClipRgn(NULL);

	pDC->SetBkMode(oldMode);
	pDC->RestoreDC(dcNumber);

	LocationBar();
	drawn.deleteAll();
	drawing = FALSE;
	LeaveCriticalSection(&crit);
	if (linked || networkInitialized) 
		PostMessage(WM_COMMAND, ID_INITIATELINKDRAWING, 0);

	if (init) {
		init = FALSE;
		if (tryToLink && !linked)  // start link thread only after initial drawing is done
			hthread = AfxBeginThread(LinkThread, this)->m_hThread;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNavView printing

BOOL CNavView::OnPreparePrinting(CPrintInfo* pInfo) {
	pInfo->SetMinPage(1);
	pInfo->SetMaxPage(1);
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CNavView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add extra initialization before printing
}

void CNavView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CNavView diagnostics

#ifdef _DEBUG
void CNavView::AssertValid() const {
	CView::AssertValid();
}

void CNavView::Dump(CDumpContext& dc) const	{
	CView::Dump(dc);
}

CNavDoc* CNavView::GetDocument() { // non-debug version is inline
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNavDoc)));
	return (CNavDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNavView message handlers

void CNavView::OnViewZoomin() {
	double xd = (right - left) / 4.;
	double yd = (top - bottom) / 4.;
	right -= xd;
	left += xd;
	top -= yd;
	bottom += yd;

	convX *= 2;
	convY *= 2;

	mag += 1;
	mag2 *= 2;
	LocationBar();
	Mready = FALSE;
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);	
}

void CNavView::OnUpdateViewZoomout(CCmdUI* pCmdUI) {
	pCmdUI->Enable(mag != 1);		
}

void CNavView::OnViewZoomout() {
	if(mag > 1) {
		double xd = (right - left) / 2.;
		double yd = (top - bottom) / 2.;
		right += xd;
		left -= xd;
		top += yd;
		bottom -= yd;

		convX /= 2;
		convY /= 2;

		mag -= 1;
		mag2 /= 2;
		LocationBar();
		Mready = FALSE;
		abort = TRUE; // abort old redraw if necessary
		Invalidate(TRUE);
	}	
}

void CNavView::OnLButtonDown(UINT nFlags, CPoint point) {
	double rx, ry;
	if (!CtoL(point.x, point.y, rx, ry))
		return;
	double yd = (top + bottom)/2.0 - ry;
	double xd = (right + left)/2.0 - rx;
	left -= xd;
	right -= xd;
	top -= yd;
	bottom -= yd;

	centerLat = ry;
	centerLon = rx;

	LocationBar();
	Mready = FALSE;
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}


void CNavView::OnLButtonDblClk(UINT nFlags, CPoint point) {
	OnLButtonDown(nFlags, point); // same as single click
}

void CNavView::OnSize(UINT nType, int cx, int cy) {
	CView::OnSize(nType, cx, cy);
	Size(cx, cy);

	Mready = FALSE;
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnViewGridlines() {
	grid = !grid;
	char buffer[30];
	sprintf(buffer, "%d", grid);
	WritePrivateProfileString("View", "grid", buffer, "nav.ini");
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewGridlines(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(grid);
}

void CNavView::OnViewNdb() {
	ndb = !ndb;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewNdb(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(ndb);	
}

void CNavView::OnViewMarkers() {
	markers = !markers;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewMarkers(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(markers);	
}

void CNavView::OnViewWaypoints() {
	waypoints = !waypoints;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewWaypoints(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(waypoints);	
}

void CNavView::OnViewRoutes() {
	routes = !routes;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewRoutes(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(routes);	
}

void CNavView::OnMapAtis() {
	atis = !atis;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateMapAtis(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(atis);	
}

void CNavView::OnViewIls() {
	ils = !ils;
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewIls(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(ils);	
}

void CNavView::OnViewVor() {
	vor = !vor;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewVor(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(vor);	
}

void CNavView::OnViewAirports() {
	airports = !airports;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewAirports(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(airports);	
}

void CNavView::OnViewId() {
	id = !id;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewId(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(id);	
}

void CNavView::OnViewFrequency() {
	freq = !freq;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewFrequency(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(freq);	
}

void CNavView::OnEditChangefont() {
	CFontDialog d(&lf, CF_SCREENFONTS|CF_INITTOLOGFONTSTRUCT, NULL, this);
	if (d.DoModal() == IDOK) {
		// asserts in debug mode - I don't know why, it seems to work
		d.GetCurrentFont(&lf);
		Invalidate(TRUE);
	}
}

void CNavView::Size(int cx, int cy) {
	if ((cx == 0) && (cy == 0)) // happens during initialization
		return;
	convX = convY = mag2;
	if (cx > cy) 
		convX *= cy / (double) cx;
	else 
		convY *= cx / (double) cy;

	if (!flatWorld) 
		return;

	double xcent = (right + left)/2.0;
	double ycent = (top + bottom)/2.0;
	double ratio = (double) cx / cy;

	// proportion correctly
	if (ratio > 2) {
		top = 90;
		bottom = -90;
		left = -top * ratio;
		right = top * ratio;		
	} else {
		left = -180;
		right = 180;		
		top = right / ratio;
		bottom = -right / ratio;
	}
	centerLat = ycent;
	centerLon = xcent;

	// scale
	for (int i=1; i<mag; i++) {
		double xd = (right - left) / 4.;
		double yd = (top - bottom) / 4.;
		right -= xd;
		left += xd;
		top -= yd;
		bottom += yd;
	}

	// center
	double xd = (right + left) / 2.;
	double yd = (top + bottom) / 2.;
	right += xcent-xd;
	left += xcent-xd;
	top += ycent-yd;
	bottom += ycent-yd;
}

void CNavView::OnEditSettitle() {
	SetTitle d;	

	d.m_title = title;
	if (d.DoModal() == IDOK) {
		title = d.m_title;
		Invalidate(TRUE);
	}
}

void CNavView::OnViewSearch() {
	DataDialog d;	
	Search d2;
	
	if (d2.DoModal() == IDOK) {
		search = d2.m_target;
		if (search != "")	
			d.DoModal();
	} 
	search = "";
}

void CNavView::OnViewDatabase() {
	DataDialog d;	
	d.DoModal();
}

void CNavView::OnEditSaveoptions() {
	char buffer[50];
/*	sprintf(buffer, "%f", top);
	WritePrivateProfileString("View", "top", buffer, "nav.ini");
	sprintf(buffer, "%f", bottom);
	WritePrivateProfileString("View", "bottom", buffer, "nav.ini");
	sprintf(buffer, "%f", right);
	WritePrivateProfileString("View", "right", buffer, "nav.ini");
	sprintf(buffer, "%f", left);
	WritePrivateProfileString("View", "left", buffer, "nav.ini");
	sprintf(buffer, "%d", mag);
	WritePrivateProfileString("View", "mag", buffer, "nav.ini");
	sprintf(buffer, "%d", mag2);
	WritePrivateProfileString("View", "mag2", buffer, "nav.ini");
*/
	sprintf(buffer, "%d", routes);
	WritePrivateProfileString("View", "routes", buffer, "nav.ini");
	sprintf(buffer, "%d", waypoints);
	WritePrivateProfileString("View", "waypoints", buffer, "nav.ini");
	sprintf(buffer, "%d", largeSymbols);
	WritePrivateProfileString("View", "largeSymbols", buffer, "nav.ini");
	sprintf(buffer, "%d", grid);
	WritePrivateProfileString("View", "grid", buffer, "nav.ini");
	sprintf(buffer, "%d", vor);
	WritePrivateProfileString("View", "vor", buffer, "nav.ini");
	sprintf(buffer, "%d", ils);
	WritePrivateProfileString("View", "ils", buffer, "nav.ini");
	sprintf(buffer, "%d", ndb);
	WritePrivateProfileString("View", "ndb", buffer, "nav.ini");
	sprintf(buffer, "%d", airports);
	WritePrivateProfileString("View", "airports", buffer, "nav.ini");
	sprintf(buffer, "%d", markers);
	WritePrivateProfileString("View", "markers", buffer, "nav.ini");
	sprintf(buffer, "%d", atis);
	WritePrivateProfileString("View", "atis", buffer, "nav.ini");
	sprintf(buffer, "%d", waypoints);
	WritePrivateProfileString("View", "waypoints", buffer, "nav.ini");
	sprintf(buffer, "%d", routes);
	WritePrivateProfileString("View", "routes", buffer, "nav.ini");
	sprintf(buffer, "%d", id);
	WritePrivateProfileString("View", "id", buffer, "nav.ini");
	sprintf(buffer, "%d", freq);
	WritePrivateProfileString("View", "freq", buffer, "nav.ini");
	sprintf(buffer, "%d", flatWorld);
	WritePrivateProfileString("View", "flatWorld", buffer, "nav.ini");
	sprintf(buffer, "%d", planVOR);
	WritePrivateProfileString("View", "planVOR", buffer, "nav.ini");
	sprintf(buffer, "%d", planNDB);
	WritePrivateProfileString("View", "planNDB", buffer, "nav.ini");
	sprintf(buffer, "%d", planAirport);
	WritePrivateProfileString("View", "planAirport", buffer, "nav.ini");
	sprintf(buffer, "%d", planWaypoints);
	WritePrivateProfileString("View", "planWaypoints", buffer, "nav.ini");
	sprintf(buffer, "%d", planContinue);
	WritePrivateProfileString("View", "planContinue", buffer, "nav.ini");
	sprintf(buffer, "%f", idealDist);
	WritePrivateProfileString("View", "idealDist", buffer, "nav.ini");
	sprintf(buffer, "%d", showAirportZones);
	WritePrivateProfileString("View", "showAirportZones", buffer, "nav.ini");
	sprintf(buffer, "%d", showMaps);
	WritePrivateProfileString("View", "showMaps", buffer, "nav.ini");
	sprintf(buffer, "%d", showFancyMaps);
	WritePrivateProfileString("View", "showFancyMaps", buffer, "nav.ini");
	sprintf(buffer, "%d", link);
	WritePrivateProfileString("View", "Link", buffer, "nav.ini");
	sprintf(buffer, "%lf", linkUpdateTime);
	WritePrivateProfileString("View", "linkUpdateTime", buffer, "nav.ini");
	sprintf(buffer, "%lf", linkFlyUpdateTime);
	WritePrivateProfileString("View", "linkFlyUpdateTime", buffer, "nav.ini");
	sprintf(buffer, "%d", linkFlySetMode);
	WritePrivateProfileString("View", "linkFlySetMode", buffer, "nav.ini");
	sprintf(buffer, "%d", flightTrackDensity);
	WritePrivateProfileString("View", "flightTrackDensity", buffer, "nav.ini");
	sprintf(buffer, "%d", forceBlackPrinting);
	WritePrivateProfileString("View", "forceBlackPrinting", buffer, "nav.ini");
	sprintf(buffer, "%d", enableNetwork);
	WritePrivateProfileString("View", "enableNetwork", buffer, "nav.ini");
	WritePrivateProfileString("View", "callsign", callsign, "nav.ini");
}

void CNavView::OnRButtonDblClk(UINT nFlags, CPoint point) {
	// find the nearest thing in the database and show details
	CNavDoc* doc = GetDocument();
	measuring = FALSE;
	double startDist = gcDist(top, left, bottom, right);
	int index = SearchNavAidNearPoint(startDist, point);

	if (index != -1) {
		CString details = "";
		doc->formatData(details, index);
		Details d;
		d.index = index;
		d.m_text = details;
		d.DoModal();
	}

	CView::OnRButtonDblClk(nFlags, point);
}

// great circle distance (in meters)
double CNavView::gcDist(double lat1, double lon1, double lat2, double lon2) {
	double dlon = (lon2-lon1) * degToRad;
	lat1 *= degToRad;
	lat2 *= degToRad;

	// calculate bearing in case anyone cares 
	bearing = atan2(sin(dlon), (-sin(lat1)*cos(dlon) + tan(lat2)*cos(lat1))) / degToRad;
	if (bearing < 0)
		bearing += 360;

	// calculate distance 
	// bad algorithm
 	// return(earthRadius*acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(dlon)));

	// good algorithm
	double la = sin((lat2-lat1)/2);
	double lo = sin(dlon/2);
	double x = la*la + cos(lat1) * cos(lat2) * lo*lo;
	return(2*earthRadius*asin(sqrt(x)));
}

void CNavView::gcDistInterpolate(double lat1, double lon1, double lat2, double lon2, double percent, double &lat3, double &lon3) {
	double radius = 180 * degToMeters / 3.1415926536;
	double x1, y1, z1, x2, y2, z2;
	StoC(y1, z1, x1, lat1, lon1, radius);
	StoC(y2, z2, x2, lat2, lon2, radius);
	// rough approximation for now
	CtoS(y1+(y2-y1)*percent, z1+(z2-z1)*percent, x1+(x2-x1)*percent, lat3, lon3, radius);
}

// great circle distance (in meters) and bearing (in radians), and projection (meters)
int CNavView::gcDistDraw(double lat1, double lon1, double lat2, double lon2, double &dist, double &bearing, double &proj) {
	double dlon = (lon2-lon1) * degToRad;
	lat1 *= degToRad;
	lat2 *= degToRad;
	double la = sin((lat2-lat1)/2);
	double lo = sin(dlon/2);
	double x = sqrt(la*la + cos(lat1) * cos(lat2) * lo*lo); // sin (half-angle subtended)
	double a = asin(x);
	dist = 2*earthRadius*a;
	if (dist > dmax)
		return FALSE;	
	proj = earthRadius*sin(2*a);
	bearing = atan2(sin(dlon), (-sin(lat1)*cos(dlon) + tan(lat2)*cos(lat1)));
	return(TRUE);
}

// spherical to cartesian conversion (y=top, x=right, z=out-of-screen)
void CNavView::StoC(double &x, double &y, double &z, double lat, double lon, double r) {
	z = r * cos(lon * degToRad) * sin((90 - lat) * degToRad);
	x = r * sin(lon * degToRad) * sin((90 - lat) * degToRad);
	y = r * cos((90 - lat) * degToRad); 
}

// cartesian to spherical conversion (y=top, x=right, z=out-of-screen)
void CNavView::CtoS(double x, double y, double z, double &lat, double &lon, double &r) {
	lon = atan2(x, z) / degToRad;
	r = sqrt(x*x + y*y + z*z);
	lat = 90 - acos(y/r) / degToRad;
}

// spherical to cartesian conversion for normalized radius (y=top, x=right, z=out-of-screen)
void CNavView::StoCe(double &x, double &y, double &z, double lat, double lon) {
	z = cos(lon * degToRad) * sin((90 - lat) * degToRad);
	x = sin(lon * degToRad) * sin((90 - lat) * degToRad);
	y = cos((90 - lat) * degToRad); 
}

// midpoint of great circle line
void CNavView::midPoint(double slat, double slon, double elat, double elon, double & mlat, double & mlon) {
	double bx, by, bz, ex, ey, ez, dummy;
	StoCe(bx, by, bz, slat, slon);
	StoCe(ex, ey, ez, elat, elon);
	CtoS((bx+ex)/2, (by+ey)/2, (bz+ez)/2, mlat, mlon, dummy);
}

// draw a line along the great circle between two screen points (lat-lon pairs)
void CNavView::gcLine(int x1, int y1, int x2, int y2, CDC *pdc, int black) {
	if ((x1==x2) &&(y1==y2))
		return;
	CDC *dc;
	if (pdc == NULL)
		dc = GetDC();
	else
		dc = pdc;
	CPen *oldPen;
	int oldROP;
	if (black) {
		oldPen = (CPen *) dc->SelectStockObject(BLACK_PEN);
		oldROP = dc->SetROP2(R2_NOTXORPEN);
	}
	dc->MoveTo(x1, y1);
	double slat;
	double slon;
	CtoL(x1, y1, slon, slat);
	double elat;
	double elon;
	CtoL(x2, y2, elon, elat);
	double radius = 180 * degToMeters / 3.1415926536;
	double bx, by, bz, ex, ey, ez, dx, dy, dz, px, py, pz, plat, plon, oldx;
	int res = 3;
	int pointx, pointy;
	StoC(by, bz, bx, slat, slon, radius);
	StoC(ey, ez, ex, elat, elon, radius);
	int d = (int) sqrt((double)(x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
	int steps = d/res;
	oldx = 0;
	bearing = 0;
	for (int i=1; i<steps; i++) {
		dx = (ex - bx)/steps;
		dy = (ey - by)/steps;
		dz = (ez - bz)/steps;
		px = bx+i*dx;
		py = by+i*dy;
		pz = bz+i*dz;
		CtoS(py, pz, px, plat, plon, radius);
		if (i==1) {	// calculate bearing in case anyone cares 
			bearing = atan2(plon-slon, plat-slat) / degToRad;
			if (bearing < 0)
				bearing += 360;
		}
		LtoC(plon, plat, pointx, pointy);
		if ((fabs(plon - oldx) > 100) && (flatWorld)) 
			dc->MoveTo(pointx, pointy);	// skip across the map edge
		else
			dc->LineTo(pointx, pointy);
		oldx = plon;
	} 
	dc->LineTo(x2, y2);
	if (black) {
		dc->SelectObject(oldPen);
		dc->SetROP2(oldROP);
	}
	if (pdc == NULL)
		ReleaseDC(dc);
}

// part of waypoint "magnetism" code by Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>
void CNavView::OnRButtonDown(UINT nFlags, CPoint point) {
	measuring = TRUE;
	fromNavAid = FALSE;
	// look for a navaid not far from here
	double startDist;
	double startLat;
	double startLong;
	double endLat;
	double endLong;

	CtoL(point.x - 5, point.y - 5, startLong, startLat);
	CtoL(point.x + 5, point.y + 5, endLong, endLat);

	startDist = gcDist(startLat,startLong,endLat,endLong);

	navAidIndex = SearchNavAidNearPoint(startDist,point);
	imx = omx = point.x;
	imy = omy = point.y;	
	if (navAidIndex > 0) {
		fromNavAid = TRUE;
		int x,y;
		CNavDoc* pDoc = GetDocument();

		if (LtoC(pDoc->db[navAidIndex].lon, pDoc->db[navAidIndex].lat, x, y)) {
			imx = omx = x;
			imy = omy = y;	
		}
	}
}

// part of waypoint "magnetism" code by Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>
void CNavView::OnRButtonUp(UINT nFlags, CPoint point) {
	measuring = FALSE; 
	gcLine(imx, imy, omx, omy, NULL); // undraw old line
	LocationBar();					  // reset status bar

	// if navaid not far don't ask for a fix
	if (fromNavAid) {
		double startDist;
		double startLat;
		double startLong;
		double endLat;
		double endLong;

		CtoL(point.x - 5, point.y - 5, startLong, startLat);
		CtoL(point.x + 5, point.y + 5, endLong, endLat);

		startDist = gcDist(startLat,startLong,endLat,endLong);

		int index;

		index = SearchNavAidNearPoint(startDist,point);
		imx = omx = point.x;
		imy = omy = point.y;	
		if (index < 0) {
			InsertGpsFixDial gpsFixDial;
			CNavDoc* pDoc = GetDocument();

			// ok ask if we want to insert a fix
			double lon2,lat2;
			if (CtoL(point.x, point.y, lon2, lat2)) {
				CString navAidDetails;
				pDoc->formatData(navAidDetails,navAidIndex);
				gpsFixDial.m_navaidDetails = navAidDetails;
				gpsFixDial.m_gpsfix_distance = gcDist( pDoc->db[navAidIndex].lat,pDoc->db[navAidIndex].lon, lat2, lon2) / 1852;
				//bearing = (int)pDoc->GetMagVar(pDoc->db[navAidIndex].lat,pDoc->db[navAidIndex].lon);
				if (bearing > 360)
					bearing -= 360;
				if (bearing < 0)
					bearing += 360;
				gpsFixDial.m_gpsfix_heading = bearing;
				gpsFixDial.fromLat =  pDoc->db[navAidIndex].lat;
				gpsFixDial.fromLong =  pDoc->db[navAidIndex].lon;
				gpsFixDial.DoModal();
			}
		}
	}
	fromNavAid = FALSE;
}

// part of waypoint "magnetism" code by Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>
void CNavView::ConvertDistHeadToCoord(double distance,double heading, double& latitude, double& longitude) {
	double deltaLat;
	distance = distance / 1852.0;
	deltaLat = distance * sin((90-heading)* degToRad);

	longitude = longitude + ((distance * cos((90-heading)* degToRad))/(cos((latitude + deltaLat/120)*degToRad)))/60;
	latitude = latitude + (deltaLat/60);
}

// part of waypoint "magnetism" code by Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>
int CNavView::SearchNavAidNearPoint(double startDist, CPoint point) {
	CNavDoc* doc = GetDocument();
	double clat;
	double clon;
	if (!CtoL(point.x, point.y, clon, clat))
		return -1;

	int found = FALSE;
	int index, d1, d2;
	double dist;
		
	for (int i=0; i<doc->dbCount; i++) {
		if ((doc->db[i].type == 1)  && (!ndb)) goto next;
		if ((doc->db[i].type == 2)  && (!ils)) goto next;
		if ((doc->db[i].type == 3)  && (!vor)) goto next;
		if ((doc->db[i].type == 4)  && (!airports)) goto next;
		if ((doc->db[i].type == 5)  && (!markers)) goto next;
		if ((doc->db[i].type == 6)  && (!markers)) goto next;
		if ((doc->db[i].type == 7)  && (!markers)) goto next;
		if ((doc->db[i].type == 8)  && (!airports)) goto next;
		if ((doc->db[i].type == 10) && (!atis)) goto next;
		if  (doc->db[i].type == 11) goto next;
		if  (doc->db[i].type == 12) goto next;
		if ((doc->db[i].type == 13) && (!waypoints)) goto next;
		if ((doc->db[i].type == 14) && (!routes))  goto next;
		// limit to window being viewed	 
		if (flatWorld) {
			if ((doc->db[i].lon) < left) goto next;
			if ((doc->db[i].lon) > right) goto next;
			if ((doc->db[i].lat) < bottom) goto next;
			if ((doc->db[i].lat) > top) goto next;
		} else 
			if(LtoC(doc->db[i].lon, doc->db[i].lat, d1, d2) == FALSE) goto next;
		
		if (doc->db[i].type == 14) { // search for center of route
			double la = (doc->db[i].lat + doc->db[i].approach) / 2.0;
			double lo = (doc->db[i].lon + doc->db[i].range) / 2.0;
			dist = gcDist(clat, clon, la, lo);
		} else
			dist = gcDist(clat, clon, doc->db[i].lat, doc->db[i].lon);
		if (dist < startDist) {
			startDist = dist;
			found = TRUE;
			index = i;
		}
		next:;
	}

	if (found)
		return index;
	else
		return -1;
}

// originally part of waypoint "magnetism" code by Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>
BOOL CNavView::SendToPlan(int index) {
	if (plancount >= MAXPLAN-2) {
		AfxMessageBox("Too many nav-aids in plan");
		return FALSE;
	}
	
	if (planning) { // see if we should insert after or append to end
		planD->update();
		if (plancount == 0) {
			plan[0] = index;
			plancount = 1;
			planD->selectPlanItem(0);
		} else {
			int selected = planD->getSel();// find the selected plan item, if any
			if (selected == -1) // nothing was selected
				selected = plancount-1;
			else
				selected = planD->selToPlan(selected);
			for (int i=plancount-1; i>selected; i--)
				plan[i+1] = plan[i];
			plan[selected+1] = index;
			plancount += 1;
			// select the newly inserted waypoint
			planD->selectPlanItem(selected+1);
		}
		Invalidate(TRUE);
	} else { // just append
		plan[plancount] = index;
		plancount += 1;
	}
	return TRUE;
}


void CNavView::OnMouseMove(UINT nFlags, CPoint point) {
	if (measuring) {
		CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
		double dist, lat1, lon1, lat2, lon2, magv;
		if (!CtoL(imx, imy, lon1, lat1))
			return;
		if (!CtoL(point.x, point.y, lon2, lat2))
			return;
		gcLine(imx, imy, omx, omy, NULL);		   // undraw old line
		gcLine(imx, imy, point.x, point.y, NULL);  // draw new line
		if ((imy == point.y) &&	(imx == point.x))
			dist = 0;
		else
			dist = gcDist(lat1, lon1, lat2, lon2);
		CNavDoc* doc = GetDocument();
		magv = doc->GetMagVar(lat1, lon1) + bearing;
		if (magv > 360.0)
			magv -= 360.0;
		if (magv < 0.0)
			magv += 360.0;

		// display on status bar
		char buffer[200];
		sprintf(buffer, "(% 8.6f, % 8.6f) to (% 8.6f, % 8.6f), %10.1f nm,  %1.1f degrees (%1.1f magnetic)", lat1, lon1, lat2, lon2, dist/1852, bearing, magv);
		main->SetStatusBar(buffer);

		omx = point.x;
		omy = point.y;
	}
}

void CNavView::OnViewFlatworld(){
	flatWorld = !flatWorld;	
	Size(screenWidth, screenHeight);
	abort = TRUE; // abort old redraw if necessary
	Mready = FALSE;
	Invalidate(TRUE);
}

void CNavView::OnUpdateViewFlatworld(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(flatWorld);	 
}

void CNavView::OnEditFlightplan() {
	if (planning) {
		planning = FALSE;
		planD->DestroyWindow();	// let it deallocate its own memory
	} else {
		planning = TRUE;
		planD = new FlightPlan;	// corresponding delete is in FlightPlan::PostNcDestroy()
		planD->Create(IDD_FLIGHTPLAN, CWnd::GetDesktopWindow());	
	}
	if (plancount > 0) {
		abort = TRUE; // abort old redraw if necessary
		Invalidate(TRUE);
	}
}

void CNavView::OnUpdateEditFlightplan(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(planning);
}

void CNavView::OnPlanPlanoptions() {
	planOpts();
}

void CNavView::planOpts(CWnd *parent) {
	AutoOpt d(parent);
	d.m_ideal = floor(idealDist/1.852); // convert from km to nm & round
	d.m_vor = planVOR;
	d.m_ndb = planNDB;
	d.m_airports = planAirport;
	d.m_waypoints = planWaypoints;
	d.m_continue = planContinue;

	if (d.DoModal() == IDOK) {
		idealDist = d.m_ideal * 1.852; // convert back to km
		planVOR = d.m_vor;
		planNDB = d.m_ndb;
		planAirport = d.m_airports;
		planWaypoints = d.m_waypoints;
		planContinue = d.m_continue;
	}		
}

void CNavView::OnMapShowPlandataonly() {
	planOnly = !planOnly;	
	if (plancount > 0) {
		abort = TRUE; // abort old redraw if necessary
		Invalidate(TRUE);
	}
}

void CNavView::OnUpdateMapShowPlandataonly(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(planOnly);	
}

void CNavView::OnMapShowPlaninformation() {
	planInfo = !planInfo;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateMapShowPlaninformation(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(planInfo);				  	
}

void CNavView::LocationBar() {
	// update status bar with location information
	char  buffer[120], dirx, diry;
	double cx , cy;
	if (flatWorld) {
		cx = (left+right)/2.0;
		cy = (top+bottom)/2.0;
	} else {
		cx = centerLon;
		cy = centerLat;
	}
	if (cx < 0.) 
		dirx = 'W'; 
	else 
		dirx = 'E';
	if (cy < 0.) 
		diry = 'S'; 
	else 
		diry = 'N';
	double cxx = fabs(cx);
	int cxd = (int) (cxx);
	int cxm = (int) ((cxx-cxd) * 60);
	int cxs = (int) (((cxx-cxd)*60-cxm)*60);
	double cyy = fabs(cy);
	int cyd = (int) (cyy);
	int cym = (int) ((cyy-cyd) * 60);
	int cys = (int) (((cyy-cyd)*60-cym)*60);
	int chars;
	if (flatWorld)
		chars = sprintf(buffer, "center:% 8.6f, % 8.6f (%d %d' %d\"%c, %d %d' %d\"%c),  size: %d by %d nm", cy, cx, cyd, cym, cys, diry, cxd, cxm, cxs, dirx,
			(int)((right-left)*degToMeters/1852), (int)((top-bottom)*degToMeters/1852));
	else
		chars = sprintf(buffer, "center:% 8.6f, % 8.6f (%d %d' %d\"%c, %d %d' %d\"%c)", cy, cx, cyd, cym, cys, diry, cxd, cxm, cxs, dirx);
	((CMainFrame *) AfxGetMainWnd())->SetStatusBar(buffer);
}

// define flight data structure	for FSasmLib version 6.0.0
#pragma	pack(1)	// needed to correct alignment problems
typedef struct tagFLTREC_DATA {
  // FsasmLib.dll version
  WORD   wVersion1;   // 6
  WORD   wVersion2;   // 0
  WORD   wVersion3;   // 0
  WORD   wVersion4;   // build id
  // time of day
  WORD   wClockHour;  // hour
  WORD   wClockMin;   // minute
  WORD   wClockSec;   // second
  // position
  DWORD  dwLatLo;     // latitude  (low dword)
  DWORD  dwLatHi;     // latitude  (high dword)
  DWORD  dwLonLo;     // longitude (low dword)
  DWORD  dwLonHi;     // longitude (high dword)
  DWORD  dwAltLo;     // altitude  (low dword)
  DWORD  dwAltHi;     // altitude  (high dword)
  // orientation
  // angles: 180 deg == 0x8000
  DWORD  dwPitch;      // pitch
  DWORD  dwBank;       // bank
  DWORD  dwHeading;    // heading
  // airspeed
  // units: 256 * meters/second
  DWORD  dwTAS;        // true
  DWORD  dwIAS;        // indicated
  // fuel tank quantities 
  // percentages: 100% == 0x800000
  DWORD  dwFuelLeftPct;     // left
  DWORD  dwFuelLeftAuxPct;  // left aux
  DWORD  dwFuelRightPct;    // right
  DWORD  dwFuelRightAuxPct; // right aux
  // gear position
  // up == 0, down == 0xFFFF
  WORD   wGear1;
  WORD   wGear2;
  WORD   wGear3;
} FLTREC_DATA;

void CNavView::OnFileImport() {
	static char BASED_CODE szFilter[] = "FltRec Files (*.dat)|*.dat|All Files (*.*)|*.*||";
	CFileDialog d(TRUE, ".dat", NULL, OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, szFilter);	
	if (d.DoModal() == IDOK) {
		CString filename = d.GetPathName();

		// based on code from Adam Szofran <zof@halcyon.com>, author of fltrec
		FILE* in;
		FLTREC_DATA fltdat;
		long  time = 0;

		if ( (in = fopen(filename, "rb")) == NULL) {
			char buf[50];
			sprintf(buf, "Cannot open %s\n", filename);
			AfxMessageBox(buf);
			return;
		}

		if (flightPaths > 0) {
			fltPath.RemoveAll();
			fltPath.FreeExtra();
			flightPaths--;
		}
		fltPath.SetSize(0,100);
		struct path_t path;
		int size = sizeof(FLTREC_DATA); // may not work due to alignment problems?
		while ( fread( &fltdat, size, 1, in) == 1) { // read data from the file & convert
			double lat, lon;
			int intPart = fltdat.dwLatHi;
			if (intPart >= 0)
				lat = intPart + (double)fltdat.dwLatLo / (double)0x100000000;
			else		 
				lat = intPart - (double)fltdat.dwLatLo / (double)0x100000000;
			lat *= 45.0 / 5000875.0;

			intPart = fltdat.dwLonHi;
			if (intPart >= 0)
				lon = intPart + (double)fltdat.dwLonLo / (double)0x100000000;
			else
				lon = intPart - (double)fltdat.dwLonLo / (double)0x100000000;
			lon *= 90.0 / (256.0 * 4194304.0);
			
			path.lat = lat;
			path.lon = lon;
			path.time = time;
			fltPath.Add(path);
			time++;
		}
		fclose(in);
		flightPaths++;
		abort = TRUE; // abort old redraw if necessary
		Invalidate(TRUE);
	}
}
#pragma	pack()

void CNavView::OnUpdateFileImport(CCmdUI* pCmdUI) {
	pCmdUI->Enable(flightPaths == 0);
}

void CNavView::OnUpdateMapClearflighttrack(CCmdUI* pCmdUI) {
	pCmdUI->Enable(flightPaths > 0);
}

void CNavView::OnMapClearflighttrack() {
	if (flightPaths > 0) {
		fltPath.RemoveAll();
		fltPath.FreeExtra();
		flightPaths--;
		abort = TRUE; // abort old redraw if necessary
		Invalidate(TRUE);
	}
}

void CNavView::OnMapSetflighttrackdensity() {
	CFTDensity d;

	d.m_density = flightTrackDensity;
	if (d.DoModal() == IDOK) { 
		flightTrackDensity = d.m_density;
		char buffer[30];
		sprintf(buffer, "%d", flightTrackDensity);
		WritePrivateProfileString("View", "flightTrackDensity", buffer, "nav.ini");
		if (flightPaths > 0) {
			abort = TRUE; // abort old redraw if necessary
			Invalidate(TRUE);
		}
	}
}

int CNavView::distToPixels(double dist) { // convert a distance (in meters) to pixels
	const double c  = 10001750.0*4;  // circumference of earth (meters)
	const double c2 = 10001750.0*2;  // half circumference of earth (meters)
	if (flatWorld)
		if (screenWidth < screenHeight*2)
			return((int) (dist * screenWidth * (mag2) / c));
		else
			return((int) (dist * screenHeight * (mag2) / c2));
	else {
		double d = 2*earthRadius * asin(1.0/mag2);
		return((int) (dist * min(screenWidth, screenHeight) / d));
	}
}

void CNavView::OnMapLargesymbols() {
	largeSymbols = !largeSymbols;
	char buffer[30];
	sprintf(buffer, "%d", largeSymbols);
	WritePrivateProfileString("View", "largeSymbols", buffer, "nav.ini");
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateMapLargesymbols(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(largeSymbols);	
}

void CNavView::OnMapSettransition() {
	Transition d;
	d.m_trans = symbolTransition;
	if (d.DoModal() == IDOK) {
		symbolTransition = d.m_trans;
		abort = TRUE; // abort old redraw if necessary
		Invalidate(TRUE);
	}
}

void CNavView::OnMapZoomtofit() {
	ZoomToFit();
}

void CNavView::OnUpdateMapZoomtofit(CCmdUI* pCmdUI) {
	CNavDoc* doc = GetDocument();
	pCmdUI->Enable(doc->dbCount > 0);
}

void CNavView::ZoomToFit() {
	CNavDoc* doc = GetDocument();
	double minLat, maxLat, maxLon, minLon;
	mag = 1;
	mag2 = 1;
	minLat =  90.;
	bottom = -90.;
	maxLat = -90.;
	top = 	  90.;
	minLon =  180.;
	left =   -180.;
	maxLon = -180.;
	right =   180.;

	CRect r;		
	GetClientRect(r);
	Size(r.right, r.bottom);

	if (doc->dbCount == 0) { // just in case
		Mready = FALSE;
		Invalidate(TRUE);
		return;
	}
	for (int i=0; i<doc->dbCount; i++) {
		if ((doc->db[i].lat) < minLat)
			minLat = doc->db[i].lat;
		if ((doc->db[i].lat) > maxLat)
			maxLat = doc->db[i].lat;
		if ((doc->db[i].lon) < minLon)
			minLon = doc->db[i].lon;
		if ((doc->db[i].lon) > maxLon)
			maxLon = doc->db[i].lon;
	}

	// set new center
	double rx, ry, xd, yd;
	rx = (minLon+maxLon)/2;
	ry = (minLat+maxLat)/2;
	left   += rx;
	right  += rx;
	top    += ry;
	bottom += ry;

	// set new magnification
	if ((maxLon-minLon<.0001)&&(maxLat-minLat<.0001)) { // too close to zero
		mag = 1;
		mag2 = 1;
	} else if (doc->dbCount == 1) {
		mag = 10;
		mag2 = 512;
	} else {
		int repeat = TRUE;
		while (repeat) {
			// zoom in
			xd = (right - left) / 4.;
			yd = (top - bottom) / 4.;
			right -= xd;
			left += xd;
			top -= yd;
			bottom += yd;
			mag += 1;
			mag2 *= 2;
			convX *= 2;
			convY *= 2;

			if (mag > 15)
				repeat = FALSE;
			// check to see if we went too far
			if ((left > minLon)||(right < maxLon)||(bottom > minLat)||(top < maxLat))
				repeat = FALSE;
		}
		// undo one step
		xd = (right - left) / 2.;
		yd = (top - bottom) / 2.;
		right += xd;
		left -= xd;
		top += yd;
		bottom -= yd;
		mag -= 1;
		mag2 /= 2;
	}

	centerLat = ry;
	centerLon = rx;
	convX = convY = mag2;
	if (screenWidth > screenHeight) 
		convX *= screenHeight / (double) screenWidth;
	else 
		convY *= screenWidth / (double) screenHeight;

	if (mag == 1) { // always center map at mag=1
		bottom = -90.;
		top = 	  90.;
		left =   -180.;
		right =   180.;

		Size(r.right, r.bottom);
	}

	Mready = FALSE;
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnMapShowMapsymbolsAirportzones() {
	showAirportZones = !showAirportZones;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateMapShowMapsymbolsAirportzones(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(showAirportZones);	
}

void CNavView::OnMapShowMaps() {
	showMaps = !showMaps;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateMapShowMaps(CCmdUI* pCmdUI) {
	CNavDoc* pDoc = GetDocument();
	pCmdUI->Enable(pDoc->mapsExist);
	pCmdUI->SetCheck(showMaps && pDoc->mapsExist);	
}

void CNavView::OnMapShowFancymaps() {
	showFancyMaps = !showFancyMaps;	
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

void CNavView::OnUpdateMapShowFancymaps(CCmdUI* pCmdUI) {
	pCmdUI->Enable(fm->mapsDetected);
	pCmdUI->SetCheck(showFancyMaps && fm->mapsDetected);	
}

void CNavView::OnSetFancymapOptions() {
	fm->SetOptions(this);	
}

void CNavView::OnUpdateSetFancymapOptions(CCmdUI* pCmdUI) {
	pCmdUI->Enable(fm->mapsDetected);
}

void CNavView::CenterAt(double lat, double lon) {
	centerLat = lat;
	centerLon = lon;

	double xd = (right + left)/2.0 - lon;
	double yd = (top + bottom)/2.0 - lat;
	left -= xd;
	right -= xd;
	top -= yd;
	bottom -= yd;

	LocationBar();
	Mready = FALSE;
	abort = TRUE; // abort old redraw if necessary
	Invalidate(TRUE);
}

int CNavView::drawMessageLoop() {	// returns TRUE if abort was detected
	MSG msg;  
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
		if (msg.message == WM_QUIT) { 
			// begin shutdown sequence
			tryToLink = FALSE;
//			linked = FALSE;
//			flying = FALSE;
//			SetEvent(hShutdown);
			PostQuitMessage(1);
			return(TRUE);
		} else if (/*(msg.message == WM_PAINT) &&*/ (abort == TRUE)) { 
			// abort this and start over
			abort = FALSE;
			return(TRUE);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);	
	}
	return(FALSE);
}

void CNavView::OnMapCenterat() {
	CCenterAt d;
	
	d.m_lon = (right + left)/2.0;
	d.m_lat = (top + bottom)/2.0;

	if (d.DoModal() == IDOK)
		CenterAt(d.m_lat, d.m_lon);
}

double CNavView::sign(double x) {
	if (x<0.0)
		return -1.0;
	else
		return 1.0;
}

void CNavView::DrawMaps(CDC* pDC) {
	CNavDoc* pDoc = GetDocument();
	if (pDoc->mapsExist) {
		CPen grayPen(PS_SOLID, 0, RGB(192, 192, 192));
		CPen *oldPen = pDC->SelectObject(&grayPen);
		if (flatWorld) 	{
			int lat, lon, leftd, rightd, topd, bottomd, lastlon=0, rx, ry;
			double latf, lonf;
			double convertLat = 90.0/0x80000000; 
			double convertLon = 180.0/0x80000000;
			if (left < -180.0)
				leftd = 0x80000000;
			else
				leftd = (int) (left / convertLon);
			if (right >= 180.0)
				rightd = 0x7fffffff;
			else
				rightd = (int) (right / convertLon);
			if (top >= 90.0)
				topd = 0x7fffffff;
			else
				topd = (int) (top / convertLat);
			if (bottom < -90.0)
				bottomd = 0x80000000;
			else
				bottomd = (int) (bottom / convertLat);
			int loop = pDoc->mapTable.GetSize();
			int lastEdgeFlag = FALSE;
			int edgeFlag = FALSE;
			int moveFlag = TRUE;
			int wrapFlag = FALSE;
			int lastx, lasty;
			int decimate = 1;
			if (mag == 1)
				decimate = 4;
			else if (mag == 2)
				decimate = 3;
			else if (mag == 3)
				decimate = 2;
			int deccounter = decimate;
			oldPen = pDC->SelectObject(&grayPen);
			for (int m=0; m<loop; m+=2) {
				lat = pDoc->mapTable[m];
				lon = pDoc->mapTable[m+1];
				if (lat == 0) {
					if (lon == 0) {
						moveFlag = TRUE;
						deccounter = decimate;
						continue;
					}
				} else if ((decimate > 1) && (deccounter != decimate)) {
					if(deccounter == 0) {
						deccounter = decimate;
						continue;
					}
					deccounter--;
					continue;
				} else if (lon < leftd) 
					edgeFlag = TRUE;
				else if (lon > rightd) 
					edgeFlag = TRUE;
				else if (lat < bottomd) 
					edgeFlag = TRUE;
				else if (lat > topd) 
					edgeFlag = TRUE;
				if (mag == 1) { // check for date-line wrap around
					if (!moveFlag) {
						if ((lon > 0x70000000) && (lastlon < -0x70000000))
							wrapFlag = TRUE;
						else if ((lon < -0x70000000) && (lastlon > 0x70000000))
							wrapFlag = TRUE;
					}
				}
				latf = lat * convertLat;
				lonf = lon * convertLon;
				LtoC(lonf, latf, rx, ry);
				if (moveFlag)
					pDC->MoveTo(rx,ry);
				else if (edgeFlag && !lastEdgeFlag)	{
					pDC->LineTo(rx,ry);
					lastx = rx;
					lasty = ry;
				} else if (edgeFlag && lastEdgeFlag) {
					lastx = rx;
					lasty = ry;
				} else if (!edgeFlag && lastEdgeFlag) {
					pDC->MoveTo(lastx,lasty);
					pDC->LineTo(rx,ry);
				} else if (wrapFlag) {
					pDC->MoveTo(rx,ry);
					wrapFlag = FALSE;	
				} else 
					pDC->LineTo(rx,ry);
				lastlon = lon;
				lastEdgeFlag = edgeFlag;
				deccounter--;
				edgeFlag = FALSE;
				moveFlag = FALSE;
			}								
		} else {
			// max distance on display
			double efconv = 1/sqrt((1/convX)*(1/convX)+(1/convY)*(1/convY));
			if (2/efconv >= 1.0)
				dmax = min(1.570796326794896560*earthRadius, 10001750) ;
			else
				dmax = min(asin(2/efconv)*earthRadius, 10001750) ;
			int lat, lon, rx, ry, lastx, lasty;
			double latf, lonf;
			double convertLat = 90.0/0x80000000; 
			double convertLon = 180.0/0x80000000;
			int loop = pDoc->mapTable.GetSize();
			int moveFlag = TRUE, lastEdgeFlag = FALSE, edgeFlag = FALSE;
			int decimate = 1;
			if (mag == 1)
				decimate = 4;
			else if (mag == 2)
				decimate = 3;
			else if (mag == 3)
				decimate = 2;
			int deccounter = decimate;

			for (int m=0; m<loop; m+=2) {
				lat = pDoc->mapTable[m];
				lon = pDoc->mapTable[m+1];
				if (lat == 0) {
					if (lon == 0) {
						moveFlag = TRUE;
						lastEdgeFlag = edgeFlag = FALSE;
						deccounter = decimate;
						continue;
					}
				}
				
				if ((decimate > 1) && (deccounter != decimate)) {
					if(deccounter == 0) 
						deccounter = decimate;
					else
						deccounter--;
					continue;
				} 

				latf = lat * convertLat;
				lonf = lon * convertLon;
				edgeFlag = !LtoC(lonf, latf, rx, ry);

				if (moveFlag && !edgeFlag) {			// previous entry was Move
					pDC->MoveTo(rx,ry);
				} else if (moveFlag && edgeFlag) {		// previous entry was Move, but current point is off edge
					deccounter--;
					lastx = rx;
					lasty = ry;
					lastEdgeFlag = TRUE;
					moveFlag = FALSE;
					continue;
				} else if (edgeFlag && !lastEdgeFlag) { // going over the edge
					pDC->LineTo(rx,ry);
				} else if (!edgeFlag && lastEdgeFlag) { // coming back from edge
					pDC->MoveTo(lastx,lasty);
					pDC->LineTo(rx,ry);
	//			} else if (edgeFlag && lastEdgeFlag) {	// still over the edge (no need to do anything)
	//				;
				} else if (!edgeFlag && !lastEdgeFlag)	// not over the edge
					pDC->LineTo(rx,ry);

				lastx = rx;
				lasty = ry;
				lastEdgeFlag = edgeFlag;
				moveFlag = FALSE;
				deccounter--;
			}								
		}
		pDC->SelectObject(oldPen);
	}
}

void CNavView::Mcalc() { // update viewpoint rotation matrix
	M11 =  cos(-centerLon * degToRad);
	M12 =  0;
	M13 =  sin(-centerLon * degToRad);
	M22 =  cos(-centerLat * degToRad);
	M32 = -sin(-centerLat * degToRad);
	M21 =  M32*M13;
	M23 = -M11*M32;
	M31 = -M22*M13;
	M33 =  M11*M22;
	Mready = TRUE;
}

void CNavView::DrawGrid(CDC *pDC) {
	char buffer[50];
	int rx, ry;
	CPen *oldPen;
	CPen grayPen(PS_SOLID, 0, RGB(192, 192, 192));
	if (pDC->IsPrinting() && forceBlackPrinting) { // light gray text does not print well
		oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);
		pDC->SetTextColor(RGB(0, 0, 0));
	} else { 
		oldPen = pDC->SelectObject(&grayPen);
		pDC->SetTextColor(RGB(192, 192, 192));
	}
	if (flatWorld) {
		double gridsize;
		if (mag <3)
			gridsize = 30.0;
		else if (mag <5)
			gridsize = 10.0;
		else if (mag == 5)
			gridsize = 5.0;
		else if (mag == 6)
			gridsize = 2.0;
		else if (mag == 7)
			gridsize = 1.0;
		else if (mag > 7)
			gridsize = 1.0 / (2 << (mag-8));
		double start, end;
		int intgrid;
		intgrid = (int) (bottom / gridsize);
		if (bottom < 0.0)
			start = gridsize * (intgrid -1);
		else 
			start = gridsize * (intgrid);
		intgrid = (int) (top / gridsize);
		if (top < 0.0)
			end = gridsize * (intgrid);
		else 
			end = gridsize * (intgrid+1);
		if (start < -90) start = -90.0;
		if (start > 90) start = 90.0;
		if (end < -90) end = -90.0;
		if (end > 90) end = 90.0;
		for (double i= start+gridsize; i<end; i+=gridsize) { // latitude
			LtoC(-180, i, rx, ry);
			if ((ry>0) && (ry<screenHeight)) {
				if(gridsize < 1) {
					int deg = (int) (i);
					int min = (int) ((i-deg) * 60);
					int sec = (int) (((i-deg)*60-min)*60);
					if (i>=0)
						sprintf(buffer, "%d %d' %d\"", abs(deg), abs(min), abs(sec));
					else
						sprintf(buffer, "-%d %d' %d\"", abs(deg), abs(min), abs(sec));
				} else   
					sprintf(buffer, "%.lf", i);
				pDC->TextOut(max(rx+1,1), max(ry+1,1), buffer);
				pDC->MoveTo(max(rx,0), ry);
				LtoC(180, i, rx, ry);
				pDC->LineTo(min(rx, screenWidth), ry);
			}
		}
		intgrid = (int) (left / gridsize);
		if (left < 0.0)
			start = gridsize * (intgrid -1);
		else 
			start = gridsize * (intgrid);
		intgrid = (int) (right / gridsize);
		if (right < 0.0)
			end = gridsize * (intgrid);
		else 
			end = gridsize * (intgrid+1);
		if (start < -180) start = -180.0;
		if (start > 180) start = 180.0;
		if (end < -180) end = -180.0;
		if (end > 180) end = 180.0;
		for (i= start+gridsize; i<end; i+=gridsize) {   // longitude
			LtoC(i,90, rx, ry);
			if ((rx>0) && (rx<screenWidth)) {
				if(gridsize < 1) {
					int deg = (int) (i);
					int min = (int) ((i-deg) * 60);
					int sec = (int) (((i-deg)*60-min)*60);
					if (i>=0)
						sprintf(buffer, "%d %d' %d\"", abs(deg), abs(min), abs(sec));
					else
						sprintf(buffer, "-%d %d' %d\"", abs(deg), abs(min), abs(sec));
				} else   
					sprintf(buffer, "%.lf", i);
				pDC->TextOut(rx+1, max(ry+1,1), buffer);
				pDC->MoveTo(rx, max(ry,0));
				LtoC(i, -90, rx, ry);
				pDC->LineTo(rx, min(ry,screenHeight));
			}
		}
	} else { // !flatWorld
		;/* don't know a good algorithm for this yet */
	}
	pDC->SetTextColor(RGB(0, 0, 0));
	pDC->SelectObject(oldPen);
}

// check for number of IDs this one would overlap
int overlaps(idPlace idP,  CArray<idPlace, idPlace> *idlist) {
	int overlaps = 0;
	idPlace test;
	for (int i = 0; i < idlist->GetSize(); i++) {
		test = idlist->GetAt(i);
		if (idP.sx < test.x) continue;
		if (idP.x > test.sx) continue;
		if (idP.sy < test.y) continue;
		if (idP.y > test.sy) continue;
		overlaps++;
	}
	return overlaps;
}

// find "good" placement for IDs
void CNavView::idPlacement(int centerx, int centery, idPlace &idP, CArray<idPlace, idPlace> *idlist, int ils) {
	int overlap;
	overlap = overlaps(idP, idlist);
	if (overlap == 0) { // no problems
		idlist->Add(idP);
		return;
	}
	// try to find a better place
	idPlace a, best = idP;
	double d = sqrt((double)(idP.x-centerx)*(idP.x-centerx)+(idP.y-centery)*(idP.y-centery));
	double ang = atan2((double)(idP.y-centery), (double)(idP.x-centerx));
	d = max(d, idP.sy-idP.y);
	int dx, dy, o;
	double mul=1.0;
	while (mul<5) {
		if (ils == 0) {
			double pref = -pi/2 + pi/12; // preferred angle
			double tang = 0;
			while (tang < 2*pi) {
				dx = (int) (mul*d*cos(tang+pref));
				dy = (int) (mul*d*sin(tang+pref));
				a = idP;
				a.x =  centerx + dx;
				a.sx = a.x + (idP.sx-idP.x);
				a.y =  centery + dy;
				a.sy = a.y + (idP.sy-idP.y);
				o = overlaps(a, idlist);
				if (overlap > o) {
					best = a;
					overlap = o;
				}
				if (overlap == 0)
					break;
				tang += pi/24;
			}
		} else { // special ils placement
			double tang = pi/12;
			while (tang <= pi/2) {
				dx = (int) (d*cos(tang+ang));
				dy = (int) (d*sin(tang+ang));
				a = idP;
				a.x =  centerx + dx;
				a.sx = a.x + (idP.sx-idP.x);
				a.y =  centery + dy;
				a.sy = a.y + (idP.sy-idP.y);
				o = overlaps(a, idlist);
				if (overlap > o) {
					best = a;
					overlap = o;
				}
				if (overlap == 0)
					break;
				tang = -tang;
				dx = (int) (d*cos(tang+ang));
				dy = (int) (d*sin(tang+ang));
				a = idP;
				a.x =  centerx + dx;
				a.sx = centerx + dx + (idP.sx-idP.x);
				a.y =  centery + dy;
				a.sy = centery + dy + (idP.sy-idP.y);
				o = overlaps(a, idlist);
				if (overlap > o) {
					best = a;
					overlap = o;
				}
				if (overlap == 0)
					break;
				tang = -tang;
				tang += pi/24;
			}
		}
		if (overlap == 0)
			break;
		mul *= 1.15;
	}
	idP = best;
	idlist->Add(idP);
}

void CNavView::OnInitialUpdate() {
	CView::OnInitialUpdate();
	DragAcceptFiles(TRUE); // register window for drag and drop support
}

void CNavView::OnDropFiles(HDROP hDropInfo) {
	char filename[_MAX_PATH];
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT) -1, NULL, 0);
	if (nFiles > 0) {
		CNavDoc* pDoc = GetDocument();
		FILE *fp;
		CString listname = orgDir + "\\" + "Dropped.lst";
		fp = fopen((LPCTSTR) listname, "w");
		for (UINT iFile = 0; iFile<nFiles; iFile++) {
			::DragQueryFile(hDropInfo, iFile, filename, _MAX_PATH);
			fprintf(fp, "%s\n", filename);	
		}
		fclose(fp);
		pDoc->OnFileLoad("Dropped.lst");
		DeleteFile((LPCTSTR) listname);
	}
	::DragFinish(hDropInfo);
}

void CNavView::OnFs6Link() {
	if (link) {
		tryToLink = link = FALSE;
//		linked = FALSE;
	} else {
		hthread = AfxBeginThread(LinkThread, this)->m_hThread; // start link thread
		tryToLink = link = TRUE;
	}
	char buffer[30];
	sprintf(buffer, "%d", link);
	WritePrivateProfileString("View", "Link", buffer, "nav.ini");
}

void CNavView::OnUpdateFs6Link(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(link);	
}

void CNavView::OnFs6Set() {
	FSInfo* pFSInfo; // set this pointer to either FS95, FS98 or FS2000 state offsets 
	HWND hWndFs6 = NULL;
	if (hWndFs6 = FindWindowEx(NULL, NULL, fs95Info.pszWindowClass, NULL))
		pFSInfo = &fs95Info;
	else if (hWndFs6 = FindWindowEx(NULL, NULL, "FS2KMAIN", NULL))
		pFSInfo = &fs2000Info;
	else if (hWndFs6 = FindWindowEx(NULL, NULL, fs98Info.pszWindowClass, NULL))
		pFSInfo = CheckFSTitle(hWndFs6, "2000") ? &fs2000Info : &fs98Info;
	if (hWndFs6 == NULL) {		
		AfxMessageBox("Can't find FlightSim. Is it running?");
		return;
	}

	double lat, lon, fract;
	if (flatWorld) {
		lat = (top + bottom)/2.0;
		lon = (right + left)/2.0;
	} else {
		lat = centerLat;
		lon = centerLon;
	}
	lat /= 45.0 / 5000875.0;
	lon /= 90.0 / (256.0 * 4194304.0);
	fract = (int)lat - lat;
	flightData.dwLatLo = (int) (fract * 0x100000000);
	if (lat >= 0)
		flightData.dwLatHi = (int) (lat - fract);
	else 
		flightData.dwLatHi = (int) (lat + fract);
	fract = (int)lon - lon;
	flightData.dwLonLo = (int) (fract * 0x100000000);
	if (lat >= 0)
		flightData.dwLonHi = (int) (lon - fract);
	else 
		flightData.dwLonHi = (int) (lon + fract);

	EnterCriticalSection(&crit);
	FS6IPC_Client ipcClient;	
	ipcClient.Start(hWndFs6);
	ipcClient.BeginRequest();
	ipcClient.WriteStateData(pFSInfo->offset_dwLatLo, 4 * sizeof(DWORD), &flightData.dwLatLo);
	ipcClient.SendRequest();
	ipcClient.Stop();
	LeaveCriticalSection(&crit);
	Invalidate(TRUE); // redraw window to clean up graphics glitches from link data
}

void CNavView::OnFs6Fs6linkoptions() {
	LinkOptions d;

	d.m_update = linkUpdateTime;
	d.m_flyupdate = linkFlyUpdateTime;
	d.m_flysetmode = linkFlySetMode;
	d.m_enablenetwork = enableNetwork;
	d.m_callsign = callsign;
	if (d.DoModal() == IDOK) {
		linkUpdateTime = d.m_update;
		linkFlyUpdateTime = d.m_flyupdate;
		linkFlySetMode = d.m_flysetmode;
		enableNetwork = d.m_enablenetwork;
		strncpy(callsign, d.m_callsign.GetBuffer(1), 30);
		char buffer[30];
		sprintf(buffer, "%lf", linkUpdateTime);
		WritePrivateProfileString("View", "linkUpdateTime", buffer, "nav.ini");
		sprintf(buffer, "%lf", linkFlyUpdateTime);
		WritePrivateProfileString("View", "linkFlyUpdateTime", buffer, "nav.ini");
		sprintf(buffer, "%d", linkFlySetMode);
		WritePrivateProfileString("View", "linkFlySetMode", buffer, "nav.ini");
		sprintf(buffer, "%d", enableNetwork);
		WritePrivateProfileString("View", "enableNetwork", buffer, "nav.ini");
		WritePrivateProfileString("View", "callsign", callsign, "nav.ini");
		SetEvent(hflyevent);
		if (enableNetwork)
			initNetwork();
		else
			shutdownNetwork();
	}	
}

void CNavView::stopFlyTo() {
	if (flying)
		OnFs6Flyto();
}

void CNavView::OnFs6Flyto() {
	flyToWaypoint = -1;
	flyingAlt = -1;
	if (flying) {
		flying = FALSE;
		SetEvent(hflyevent);
		Invalidate(TRUE);
		return;
	}
	flying = TRUE;
	if (!flatWorld) {
		flyingLat = centerLat;
		flyingLon = centerLon;
	} else {
		flyingLon = (right + left)/2.0;
		flyingLat = (top + bottom)/2.0;
	}
	// start fly thread
	if (hflythread == 0)
		hflythread = AfxBeginThread(FlyThread, this)->m_hThread;
	Invalidate(TRUE);
}

void CNavView::OnUpdateFs6Flyto(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(flying && (flyToWaypoint == -1));	
}

void CNavView::OnHelpviewdocs() {
	STARTUPINFO si;
	GetStartupInfo(&si);
	PROCESS_INFORMATION pinf;
	CString command = "start ";
	command = command + orgDir + "\\Nav.rtf";
	CreateProcess(NULL, command.GetBuffer(1), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pinf);
	command.ReleaseBuffer();
}

void CNavView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	CRect clientRect;								
	GetClientRect(clientRect);
	CPoint point(clientRect.right/2, clientRect.bottom/2);
	int scrollPixels = 20;
	if ((nChar == 189) || (nChar == VK_SUBTRACT)) {		// zoom out
		PostMessage(WM_COMMAND, ID_VIEW_ZOOMOUT, 0);
	} else if ((nChar == 187) || (nChar == VK_ADD)) {	// zoom in
		PostMessage(WM_COMMAND, ID_VIEW_ZOOMIN, 0);		
	} else if ((nChar == 'n') || (nChar == 'N')) {		// find nearest (same as double right click)
		OnRButtonDblClk(0, point);
	} else 	if (nChar == VK_RIGHT) {					// scroll right
		point.x += scrollPixels;
		OnLButtonDown(0, point);
	} else if (nChar == VK_LEFT) {						// scroll left		
		point.x -= scrollPixels;
		OnLButtonDown(0, point);
	} else if (nChar == VK_UP) {						// scroll up		
		point.y -= scrollPixels;
		OnLButtonDown(0, point);
	} else if (nChar == VK_DOWN) {						// scroll down		
		point.y += scrollPixels;
		OnLButtonDown(0, point);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CNavView::OnSetFocus(CWnd* pOldWnd) {
	CView::OnSetFocus(pOldWnd);
	
	if (linked)
		Invalidate(TRUE);	// re-draw whole map to avoid double images of plane
}

// update the FS6-Fly To Plan Waypoint menu item
void CNavView::OnUpdatePlan0(CCmdUI* pCmdUI)  {
	pCmdUI->Enable(flying && (flyToWaypoint>0));			
	// Locate the FS6 submenu
	CMenu* pFileMenu = NULL;
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	int iPos;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_FS6_FS6LINKOPTIONS) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);
	pTopMenu = pFileMenu;
	pFileMenu = NULL;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_PLAN0) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);

	// Update the submenu 
	// First, delete all items but ID_PLAN0 at position 0
	for (iPos = pFileMenu->GetMenuItemCount()-1; iPos > 0; iPos--)
		pFileMenu->DeleteMenu(iPos, MF_BYPOSITION);

	if (!planning)
		return;
	// Then, add an item for each available waypoint
	pFileMenu->AppendMenu(MF_SEPARATOR);
	CString name;
	for (int i=1; i <= plancount; i++)	{
		name = planD->PlanTableName(i-1);
		pFileMenu->AppendMenu(MF_STRING, WM_NAV+1000+i, name.GetBuffer(1));
		name.ReleaseBuffer();
	}
}

void CNavView::OnPlan0() {
	FlyToWaypoint(0);	
}

BOOL CNavView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	// If pHandlerInfo is NULL, then handle the message
	if (pHandlerInfo == NULL) {
		// Filter the commands sent to a list file menu option
		for (UINT i = WM_NAV+1000+1; i <= (unsigned)plancount+WM_NAV+1000+1; i++) {
			if (nID == i) {
				if (nCode == CN_COMMAND)    // Handle WM_COMMAND message
					FlyToWaypoint(i-(WM_NAV+1000));
				else if (nCode == CN_UPDATE_COMMAND_UI) {
					((CCmdUI*)pExtra)->Enable(linked);
					((CCmdUI*)pExtra)->SetCheck(i-(WM_NAV+1000) == (unsigned)flyToWaypoint);
				}
				return TRUE;
			}
		}

		drawDataHolder all;
		GetAllDrawData(all);
		for (i = WM_NAV+1100; i < (unsigned)all.GetSize()+WM_NAV+1100; i++) {
			if (nID == i) {
				int j = i-(WM_NAV+1100);
				if	((j < 0) || (j >= all.GetSize()))
					continue;// shouldn't happen
				if (nCode == CN_COMMAND)	// Handle WM_COMMAND message
					centerOnAircraft = *((int*) &all.data[j]->source);
				else if (nCode == CN_UPDATE_COMMAND_UI) {
					((CCmdUI*)pExtra)->Enable(TRUE);
					((CCmdUI*)pExtra)->SetCheck(centerOnAircraft == *((int*) &all.data[j]->source));
				}
				return TRUE;
			}
		}
	}
	// If we didn't process the command, call the base class
	// version of OnCmdMsg so the message-map can handle the message	
	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CNavView::FlyToWaypoint(int waypointNumber) {
	flyToWaypoint = waypointNumber;
	if (waypointNumber == 0) {
		flyingAlt = -1;
		flying = FALSE;
		SetEvent(hflyevent);
		Invalidate(TRUE);
		return;
	}
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CNavView *view = (CNavView *) main->GetActiveView();
	CNavDoc *doc = (CNavDoc *) main->GetActiveDocument();
	int dbtype;
	if (planD->plantable[flyToWaypoint-1].index >= 0)
		dbtype = doc->db[planD->plantable[flyToWaypoint-1].index].type;
	else 
		dbtype = planD->plantable[flyToWaypoint-1].index; // toc, bod, toc/bob

	if (dbtype == 14) { // fly to closest end of route segment
		double la1 = doc->db[planD->plantable[flyToWaypoint-1].index].lat;
		double lo1 = doc->db[planD->plantable[flyToWaypoint-1].index].lon;
		double la2 = doc->db[planD->plantable[flyToWaypoint-1].index].approach;
		double lo2 = doc->db[planD->plantable[flyToWaypoint-1].index].range;
		if (gcDist(lastLatRead, lastLonRead, la1, lo1) < gcDist(lastLatRead, lastLonRead, la2, lo2)) {
			flyingLat = la1;
			flyingLon = lo1;
		} else {
			flyingLat = la2;
			flyingLon = lo2;
		}
	} else {
		flyingLat = planD->plantable[flyToWaypoint-1].lat;
		flyingLon =	planD->plantable[flyToWaypoint-1].lon;
	}
	flyingAlt = planD->plantable[flyToWaypoint-1].alt;

	// check for airport at beginning and end of route
	if ((flyToWaypoint == 1) || (flyToWaypoint == plancount)) 
		if ((dbtype == 8) || (dbtype == 4)) 
			flyingAlt += 2000;

	Invalidate(TRUE);

	// start fly thread
	if (flying)
		return;
	flying = TRUE;
	if (hflythread == 0)
		hflythread = AfxBeginThread(FlyThread, this)->m_hThread;
}

void CNavView::IncrementFlytoWaypoint() {
	ASSERT(flyToWaypoint>0);
	ASSERT(flyToWaypoint<plancount);
	FlyToWaypoint(flyToWaypoint+1);
}

int CNavView::isLinked() {
	return linked;
}

double CNavView::closestCorner(CPoint target, CRect rect, CPoint &closest) {
	double dist = pointDistance(target, CPoint(rect.left, rect.top));
	closest = CPoint(rect.left, rect.top);
	double d = pointDistance(target, CPoint(rect.right, rect.top));
	if (d<dist) {
		closest = CPoint(rect.right, rect.top);
		dist = d;
	}
	d = pointDistance(target, CPoint(rect.right, rect.bottom));
	if (d<dist) {
		closest = CPoint(rect.right, rect.bottom);
		dist = d;
	}
	d = pointDistance(target, CPoint(rect.left, rect.bottom));
	if (d<dist) {
		closest = CPoint(rect.left, rect.bottom);
		dist = d;
	}
	return dist;
}

double CNavView::pointDistance(CPoint r, CPoint s) {
	int d = (r.x-s.x)*(r.x-s.x) + (r.y-s.y)*(r.y-s.y);
	return sqrt((double) d);
}

void CNavView::drawDist(CDC *pDC, int dist, CPoint from, CPoint to) {
	CPoint diff = to - from;
	double d = pointDistance(from, to);
	double dx = diff.x * dist / d;
	double dy = diff.y * dist / d;
	CPoint newTo = from;
	newTo.Offset((int)dx, (int)dy);
	pDC->MoveTo(from);
	pDC->LineTo(newTo);
}

void CNavView::OnFileStartwithlastlist() {	
	startWithLastList = !startWithLastList;
	char buffer[30];
	sprintf(buffer, "%d", startWithLastList);
	WritePrivateProfileString("View", "startWithLastList", buffer, "nav.ini");
	if (lastSceneryList != "")
		WritePrivateProfileString("View", "lastSceneryList", lastSceneryList, "nav.ini");
}

void CNavView::OnUpdateFileStartwithlastlist(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(startWithLastList);
}

/* slightly modified Faster Line Segment Intersection */
/* (from Graphics Gems III, Franklin Antonio) */
/* return TRUE if the lines from points 1-2 and 3-4 intersect */
int lines_intersect(int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4) {
	int Ax,Bx,Cx,Ay,By,Cy,d,e,f;
	int x1lo,x1hi,y1lo,y1hi;

	Ax = x2-x1;
	Bx = x3-x4;

	if(Ax<0) {						/* X bound box test*/
		x1lo=x2; x1hi=x1;
	} else {
		x1hi=x2; x1lo=x1;
	}
	if(Bx>0) {
		if(x1hi < x4 || x3 < x1lo) return FALSE;
	} else {
		if(x1hi < x3 || x4 < x1lo) return FALSE;
	}

	Ay = y2-y1;
	By = y3-y4;

	if(Ay<0) {						/* Y bound box test*/
		y1lo=y2; y1hi=y1;
	} else {
		y1hi=y2; y1lo=y1;
	}
	if(By>0) {
		if(y1hi < y4 || y3 < y1lo) 
			return FALSE;
	} else {
		if(y1hi < y3 || y4 < y1lo) 
			return FALSE;
	}

	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;					/* alpha numerator*/
	f = Ay*Bx - Ax*By;					/* both denominator*/
	if(f>0) {							/* alpha tests*/
		if(d<0 || d>f) 
			return FALSE;
	} else {
		if(d>0 || d<f) 
			return FALSE;
	}

	e = Ax*Cy - Ay*Cx;					/* beta numerator*/
	if(f>0) {							/* beta tests*/
		if(e<0 || e>f) 
			return FALSE;
	} else {
		if(e>0 || e<f) 
			return FALSE;
	}
	return TRUE;  
}

int CNavView::linesIntersect(CPoint p1, CPoint p2, CPoint p3, CPoint p4) {
	return(lines_intersect(p1.x,p1.y, p2.x,p2.y, p3.x,p3.y, p4.x,p4.y));
}

int CNavView::linesIntersect(CPoint p1, CPoint p2, CRect r) {
	if (lines_intersect(p1.x,p1.y, p2.x,p2.y, r.left, r.top, r.right, r.top))
		return(TRUE);
	else if (lines_intersect(p1.x,p1.y, p2.x,p2.y, r.left, r.bottom, r.right, r.bottom))
		return(TRUE);
	else if (lines_intersect(p1.x,p1.y, p2.x,p2.y, r.right, r.top, r.right, r.bottom))
		return(TRUE);
	else return (lines_intersect(p1.x,p1.y, p2.x,p2.y, r.left, r.top, r.left, r.bottom));
}

// new Lat/Lon version of gcLine, works even if end points are not visible
// return TRUE if all was visible 
int CNavView::gcLine(double slon, double slat, double elon, double elat, CDC *pdc, int black, int depth) {
	int ret = FALSE, ret2 = TRUE;
	if ((slon==elon) && (slat==elat))
		return ret;

	CDC *dc;
	if (pdc == NULL)
		dc = GetDC();
	else
		dc = pdc;
	CPen *oldPen;
	int oldROP;
	if (black) {
		oldPen = (CPen *) dc->SelectStockObject(BLACK_PEN);
		oldROP = dc->SetROP2(R2_NOTXORPEN);
	}

	int x1, y1, x2, y2;
	int vis1 = LtoC(slon, slat, x1, y1);
	int vis2 = LtoC(elon, elat, x2, y2);
	double dist2 = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);
	if (dist2 < 6400 * mag2) { // close enough to approximate with straight line
		if (vis1 && vis2) { // both endpoints visible
			dc->MoveTo(x1, y1);
			dc->LineTo(x2, y2);
			ret2 = ret = TRUE;
		} else if (flatWorld) { 
			if (linesIntersect(CPoint(x1,y1), CPoint(x2,y2), clientRect)) {
				dc->MoveTo(x1, y1);
				dc->LineTo(x2, y2);
				ret2 = ret = TRUE;
			} else
				ret2 = ret = FALSE;
		} else if (!flatWorld && (mag2>1) && (vis1 || vis2)) { 
			dc->MoveTo(x1, y1);
			dc->LineTo(x2, y2);
			ret2 = ret = TRUE;
		} 
	} else { // recurse
		if (flatWorld && (fabs(slon-elon) > 359.9))
			ret2 = ret = FALSE; // avoid falling off end of world
		else if (depth > 20) 
			ret2 = ret = FALSE; // too deep, skip it
		else {
			double nlat, nlon;
			midPoint(slat, slon, elat, elon, nlat, nlon);
			depth++;
			ret  = gcLine(slon, slat, nlon, nlat, dc, FALSE, depth);
			ret2 = gcLine(nlon, nlat, elon, elat, dc, FALSE, depth);
		}
	}

	if (black) {
		dc->SelectObject(oldPen);
		dc->SetROP2(oldROP);
	}
	if (pdc == NULL)
		ReleaseDC(dc);
	return ret && ret2;
}

UINT NetReadThread( LPVOID pParam ) {
	CNavView *pView = (CNavView *) pParam;
	char rbuffer[1024];
	int bytesReceived, addrSize;
	struct sockaddr_in from;
	addrSize = sizeof(struct sockaddr_in);
	struct fd_set in;
	struct timeval timeout = {0, 100000}; // wait at most 100ms
	int ready;

	while (pView->networkInitialized) {
		FD_ZERO(&in); 
		FD_SET(sockfd, &in); 
		ready = select(0, &in, NULL, NULL, &timeout); 
		if (ready == 1) { 
			bytesReceived = recvfrom(sockfd, rbuffer, 1024, 0, (sockaddr*) &from, &addrSize);
			if (bytesReceived == sizeof(struct netData)) {
				int fromMe = FALSE; // ignore packets from this computer
				for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
					if (0 == memcmp(phe->h_addr_list[i], &from.sin_addr, sizeof(struct in_addr))) {
						fromMe = TRUE;
						break;
					}
				}
				if (!fromMe) 
					undrawn.add(new drawData((struct netData *) rbuffer, &from));
			}
		}
	}
	return 0;
}

void CNavView::netSend(char *stuff, int size) {
	int bytesSent = sendto(sockfd, stuff, size, MSG_DONTROUTE, 
		(struct sockaddr *)&their_addr, sizeof(struct sockaddr));
}

void CNavView::initNetwork() {
	if(networkInitialized)
		return;
	int err;
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {// no winsock available	    
//		AfxMessageBox("no winsock available");
		return;
	}

	char ac[80]; // who am I?
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
		WSACleanup();
//		AfxMessageBox("gethostname failed");
		return;
	}

//	char buf[256];
//	sprintf(buf, "My host name is %s", ac);
//	AfxMessageBox(buf);

    phe = gethostbyname(ac);
    if (phe == 0) {
		WSACleanup();
		return;
	}
	
    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
//		sprintf(buf, "My address %d: %s", i, inet_ntoa(addr));
//		AfxMessageBox(buf);
    }

	// setup socket for broadcasting
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		WSACleanup();
//		AfxMessageBox("socket create error");
		return;
	}
	their_addr.sin_family = AF_INET;      /* host byte order */
	their_addr.sin_port = htons(47747);   /* short, network byte order */
	unsigned long baddr = inet_addr("255.255.255.255");
	memcpy(&(their_addr.sin_addr), &baddr, 4); 
	memset(&(their_addr.sin_zero), 0, 8); /* zero the rest of the struct */
	i = 1; //turn on the socket's permission to broadcast
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i));
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&i, sizeof(i));

	// setup socket for listening
	struct sockaddr_in my_addr; 
	my_addr.sin_family = AF_INET;           /* host byte order */
	my_addr.sin_port = htons(47747);        /* short, network byte order */
	my_addr.sin_addr.s_addr = INADDR_ANY;   /* auto-fill with my IP */
	memset(&(my_addr.sin_zero), 0, 8);      /* zero the rest of the struct */
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
		WSACleanup();
//		AfxMessageBox("bind error");
		return;
	}

	networkInitialized = TRUE;
	netRead = AfxBeginThread(NetReadThread, this)->m_hThread;
}

void CNavView::shutdownNetwork() {
	if (!networkInitialized)
		return;

	networkInitialized = FALSE;
	WaitForSingleObject(netRead, 10000); 
	CloseHandle(netRead);

	if (sockfd != -1)
		closesocket(sockfd);
	sockfd = -1;
	WSACleanup();
}

// use BitBlt to XOR text to the screen
void CNavView::LinkDrawText(CDC *pDC, int x, int y, char *text) {
	CDC newDC;
	CBitmap bitmap;
	CBitmap* pOldBitmap;
	newDC.CreateCompatibleDC(pDC);
	CFont font;
	LOGFONT lfnew = lf;
	font.CreateFontIndirect(&lfnew);
	CFont* oldFont = (CFont *) newDC.SelectObject(&font);

	int chars = strlen(text);
	CSize tSize = newDC.GetOutputTextExtent(text, chars);
	bitmap.CreateCompatibleBitmap(pDC, tSize.cx, tSize.cy);
	pOldBitmap = newDC.SelectObject(&bitmap);

	newDC.TextOut(0, 0, text);
	newDC.BitBlt(0, 0, tSize.cx, tSize.cy, NULL, 0, 0, DSTINVERT);
	pDC->BitBlt(x, y, tSize.cx, tSize.cy, &newDC, 0, 0, SRCINVERT);
	newDC.SelectObject(pOldBitmap);
	newDC.SelectObject(oldFont);
}

void CNavView::LinkDraw(CDC *pDC, drawData *dd) {
	double Heading = dd->nData.heading;
	double Alt = dd->nData.alt;
	double TAS = dd->nData.tas;
	double Lat = dd->nData.lat;
	double Lon = dd->nData.lon;
	double Mag = dd->nData.mag;
	double mag = Heading + Mag + .5;
	int local = FALSE, blankCallsign = FALSE;
	char call[30];

	if (centerOnAircraft == *((int*) &dd->source)) 
		local = TRUE; // not really local, but show the data for this plane
	if (0 == strlen(dd->nData.callsign)) { // need to build a fake callsign
		if (*((int*) &dd->source) == 0)
			blankCallsign = TRUE;
		else { // use IP address
			sprintf(call, "%s", inet_ntoa(dd->source.sin_addr));
		}
	} else
		strncpy(call, dd->nData.callsign, 30);

	int cx, cy;
	LtoC(Lon, Lat, cx, cy);
	double sa = sin(Heading*degToRad);
	double ca = cos(Heading*degToRad);
	double ux, uy, lx, ly, ldist = 1.0;
	lx = -100*(-sa);
	ly = -100*ca;
	if(flatWorld) { // calculate latitude distortion due to flat map
		ldist = 1.0/cos((top + bottom) *.5 * degToRad);
		lx = (int) (lx * ldist);
	}
	char buf[200];
	int chars;
	CSize tSize;

	CPen *oldPen = (CPen *) pDC->SelectStockObject(BLACK_PEN);	
	pDC->MoveTo(cx, cy);
	pDC->LineTo(cx+(int) lx, cy+(int) ly);
	pDC->SelectObject(oldPen);

	if ((*((int*) &dd->source) == 0) && flying) {
		double dist = gcDist(Lat, Lon, flyingLat, flyingLon)/1852;
		double hours = dist / TAS;
		double mins = 60 * (hours - floor(hours));
		hours = floor(hours);
		chars = sprintf(buf, "FlyTo: %.1f nm,  ETA: %0.lf:%04.1lf", dist, hours, mins);
		tSize = pDC->GetOutputTextExtent(buf, chars);
		CRect client;
		GetClientRect(client);
		LinkDrawText(pDC, client.right-tSize.cx, client.bottom-tSize.cy, buf);
	}

	if (mag > 360) mag -= 360;
	if (mag < 0) mag += 360;
	int offset;

	chars = sprintf(buf, "%.0f deg (%.0f mag)", Heading, mag);
	tSize = pDC->GetOutputTextExtent(buf, chars);
	if (ly<0)
		offset = 30;
	else
		offset = -15 -tSize.cy;
	LinkDrawText(pDC, cx-tSize.cx/2, cy+offset-tSize.cy, buf);

	chars = sprintf(buf, "%.0f ft, %.0f kts", Alt, TAS);
	tSize = pDC->GetOutputTextExtent(buf, chars);
	LinkDrawText(pDC, cx-tSize.cx/2, cy+offset, buf);

	if (!blankCallsign) {
		chars = sprintf(buf, "%s", call);
		tSize = pDC->GetOutputTextExtent(buf, chars);
		if (ly<0)
			LinkDrawText(pDC, cx-tSize.cx/2, cy+offset+tSize.cy, buf);
		else
			LinkDrawText(pDC, cx-tSize.cx/2, cy+offset-2*tSize.cy, buf);
	}

	LOGBRUSH lb;
	lb.lbStyle = BS_SOLID;
	lb.lbColor = RGB(0, 0, 0);
	CPen widePen(PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_BEVEL, 3, &lb);					
	oldPen = (CPen *) pDC->SelectObject(&widePen);	

	if(flatWorld) { // correct rotation angle for latitude
		double ap = atan2(lx, -1*ly);
		sa = sin(ap);
		ca = cos(ap);
	}
	ux = -7*(-sa);
	uy = -7*ca;
	lx = 12*(-sa);
	ly = 12*ca;
	pDC->MoveTo(cx+(int) ux, cy+(int) uy);
	pDC->LineTo(cx+(int) lx, cy+(int) ly);
	ux = -15*ca ;
	uy = -15*sa;
	lx = 15*ca;
	ly = 15*sa;
	pDC->MoveTo(cx+(int) ux, cy+(int) uy);
	pDC->LineTo(cx+(int) lx, cy+(int) ly);
	ux = -3*ca + 10*(-sa);
	uy = -3*sa + 10*ca;
	lx = 3*ca + 10*(-sa);
	ly = 3*sa + 10*ca;
	pDC->MoveTo(cx+(int) ux, cy+(int) uy);
	pDC->LineTo(cx+(int) lx, cy+(int) ly);
 	pDC->SelectObject(oldPen);
}

void CNavView::DoLinkDrawing() {
	if (drawing)
		return;

	EnterCriticalSection(&crit);
	double lat, lon;
	// check for need to recenter map
	int undrawnSize = undrawn.GetSize();
	if (-1 != centerOnAircraft) { // -1 == "None"
		for (int i=0; i<undrawnSize; i++) {
			if (centerOnAircraft == *((int*) &undrawn.data[i]->source)) {
				int xc, yc;
				lat = undrawn.data[i]->nData.lat;
				lon = undrawn.data[i]->nData.lon;
				if (!LtoC(lon, lat, xc, yc)) {
					CenterAt(lat, lon);
					LeaveCriticalSection(&crit);
					return;
				}
			}
		}
	}

	// setup drawing 
	CDC *pDC;
	pDC = GetDC();
	CFont *oldFont;
	CFont font;
	LOGFONT lfnew = lf;
	font.CreateFontIndirect(&lfnew);
	oldFont = (CFont *) pDC->SelectObject(&font);
	int oldROP;
	oldROP = pDC->SetROP2(R2_NOTXORPEN);

	// erase the drawn that have new values in undrawn
	for (int i=0; i<undrawnSize; i++) 
		for (int j=0; j<drawn.GetSize(); j++) 
			if (0 == memcmp(&undrawn.data[i]->source, &drawn.data[j]->source, sizeof(struct sockaddr_in))) 
				LinkDraw(pDC, drawn.data[j]);

	// draw the undrawn, move them to drawn
	time_t timeNow;
	time(&timeNow);
	for (i=0; i<undrawnSize; i++) {
		int back = undrawnSize-1-i; // count backwards
		LinkDraw(pDC, undrawn.data[back]);
		undrawn.data[back]->timeStamp = timeNow;
		drawn.add(new drawData(undrawn.data[back]));
		undrawn.deleteItem(back);
	}
	
	// erase and expire drawn that are too old (30 seconds)
	int drawnSize = drawn.GetSize();
	for (int j=0; j<drawnSize; j++) {
		int back = drawnSize-1-j; // count backwards
		if (drawn.data[back]->timeStamp < (timeNow-30)) {
			LinkDraw(pDC, drawn.data[back]);
			drawn.deleteItem(back);
		}
	}

	// unsetup drawing
	pDC->SetROP2(oldROP);
	pDC->SelectObject(oldFont);
	ReleaseDC(pDC);
	LeaveCriticalSection(&crit);
}

// zoom with mouse scroll wheel 
BOOL CNavView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	if (zDelta < 0)
		PostMessage(WM_COMMAND, ID_VIEW_ZOOMIN, 0);
	else if (zDelta > 0)
		PostMessage(WM_COMMAND, ID_VIEW_ZOOMOUT, 0);
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CNavView::addCraft(CString name, int cls, int clf, int clr, int crs, int crf, int alt, int des, int def, int der) {
	char buf[20];
	long temp;

	// attempt read to see if this one exists
	GetPrivateProfileString(name, "cruisealtitude", "-42", buf, 20, "nav.ini");
	sscanf(buf, "%ld", &temp);
	if (temp == -42) { // read failed, save craft name and increment number
		int craftnumber = GetPrivateProfileInt("Plan", "craftnumber", 0, "nav.ini");
		craftnumber++;
		sprintf(buf, "%d", craftnumber);
		WritePrivateProfileString("Plan", "craftnumber", buf, "nav.ini");
		sprintf(buf, "craft%d", craftnumber);
		WritePrivateProfileString("Plan", buf, name, "nav.ini");
	}

	sprintf(buf, "%d", cls);
	WritePrivateProfileString(name, "climbspeed",     buf, "nav.ini");
	sprintf(buf, "%d", clf);
	WritePrivateProfileString(name, "climbfuel",      buf, "nav.ini");
	sprintf(buf, "%d", clr);
	WritePrivateProfileString(name, "climbrate",      buf, "nav.ini");

	sprintf(buf, "%d", crs);
	WritePrivateProfileString(name, "cruisespeed",    buf, "nav.ini");
	sprintf(buf, "%d", crf);
	WritePrivateProfileString(name, "cruisefuel",     buf, "nav.ini");
	sprintf(buf, "%d", alt);
	WritePrivateProfileString(name, "cruisealtitude", buf, "nav.ini");

	sprintf(buf, "%d", des);
	WritePrivateProfileString(name, "descentspeed",   buf, "nav.ini");
	sprintf(buf, "%d", def);
	WritePrivateProfileString(name, "descentfuel",    buf, "nav.ini");
	sprintf(buf, "%d", der);
	WritePrivateProfileString(name, "descentrate",    buf, "nav.ini");
}

void CNavView::OnMapStartwithlastview() {
	startWithLastView = !startWithLastView;
	char buffer[20];
	sprintf(buffer, "%d", startWithLastView);
	WritePrivateProfileString("View", "startWithLastView", buffer, "nav.ini");
}

void CNavView::OnUpdateMapStartwithlastview(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(startWithLastView);
}

void CNavView::ZoomLastView() {
	if (startWithLastView && !lastViewZoomFinished) {
		char buffer[30];
 		GetPrivateProfileString("View", "lastViewLat", "0", buffer, 30, "nav.ini");
		sscanf(buffer, "%lf", &lastViewLat);
 		GetPrivateProfileString("View", "lastViewLon", "0", buffer, 30, "nav.ini");
		sscanf(buffer, "%lf", &lastViewLon);
		CenterAt(lastViewLat, lastViewLon);
		int zooms = GetPrivateProfileInt("View", "lastViewZoom", 1, "nav.ini");
		for (int i=1; i<zooms; i++)
			OnViewZoomin();
	}
	lastViewZoomFinished = TRUE;
}

void CNavView::OnUpdateFlightsimMapfollowsaircraftNone(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(centerOnAircraft == -1);
	
	// Locate the FS6 submenu
	CMenu* pFileMenu = NULL;
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	int iPos;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_FS6_FS6LINKOPTIONS) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);
	pTopMenu = pFileMenu;
	pFileMenu = NULL;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_FLIGHTSIM_MAPFOLLOWSAIRCRAFT_NONE) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);

	// Update the submenu 
	// First, delete all items after ID_FLIGHTSIM_MAPFOLLOWSAIRCRAFT_LOCAL at position 1
	for (iPos = pFileMenu->GetMenuItemCount()-1; iPos > 1; iPos--)
		pFileMenu->DeleteMenu(iPos, MF_BYPOSITION);
	
	drawDataHolder all;
	GetAllDrawData(all);

	// Then, add an item for each available drawDataHolder
	char call[30];
	for (int i=0; i < all.GetSize(); i++)	{
		if (*((int*) &all.data[i]->source) == 0) // local has it's own menu item
			continue;
		if (0 == strlen(all.data[i]->nData.callsign)) // need to build a fake callsign
			sprintf(call, "%s", inet_ntoa(all.data[i]->source.sin_addr));
		else
			strncpy(call, all.data[i]->nData.callsign, 30);
		pFileMenu->AppendMenu(MF_STRING, WM_NAV+1100+i, call);
	}
}

void CNavView::OnUpdateFlightsimMapfollowsaircraftLocal(CCmdUI* pCmdUI) {
	pCmdUI->SetCheck(centerOnAircraft == 0);				
}

void CNavView::OnFlightsimMapfollowsaircraftNone() {
	centerOnAircraft = -1;
	Invalidate(TRUE);
}

void CNavView::OnFlightsimMapfollowsaircraftLocal() {
	centerOnAircraft = 0;
	Invalidate(TRUE);
}

void CNavView::GetAllDrawData(drawDataHolder &all) {
	for (int i=0; i<undrawn.GetSize(); i++) 
		all.add(new drawData(undrawn.data[i]));

	for (int j=0; j<drawn.GetSize(); j++) 
		all.add(new drawData(drawn.data[j]));
}

