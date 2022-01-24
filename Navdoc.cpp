// Navdoc.cpp : implementation of the CNavDoc class
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

#include <afxtempl.h>
#include "Dpstar.h"
#include "Navdoc.h"
#include "flightpl.h"
#include "navview.h"
#include "math.h"
#include "serbgl.h"
#include "io.h"
#include "mainfrm.h"
#include "export.h"
#include "ExportProgress.h"
#include "NewList.h"
#include "InsertGPSDialog.h"

#define WM_NAV (WM_USER+500)
#define degToMeters  (10001750.0 / 90.0)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavDoc

IMPLEMENT_DYNCREATE(CNavDoc, CDocument)

#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(CNavDoc, CDocument)
	//{{AFX_MSG_MAP(CNavDoc)
	ON_COMMAND(ID_FILE_ADD, OnFileAdd)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADD, OnUpdateFileAdd)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_FILE_SEARCH, OnFileSearch)
	ON_COMMAND(ID_FILE_EXPORT_FLIGHTSHOP, OnFileExport)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_FLIGHTSHOP, OnUpdateFileExport)
	ON_COMMAND(ID_MAP_INSERTGPSFIX, OnMapInsertgpsfix)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADDTOSCENERYLIST_NEW, OnUpdateFileAddtoscenerylistNew)
	ON_COMMAND(ID_FILE_ADDTOSCENERYLIST_NAVLST, OnFileAddtoscenerylistNavlst)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADDTOSCENERYLIST_NAVLST, OnUpdateFileAddtoscenerylistNavlst)
	ON_COMMAND(ID_FILE_LOADSCENERYLIST_NAVLST, OnFileLoadscenerylistNavlst)
	ON_COMMAND(ID_FILE_SEARCHFORBGLFILES_NAVLST, OnFileSearchforbglfilesNavlst)
	ON_COMMAND(ID_FILE_ADDTOSCENERYLIST_NEW, OnFileAddtoscenerylistNew)
	ON_COMMAND(ID_FILE_SEARCHFORBGLFILES_NEW, OnFileSearchforbglfilesNew)
	ON_COMMAND(ID_FILE_SAVECOMPILEDSCENERYLIST, OnFileSavecompiledscenerylist)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVECOMPILEDSCENERYLIST, OnUpdateFileSavecompiledscenerylist)
	ON_COMMAND(ID_FILE_DELETECOMPILEDSCENERYLIST, OnFileDeletecompiledscenerylist)
	ON_UPDATE_COMMAND_UI(ID_FILE_DELETECOMPILEDSCENERYLIST, OnUpdateFileDeletecompiledscenerylist)
	ON_COMMAND(ID_FILE_EXTRACTFS2000WAYPOINTSANDROUTES, OnFileExtractfs2000waypointsandroutes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavDoc construction/destruction

extern CRITICAL_SECTION crit;

CNavDoc::CNavDoc() {
	runwayIndex.SetSize(0,8000);
	ilsIndex.SetSize(0,4000);
	vorIndex.SetSize(0,8000);
	ndbIndex.SetSize(0,8000);
	markerIndex.SetSize(0,8000);
	gpsIndex.SetSize(0,1000);
	airportIndex.SetSize(0,8000);
	atisIndex.SetSize(0,4000);
	dynobjIndex.SetSize(0,1000);
	dynscnIndex.SetSize(0,1000);
	waypointIndex.SetSize(0,8000);	
	routeIndex.SetSize(0,8000);	
	dbFiles.SetSize(0,200);	
	listFiles = 0;
	listFileName="";
	dbCount = 0;
	db.SetSize(0,50000);
	idcounter = 0;
	numbercounter = 0;
	searchingBGLFiles=FALSE;
	magVarTableExists = FALSE;
	compiledSceneryList = FALSE;
	ReadMagVarTable();
	mapsExist = FALSE;
	ReadMaps();
	airportsExist = FALSE;
	includeAFDinfo = TRUE;

	dpStar.readAllFiles(); 
}

CNavDoc::~CNavDoc() {
	ClearDatabases();
}

BOOL CNavDoc::OnNewDocument() {
	if (!CDocument::OnNewDocument())
		return FALSE;
	ClearDatabases();
	compiledSceneryList = FALSE;
	waypointsExist = FALSE;
	ReadWaypoints();
	Index();
	ReadAirports();
	listFileName="";
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	v->init = TRUE;
	v->mag = 1;
	v->mag2 = 1;
	UpdateAllViews(NULL);
	return TRUE;
}

void CNavDoc::ClearDatabases() {
	dbFileIndex = 0;
	dbCount = 0;
	db.RemoveAll();
	db.FreeExtra();
	dbFiles.RemoveAll();
	dbFiles.FreeExtra();
	runwayIndex.RemoveAll();
	runwayIndex.FreeExtra();
	ilsIndex.RemoveAll();
	ilsIndex.FreeExtra();
	vorIndex.RemoveAll();
	vorIndex.FreeExtra();
	ndbIndex.RemoveAll();
	ndbIndex.FreeExtra();
	markerIndex.RemoveAll();
	markerIndex.FreeExtra();
	gpsIndex.RemoveAll();
	gpsIndex.FreeExtra();
	airportIndex.RemoveAll();
	airportIndex.FreeExtra();
	atisIndex.RemoveAll();
	atisIndex.FreeExtra();
	dynobjIndex.RemoveAll();
	dynobjIndex.FreeExtra();
	dynscnIndex.RemoveAll();
	dynscnIndex.FreeExtra();
	waypointIndex.RemoveAll();
	waypointIndex.FreeExtra();
	routeIndex.RemoveAll();
	routeIndex.FreeExtra();
}

/////////////////////////////////////////////////////////////////////////////
// CNavDoc serialization

void CNavDoc::Serialize(CArchive& ar) {
	if (ar.IsStoring())	{
		// TODO: add storing code here
	} else {
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CNavDoc diagnostics

#ifdef _DEBUG
void CNavDoc::AssertValid() const {
	CDocument::AssertValid();
}

void CNavDoc::Dump(CDumpContext& dc) const {
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNavDoc commands

BOOL CNavDoc::OnOpenDocument(LPCTSTR lpszPathName) {
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	dbFileIndex = 0;
	dbCount = 0;
	db.RemoveAll();
 	db.FreeExtra();
	db.SetSize(0,5000);
	dbFiles.RemoveAll();
	dbFiles.FreeExtra();
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	v->mag = 1;
	v->mag2 = 1;
	UpdateAllViews(NULL);

	fName = lpszPathName;
	dbFileIndex = 0;
    dbFiles.Add(fName);
	readFile(lpszPathName);
	SetTitle(lpszPathName);
	waypointsExist = FALSE;
	ReadWaypoints();
	Index();
	ReadAirports();
	v->ZoomToFit();
	compiledSceneryList = FALSE;
	return TRUE;
}

// read 3 bytes as a signed int
int CNavDoc::read3bytes(char *b) {
	char i[4];
	i[0] = 0;
	i[1] = *b;
	i[2] = *(b+1);
	i[3] = *(b+2);
	int lat = *((int *) i);
	lat >>= 8; 
	return(lat);
}

// read 3 bytes as a signed int, 1 byte as fractional part
double CNavDoc::readDM(char *b) {
	unsigned char dec = (unsigned char) b[0];
	int lat = read3bytes(&b[1]);
	if (lat > 0)
		return( (double) lat + dec / 256.0 );
	else
		return( (double) lat - dec / 256.0 );
}

// read 6 latitude bytes in "Micro" format
double CNavDoc::readMicro6Lat(char *b) {
	unsigned short fractPart;
	int intPart;
	intPart = *(int*)(b+2);
	fractPart = *(unsigned short *)b;
	if (intPart >= 0)
		return(intPart + fractPart / 65536.0);
	else
		return(intPart - fractPart / 65536.0);
}

// read 6 longitude bytes in "Micro" format ???
double CNavDoc::readMicro6Lon(char *b) {
	unsigned short fractPart;
	int intPart;
	intPart = read3bytes(b+3);
	fractPart = *(unsigned short *)b;
	if (intPart >= 0)
		return(intPart + fractPart / 65536.0);
	else
		return(intPart - fractPart / 65536.0);
}

// read 4 bytes in "Micro" format
double CNavDoc::readMicro4(char *b) {
	unsigned short fractPart;
	short intPart;
	intPart = *(short *)(b+2);
	fractPart = *(unsigned short *)b;
	if (intPart >= 0)
		return(intPart + fractPart / 65536.0);
	else
		return(intPart - fractPart / 65536.0);
}

int CNavDoc::readFile(CString filename) {
	char *fname=filename.GetBuffer(1);
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	HANDLE filehandle, filemapping;
	char *bgl;
	int count = 0;
	char buffer[100];
	CString names[1000];

	// find file size
	WIN32_FIND_DATA fd;
	HANDLE findHandle = FindFirstFile(filename, &fd);
	if (findHandle == INVALID_HANDLE_VALUE) {
		sprintf(buffer, "Could not open %s\n", filename);
		AfxMessageBox(buffer);
		return(2);
	}	
	int size = (int) fd.nFileSizeLow; // assume its not more than 32 bits
	int maxSize = size;
	FindClose(findHandle);

	// A zero length BGL file was causing a "file mapping failed" error for
	// every file loaded until the next reboot. This would never happen with Linux :)
	if (size <= 0) {
		sprintf(buffer, "%s seems to have a bogus length, skipping it\n", filename);
		AfxMessageBox(buffer, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
		return(5);
	}

	filehandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (filehandle == INVALID_HANDLE_VALUE) {
		sprintf(buffer, "Could not open %s\n", filename);
		AfxMessageBox(buffer);
		return(2);
	}	

	filemapping = CreateFileMapping(filehandle, NULL, PAGE_READONLY, 0, 0, "bglfile");
	DWORD err = GetLastError();
	if ((filemapping != NULL) && (err == ERROR_ALREADY_EXISTS)) {
		sprintf(buffer, "File mapping failed, object already exists\n, file: %s", filename);
		AfxMessageBox(buffer);
		CloseHandle(filemapping);
		CloseHandle(filehandle);
		return(3);
	} else if (filemapping == NULL) {
		sprintf(buffer, "File mapping failed, error code %d, file: %s\n", err, filename);
		AfxMessageBox(buffer);
		CloseHandle(filehandle);
		return(3);
	}	

	bgl = (char *) MapViewOfFile(filemapping, FILE_MAP_READ, 0, 0, 0); 
	if (bgl == NULL) {
		CloseHandle(filemapping);
		CloseHandle(filehandle);
		sprintf(buffer, "Could not map file: %s\nMaybe your virtual memory setting is too low", filename);
		AfxMessageBox(buffer);
		return(4);
	}
	// finished mapping file to memory

  	int sect0  = *((int *) &bgl[18]);  //used
  	int sect9  = *((int *) &bgl[58]);  //used
	int sect10 = *((int *) &bgl[62]);  //used
 	int sect11 = *((int *) &bgl[66]);  //used 
  	int sect12 = *((int *) &bgl[70]);
  	int sect13 = *((int *) &bgl[74]);  //used
 	int sect14 = *((int *) &bgl[78]);  //used
  	int sect15 = *((int *) &bgl[82]);  //used
 	int sect16 = *((int *) &bgl[102]); //used
  	int sect17 = *((int *) &bgl[106]);
  	int sect18 = *((int *) &bgl[110]); //used
  	int sect19 = *((int *) &bgl[114]);
  	int sect20 = *((int *) &bgl[118]); //used
  	int sect21 = *((int *) &bgl[122]);

	if (sect0 > size)  sect0 = 0;	// limit damage caused by bogus headers
	if (sect9 > size)  sect9 = 0;	
	if (sect10 > size) sect10 = 0;	
	if (sect11 > size) sect11 = 0;	
	if (sect12 > size) sect12 = 0;	
	if (sect13 > size) sect13 = 0;	
	if (sect14 > size) sect14 = 0;	
	if (sect15 > size) sect15 = 0;	
	if (sect16 > size) sect16 = 0;	
	if (sect17 > size) sect17 = 0;	
	if (sect18 > size) sect18 = 0;	
	if (sect19 > size) sect19 = 0;	
	if (sect20 > size) sect20 = 0;	
	if (sect21 > size) sect21 = 0;	

	// try to find where section 9 ends
	if ((sect0  > sect9) && (sect0 < size))   size = sect0;
	if ((sect10 > sect9) && (sect10 < size))  size = sect10;
	if ((sect11 > sect9) && (sect11 < size))  size = sect11;
	if ((sect12 > sect9) && (sect12 < size))  size = sect12;
	if ((sect13 > sect9) && (sect13 < size))  size = sect13;
	if ((sect14 > sect9) && (sect14 < size))  size = sect14;
	if ((sect15 > sect9) && (sect15 < size))  size = sect15;
	if ((sect16 > sect9) && (sect16 < size))  size = sect16;
	if ((sect17 > sect9) && (sect17 < size))  size = sect17;
	if ((sect18 > sect9) && (sect18 < size))  size = sect18;
	if ((sect19 > sect9) && (sect19 < size))  size = sect19;
	if ((sect20 > sect9) && (sect20 < size))  size = sect20;
	if ((sect21 > sect9) && (sect21 < size))  size = sect21;

	// need to keep track of VORs found by different methods
	vorDB.SetSize(0,100);

	// section 20: FS98 Airports
	if ((sect20 == 0x87654321) && (includeAFDinfo||!searchingBGLFiles)) {
		double lastAptLat, lastAptLon, lastRwyLat, lastRwyLon, lastRwyMagVar;
		CString lastAptName;
/*
//		FILE *dump = fopen("dump.txt", "w");
		int descOffset = *((UNALIGNED int *) &bgl[0xA4]);
		int chunk = 0;
		while(*((UNALIGNED int *) &bgl[descOffset+chunk]) == 0x00000001) {
			int chunkSize = *((UNALIGNED int *) &bgl[descOffset+chunk + 4]);
			int type = *((UNALIGNED int *) &bgl[descOffset+chunk + 8]);
			if (type == 6) { // waypoint
				int subChunk = descOffset+chunk + 16;
				while (0x00010008 == *((UNALIGNED int *) &bgl[subChunk])) {
					int subChunkSize = *((UNALIGNED int *) &bgl[subChunk + 4]);
					int lati = *((UNALIGNED int *) &bgl[subChunk + 12]); 
					int loni = *((UNALIGNED int *) &bgl[subChunk + 20]); 
					double lat = ((double)lati) / degToMeters;
					double lon = ((double)loni) / 0x7fffffff * 180;
					char name[8];
					strncpy(name, &bgl[subChunk + 28], 7);
					name[7]=0x00;
					ASSERT(strlen(name) > 2);
//					fprintf(dump, "\n%s\t%lf\t%lf", name, lat, lon);
					subChunk += subChunkSize + 8;
				}
			} else if (type == 7) { // victor airway
				int subChunk = descOffset+chunk + 16;
				if (0x00010006 == *((UNALIGNED int *) &bgl[subChunk])) {
					int nameSize = *((UNALIGNED int *) &bgl[subChunk + 4]);
					char awyName[20];
					strncpy(awyName, &bgl[subChunk + 8], nameSize);
					awyName[nameSize]=0x00;
					//fprintf(dump, "\n%s", awyName);
					int item = subChunk + 8 + nameSize;
					while (0x00011002 == *((UNALIGNED int *) &bgl[item])) {
						int itemSize = *((UNALIGNED int *) &bgl[item + 4]);
						// try 8 char ids instead of 6
						char name[8];
						strncpy(name, &bgl[item + 8], 7);
						name[7]=0x00;
						ASSERT(strlen(name) > 2);
						//fprintf(dump, "-%s", name);
						item += itemSize + 8; 
					}
				}
			} else if (type == 8) { // jet airway
				int subChunk = descOffset+chunk + 16;
				if (0x00010006 == *((UNALIGNED int *) &bgl[subChunk])) {
					int nameSize = *((UNALIGNED int *) &bgl[subChunk + 4]);
					char awyName[20];
					strncpy(awyName, &bgl[subChunk + 8], nameSize);
					awyName[nameSize]=0x00;
					//fprintf(dump, "\n%s", awyName);
					int item = subChunk + 8 + nameSize;
					while (0x00011002 == *((UNALIGNED int *) &bgl[item])) {
						int itemSize = *((UNALIGNED int *) &bgl[item + 4]);
						// try 8 char ids instead of 6
						char name[8];
						strncpy(name, &bgl[item + 8], 7);
						name[7]=0x00;
						ASSERT(strlen(name) > 2);
						//fprintf(dump, "-%s", name);
						item += itemSize + 8; 
					}
				}
			}
			chunk += chunkSize + 8;
		}
//		fclose(dump);
*/
		int nameOffset = *((UNALIGNED int *) &bgl[0x9c]);
		for (int i=0; i<4; i++) { //for each language
			if (*((UNALIGNED int *) &bgl[nameOffset]) != 0x00030001) 
				break;
			int chunkSize = *((UNALIGNED int *) &bgl[nameOffset + 4]);
			// 0x409 for english, 0x40c for french, 0x407 for german, 0x410 for spanish
			int language = *((UNALIGNED int *) &bgl[nameOffset + 8]);
			ASSERT(language == 0x409);
			if (language != 0x409) // read this language data (English)
				continue;
			
			int subChunk = nameOffset + 24;
			while(*((UNALIGNED int *) &bgl[subChunk]) == 0x00030002) {
				int subChunkSize = *((UNALIGNED int *) &bgl[subChunk + 4]);
				int dataType = *((UNALIGNED int *) &bgl[subChunk + 8]);
				int dataOffset = *((UNALIGNED int *) &bgl[subChunk + 12]);
				char *dataName = (UNALIGNED char *) &bgl[subChunk + 16];

				if (dataType == 0x00010001) {			// continent
					; //ignore
				} else if (dataType == 0x00020001) {	// country
					; //ignore
				} else if (dataType == 0x00030001) {	// state
					; //ignore
				} else if (dataType == 0x00040001) {	// town
					; //ignore
				} else if (dataType == 0x00050005) {	// some new fs2000 thing???
					; // just ignore it for now
					ASSERT(*((UNALIGNED int *) &bgl[dataOffset]) == 0x00000001);
					int vSize = *((UNALIGNED int *) &bgl[dataOffset + 4]);
					int type = *((UNALIGNED int *) &bgl[dataOffset + 8]);
					ASSERT(type == 5); // ???
					ASSERT(*((UNALIGNED int *) &bgl[dataOffset + 16]) == 0x00010007); //sub chunk id
					int vSubSize = *((UNALIGNED int *) &bgl[dataOffset + 20]);
					;/////// don't know what comes next

				} else if (dataType == 0x00050001) {	// airport
					int airOff = dataOffset;
					if (*((UNALIGNED int *) &bgl[airOff]) == 0x00000001) {
						int aSize = *((UNALIGNED int *) &bgl[airOff + 4]);
						int aiSize, scanId, scanPtr = airOff +8;
zeroLengthAI:
						while ((scanId=*((UNALIGNED int *) &bgl[scanPtr])) != 0) {
							aiSize = *((UNALIGNED int *) &bgl[scanPtr + 4]); // sub chunk size
							if (aiSize == 0) {
								scanPtr += 8;
								goto zeroLengthAI;
							}

							if (scanId == 0x00010001) { //sub chunk id
								int lati = *((UNALIGNED int *) &bgl[scanPtr + 12]); 
								int loni = *((UNALIGNED int *) &bgl[scanPtr + 20]); 
								int alt = *((UNALIGNED int *) &bgl[scanPtr + 28]); // meters
								char aID[6];
								strncpy(aID, &bgl[scanPtr + 44], 4);
								aID[4] = 0x00;
								char aName[31];
								strncpy(aName, dataName, 24);
								aName[24] = 0x00;
								double lat = ((double)lati) / degToMeters;
								double lon = ((double)loni) / 0x7fffffff * 180;
								lastAptLat = lat;
								lastAptLon = lon;
								lastAptName = CString(aName);
								// check to make sure this airport has not already been found
								if (!duplicateAirport(aName, lat, lon)) {
									addDB(4, lat, lon, alt, 0, aID, aName, 0, 0, 0, fname); 
									count++;
									if (searchingBGLFiles==TRUE) {
										if (UnmapViewOfFile(bgl) == FALSE)
											AfxMessageBox("UnmapViewOfFile failed");
										if (CloseHandle(filemapping) == FALSE)
											AfxMessageBox("Could not close filemapping handle");
										if (CloseHandle(filehandle) == FALSE)
											AfxMessageBox("Could not close file handle");
										return(0);
									}
								}
							} else if (scanId == 0x00010005) { // aux runway info
								;
							} else if (scanId == 0x00010002) { // main runway record
								int lati = *((UNALIGNED int *) &bgl[scanPtr + 12]); 
								int loni = *((UNALIGNED int *) &bgl[scanPtr + 20]); 
								double lat = ((double)lati) / degToMeters;
								lastRwyLat = lat;
								double lon = ((double)loni) / 0x7fffffff * 180;
								lastRwyLon = lon;
								int alt = *((UNALIGNED int *) &bgl[scanPtr + 28]); // meters
								int rwHeadi = *((UNALIGNED int *) &bgl[scanPtr + 32]); 
								double rwHead = rwHeadi / 65536.0;
								rwHead /= 182.04;
								int rwMagVari = *((UNALIGNED int *) &bgl[scanPtr + 36]); 
								double rwMagVar = rwMagVari / 65536.0;
								rwMagVar /= -182.04;
								lastRwyMagVar = rwMagVar;
								double length = *((UNALIGNED short *) &bgl[scanPtr + 40]); // ft
								double width = *((UNALIGNED short *) &bgl[scanPtr + 44]); //ft
								int lrcType = *((UNALIGNED int *) &bgl[scanPtr + 48]); // runway, left, right, center, hellipad = 0,1,2,3,4
								unsigned char rwID = *((unsigned char *) &bgl[scanPtr + 52]);
								unsigned char rwID2 = ' ';
								if (lrcType == 1)
									rwID2 = 'L';
								if (lrcType == 2)
									rwID2 = 'R';
								if (lrcType == 3)
									rwID2 = 'C';
								if (lrcType == 4)
									rwID2 = 'H';
								if (rwID == 37)			// north/south
									rwID = 36;
								else if (rwID == 38)	// east/west
									rwID = 9;
								else if (rwID == 39)	// nw/se
									rwID = 13;
								else if (rwID == 40)	// sw/ne
									rwID = 4;
								char surface = *((char *) &bgl[scanPtr + 56]);
								char rwName[6];
								int c=sprintf(rwName, "%d", (unsigned) rwID);
								sprintf(&rwName[c], "%c", rwID2);
								addDB(8, lat, lon, (int) alt, (double) surface, rwName, "Runway", (double) width, rwHead, (double) length, fname); 
								count++;
								if (searchingBGLFiles==TRUE) {
									if (UnmapViewOfFile(bgl) == FALSE)
										AfxMessageBox("UnmapViewOfFile failed");
									if (CloseHandle(filemapping) == FALSE)
										AfxMessageBox("Could not close filemapping handle");
									if (CloseHandle(filehandle) == FALSE)
										AfxMessageBox("Could not close file handle");
									return(0);
								}
							} else if (scanId == 0x00010004) { // ILS/Marker record
								int lati = *((UNALIGNED int *) &bgl[scanPtr + 12]); 
								int loni = *((UNALIGNED int *) &bgl[scanPtr + 20]); 
								int alt =  *((UNALIGNED int *) &bgl[scanPtr + 28]); // meters
								int rwHeadi = *((UNALIGNED int *) &bgl[scanPtr + 32]); 
								double rwHead = rwHeadi / 65536.0;
								rwHead /= 182.04;
								double lat = ((double)lati) / degToMeters;
								double lon = ((double)loni) / 0x7fffffff * 180;
								double ilsFreq;
								// type seems to be 1, 2, or 3
								int type = *((UNALIGNED int *) &bgl[scanPtr + 40]); 
								int typeclass = *((UNALIGNED int *) &bgl[scanPtr + 44]); 
								char ilsID[5];
								strncpy(ilsID, &bgl[scanPtr + 56], 4);
								ilsID[4] = 0x00;
								if (type == 7) { // FACILITY_NAVAID_MARKER
									if (typeclass == 8) // OM
										addDB(7, lat, lon, alt, 0, "", "", 0, 0, 0, fname); 
									else if (typeclass == 9) // MM
										addDB(6, lat, lon, alt, 0, "", "", 0, 0, 0, fname); 
									else if (typeclass == 10) // OM
										addDB(5, lat, lon, alt, 0, "", "", 0, 0, 0, fname); 
								} else if (type == 4) { // localizer
									ASSERT(ilsID[0] != 0x00);
									ilsFreq = *((UNALIGNED char *) &bgl[scanPtr + 39]) * 100; // BCD
									ilsFreq += ((*((UNALIGNED char *) &bgl[scanPtr + 38]) & 0xf0) >> 4) * 10;
									ilsFreq +=  (*((UNALIGNED char *) &bgl[scanPtr + 38]) & 0x0f);
									ilsFreq += ((*((UNALIGNED char *) &bgl[scanPtr + 37]) & 0xf0) >> 4) / 10.0;
									ilsFreq +=  (*((UNALIGNED char *) &bgl[scanPtr + 37]) & 0x0f) / 100.0;
									// need to flip localizer to oposite end of runway to draw cone properly
									double dlat = lastRwyLat - lat;
									double dlon = lastRwyLon - lon;
									lat += 2*dlat;
									lon += 2*dlon;
									// should rwHead take magvar into account???
									addDB(2, lat, lon, alt, ilsFreq, ilsID, ilsID, lastRwyMagVar, rwHead, 0, fname); 				
								} 
							} else if (scanId == 0x00020001) { // ATIS/COM freq record
								double commFreq;
								commFreq = *((UNALIGNED char *) &bgl[scanPtr + 11]) * 100; // BCD
								commFreq += ((*((UNALIGNED char *) &bgl[scanPtr + 10]) & 0xf0) >> 4) * 10;
								commFreq +=  (*((UNALIGNED char *) &bgl[scanPtr + 10]) & 0x0f);
								commFreq += ((*((UNALIGNED char *) &bgl[scanPtr + 9]) & 0xf0) >> 4) / 10.0;
								commFreq +=  (*((UNALIGNED char *) &bgl[scanPtr + 9]) & 0x0f) / 100.0;
								int commType = *((UNALIGNED int *) &bgl[scanPtr+12]);
								if (commType == 1) { // it's an ATIS, need preferred runways
									addDB(10, lastAptLat , lastAptLon , 0, commFreq, "ATIS ", lastAptName.GetBuffer(1), 0, 0, 0, fname); 				
								}
							} 
	//						else { //???
	//							sprintf(buffer, "unknown runway scanId: %x", scanId);
	//							AfxMessageBox(buffer);
	//						}
							scanPtr += aiSize + 8;
						}
						airOff += aSize + 8;
					}
				} else if (dataType == 0x00050003) {	// VOR
					int vorOff = dataOffset;

					ASSERT(*((UNALIGNED int *) &bgl[dataOffset]) == 0x00000001);
					int vSize = *((UNALIGNED int *) &bgl[dataOffset + 4]);
					int type = *((UNALIGNED int *) &bgl[dataOffset + 8]);
					ASSERT(type == 2); // VOR

					int subChunkOff = 16;
					char lastVorID[5];
					lastVorID[0] = 0x00;
					while (*((UNALIGNED int *) &bgl[dataOffset + subChunkOff]) == 0x00010004) {
						int vSubSize = *((UNALIGNED int *) &bgl[dataOffset + subChunkOff+4]);
						int lati = *((UNALIGNED int *) &bgl[dataOffset + subChunkOff+12]); 
						int loni = *((UNALIGNED int *) &bgl[dataOffset + subChunkOff+20]); 
						int alt =  *((UNALIGNED int *) &bgl[dataOffset + subChunkOff+28]); // meters
						int vorSubType = *((UNALIGNED int *) &bgl[dataOffset + subChunkOff+40]); 
						if (vorSubType != 8) { // skip TACANs
							double freq = *((UNALIGNED char *) &bgl[dataOffset + subChunkOff+39]) * 100; // BCD
							freq += ((*((UNALIGNED char *) &bgl[dataOffset + subChunkOff+38]) & 0xf0) >> 4) * 10;
							freq +=  (*((UNALIGNED char *) &bgl[dataOffset + subChunkOff+38]) & 0x0f);
							freq += ((*((UNALIGNED char *) &bgl[dataOffset + subChunkOff+37]) & 0xf0) >> 4) / 10.0;
							freq +=  (*((UNALIGNED char *) &bgl[dataOffset + subChunkOff+37]) & 0x0f) / 100.0;
							char vorID[6];
							strncpy(vorID, &bgl[dataOffset + subChunkOff+56], 4);
							vorID[4] = 0x00;
							if (0 != strncmp(lastVorID, vorID, 4))	{ // skip VOR/DME duplicates
								strncpy(lastVorID, vorID, 4);
								char vorName[25];
								strncpy(vorName, dataName, 24);
								vorName[24] = 0x00;
								double lat = ((double)lati) / degToMeters;
								double lon = ((double)loni) / 0x7fffffff * 180;

								double mv = GetMagVar(lat, lon); // probably not right, but better than 0?
								// no proper magvar data, no range data, & duplicates data in "normal" BGL files
								addDB(3, lat, lon, alt, freq, vorID, vorName, mv, 0, 0.0, fname); 	
							}
						}
						subChunkOff += 8+vSubSize;
					}
// */
				} else if (dataType == 0x00050004) {	// NDB
					// finds NDBs with names, but misses one ones without
					int ndbOff = dataOffset;
					if (*((UNALIGNED int *) &bgl[ndbOff]) == 0x00000001) {
						int vSize = *((UNALIGNED int *) &bgl[ndbOff + 4]);
						int type = *((UNALIGNED int *) &bgl[ndbOff + 8]);
						ASSERT(type == 3); // NDB
						int subChunkOff = 16;
						// "if" should be "while"?
						if (*((UNALIGNED int *) &bgl[ndbOff + subChunkOff]) == 0x00010004) {
							int vSubSize = *((UNALIGNED int *) &bgl[ndbOff + subChunkOff + 4]);
							int ndbType = *((UNALIGNED int *) &bgl[ndbOff + subChunkOff+40]); 
							int ndbClass = *((UNALIGNED int *) &bgl[ndbOff + subChunkOff+44]); 	
							int lati = *((UNALIGNED int *) &bgl[ndbOff + subChunkOff + 12]); 
							int loni = *((UNALIGNED int *) &bgl[ndbOff + subChunkOff + 20]); 
							int alt =  *((UNALIGNED int *) &bgl[ndbOff + subChunkOff + 28]); // meters
							double freq = ((*((UNALIGNED char *) &bgl[ndbOff + subChunkOff + 37]) & 0xf0) >> 4) * 100;
							freq +=  (*((UNALIGNED char *) &bgl[ndbOff + subChunkOff + 37]) & 0x0f) * 10;
							freq += ((*((UNALIGNED char *) &bgl[ndbOff + subChunkOff + 36]) & 0xf0) >> 4);
							freq +=  (*((UNALIGNED char *) &bgl[ndbOff + subChunkOff + 36]) & 0x0f) / 10.0;
							char ndbID[5];
							strncpy(ndbID, &bgl[ndbOff + subChunkOff + 56], 4);
							ndbID[4] = 0x00;
							char ndbName[25];
							strncpy(ndbName, dataName, 24);
							ndbName[24] = 0x00;
							double lat = ((double)lati) / degToMeters;
							double lon = ((double)loni) / 0x7fffffff * 180;
							//  no range data
							addDB(1, lat, lon, alt, freq, ndbID, ndbName, 0.0, 0, 0.0, fname); 								
							vorRec r; // keep track of found VORs
							r.freq = freq;
							r.lat = lat;
							r.lon = lon;
							vorDB.Add(r);

							subChunkOff += 8+vSubSize;
						}
						ndbOff += 8+vSize;
					}
				}
				subChunk += subChunkSize + 8;
			}
			nameOffset += chunkSize + 8;
		}
	}

	// brute force way: scan for NDB signatures
	// finds all NDBs, but not their names
	if ((sect20 == 0x87654321) && (includeAFDinfo||!searchingBGLFiles)) {
		int end = maxSize - 79;
		char sig[] = {(char)0x01,(char)0x00,(char)0x00,(char)0x00,
			          (char)0x4c,(char)0x00,(char)0x00,(char)0x00,
			 		  (char)0x03,(char)0x00,(char)0x00,(char)0x00,
					  (char)0x00,(char)0x00,(char)0x00,(char)0x00,
					  (char)0x04,(char)0x00,(char)0x01,(char)0x00,
					  (char)0x34,(char)0x00,(char)0x00,(char)0x00,
					  (char)0x00,(char)0x00,(char)0x00,(char)0x00};
		for (int w=0; w<end; w++) {
			if (0 == memcmp(&bgl[w], sig, 28)) {
				int ndbType = *((UNALIGNED int *) &bgl[w+56]); 
				if (3 == ndbType) {
					int lati = *((UNALIGNED int *) &bgl[w + 28]); 
					int loni = *((UNALIGNED int *) &bgl[w + 36]); 
					int alt =  *((UNALIGNED int *) &bgl[w + 44]); // meters
					double freq = ((*((UNALIGNED char *) &bgl[w + 53]) & 0xf0) >> 4) * 100;
					freq +=  (*((UNALIGNED char *) &bgl[w + 53]) & 0x0f) * 10;
					freq += ((*((UNALIGNED char *) &bgl[w + 52]) & 0xf0) >> 4);
					freq +=  (*((UNALIGNED char *) &bgl[w + 52]) & 0x0f) / 10.0;
					char ndbID[5];
					strncpy(ndbID, &bgl[w + 72], 4);
					ndbID[4] = 0x00;
					double lat = ((double)lati) / degToMeters;
					double lon = ((double)loni) / 0x7fffffff * 180;
					// only add if not already found
					int found = FALSE;
					for (int v=0; v<vorDB.GetSize(); v++) {
						if (vorDB[v].freq == freq) 
							if (.1 > fabs(lat-vorDB[v].lat))
								if (.1 > fabs(lon-vorDB[v].lon)) {
									found = TRUE;
									break;
								}					
					}
					if (!found)
						addDB(1, lat, lon, alt, freq, ndbID, ndbID, 0.0, 0, 0.0, fname); 								
					// no range data
					w += 79; 
				}
			}
		}
		vorDB.RemoveAll();
		vorDB.FreeExtra();
	}
	// section 14: NDB
	if ((sect14 != 0) && (!includeAFDinfo||!searchingBGLFiles))	{
		int rangeTableOffset = 0;
		// for each latitude range
		while (bgl[sect14 + rangeTableOffset] == 0x15) {
			short minLat = *((UNALIGNED short *) &bgl[sect14 + rangeTableOffset+1]);
			short maxLat = *((UNALIGNED short *) &bgl[sect14 + rangeTableOffset+3]);
			int ndbOffset = *((UNALIGNED int *) &bgl[sect14 + rangeTableOffset+5]);
			// for each station in this range
			while (bgl[sect14 + ndbOffset] == 0x04) {
				int lat = read3bytes(&bgl[sect14 + ndbOffset + 4]);
				int lon = read3bytes(&bgl[sect14 + ndbOffset + 7]);
				// many ndb altitudes are -9999, why?
				char freqbcd = *((char *) &bgl[sect14 + ndbOffset + 1]);
				char freqhun = *((char *) &bgl[sect14 + ndbOffset + 2]);
				char freqdec = *((char *) &bgl[sect14 + ndbOffset + 3]);
				char freqten = (freqbcd & 0xF0) >> 4;
				char freqone = (freqbcd & 0x0F);
				double freq = (double) freqhun * 100. + (double) freqten * 10. + (double) freqone + (double) freqdec * .1;
				short alt = *((UNALIGNED short *) &bgl[sect14 + ndbOffset + 10]);
				char range = *((char *) &bgl[sect14 + ndbOffset + 12]);
				char ndbID[6];
				strncpy(ndbID, &bgl[sect14 + ndbOffset + 13], 5);
				ndbID[5] = 0x00;
				char ndbName[25];
				strncpy(ndbName, &bgl[sect14 + ndbOffset + 18], 24);
				ndbName[24] = 0x00;
				addDB(1, (double) lat * 90.0 / 5000875.0, (double) lon / (double) 0x1000000 * 360.0, alt, freq, ndbID, ndbName, 0, 0, (double) range * 2048.0 / 1000.0); 				

				ndbOffset += 42;
				count++;
				if (searchingBGLFiles==TRUE) {
					if (UnmapViewOfFile(bgl) == FALSE)
						AfxMessageBox("UnmapViewOfFile failed");
					if (CloseHandle(filemapping) == FALSE)
						AfxMessageBox("Could not close filemapping handle");
					if (CloseHandle(filehandle) == FALSE)
						AfxMessageBox("Could not close file handle");
					return(0);
				}
			}
			rangeTableOffset += 9;
		}
	}

	// section 13: ATIS
	if ((sect13 != 0) && (!includeAFDinfo||!searchingBGLFiles))	{
		int rangeTableOffset = 0;
		// for each latitude range
		while (bgl[sect13 + rangeTableOffset] == 0x15) {
			short minLat = *((UNALIGNED short *) &bgl[sect13 + rangeTableOffset+1]);
			short maxLat = *((UNALIGNED short *) &bgl[sect13 + rangeTableOffset+3]);
			int ndbOffset = *((UNALIGNED int *) &bgl[sect13 + rangeTableOffset+5]);
			// for each station in this range
			while (bgl[sect13 + ndbOffset] == 0x04) {
				char freqdec = *((char *) &bgl[sect13 + ndbOffset + 1]);
				char freqhun = *((char *) &bgl[sect13 + ndbOffset + 2]);
				double lat = read3bytes(&bgl[sect13 + ndbOffset + 3]) * 90.0 / 5000875.0;
				double lon = read3bytes(&bgl[sect13 + ndbOffset + 6]) / (double) 0x1000000 * 360.0;
				int range = *((char *) &bgl[sect13 + ndbOffset + 9]) * 2048; // in meters
				unsigned short recLength = *((unsigned short *) &bgl[sect13 + ndbOffset + 10]);
				char pr1 = *((char *) &bgl[sect13 + ndbOffset + 12]); // preferred runways
				char pr2 = *((char *) &bgl[sect13 + ndbOffset + 13]);
				char pr3 = *((char *) &bgl[sect13 + ndbOffset + 14]);
				char pr4 = *((char *) &bgl[sect13 + ndbOffset + 15]);
				unsigned int allRunways = *((unsigned int *) &bgl[sect13 + ndbOffset + 12]); 
				char freqtenth    = (freqdec & 0xF0) >> 4;
				char freqhundreth = (freqdec & 0x0F);
				char freqone = (freqhun & 0x0F);
				char freqten = (freqhun & 0xF0) >> 4;
				double freq = 100 + freqten * 10. + freqone + freqtenth * .1 + freqhundreth * .01;

				// generate DMS name, since ATIS don't have names
				char buf[26];
				char dirx, diry;
				if (lon < 0.) 
					dirx = 'W'; 
				else 
					dirx = 'E';
				if (lat < 0.) 
					diry = 'S'; 
				else 
					diry = 'N';
				int xdeg, xmin, ydeg, ymin;
				double xsec, ysec, gpslat, gpslon;
				gpslat = fabs(lat);
				gpslon = fabs(lon);
				xdeg = (int) gpslon;
				xmin = (int) ((gpslon - xdeg) * 60);
				xsec = (gpslon - xdeg - xmin / 60.0) * 3600;
				if (xsec >= 59.995) { // round to .01 seconds
					xsec = 0.0;
					xmin += 1;
				}
				if (xmin >= 60) {
					xmin -= 60;
					xdeg += 1;
				}

				ydeg = (int) gpslat;
				ymin = (int) ((gpslat - ydeg) * 60);
				ysec = (gpslat - ydeg - ymin / 60.0) * 3600;
				if (ysec >= 59.995) { // round to .01 seconds
					ysec = 0.0;
					ymin += 1;
				}
				if (ymin >= 60) {
					ymin -= 60;
					ydeg += 1;
				}
			
				sprintf(buf, "ATIS %d %d'%2.0f\"%c, %d %d'%2.0f\"%c", ydeg, ymin, ysec, diry, xdeg, xmin, xsec, dirx);
				CString name(buf);

				addDB(10, lat , lon , 0, freq, "ATIS ", name.GetBuffer(1), 0, 0, allRunways); 				
				name.ReleaseBuffer();
				ndbOffset += recLength;
				count++;
				if (searchingBGLFiles==TRUE) {
					if (UnmapViewOfFile(bgl) == FALSE)
						AfxMessageBox("UnmapViewOfFile failed");
					if (CloseHandle(filemapping) == FALSE)
						AfxMessageBox("Could not close filemapping handle");
					if (CloseHandle(filehandle) == FALSE)
						AfxMessageBox("Could not close file handle");
					return(0);
				}
			}
			rangeTableOffset += 9;
		}
	}

	// section 0: VOR and ILS
    short minS0freq = *((short *) &bgl[22]);
    short maxS0freq = *((short *) &bgl[24]);
	if ((sect0 != 0) && (!includeAFDinfo||!searchingBGLFiles)) {
		// for each frequency
		for (int i=0; i<=maxS0freq-minS0freq; i++) {
			if(bgl[sect0+5*i] == 0x01) {
				int sectOffset = *((UNALIGNED int *) &bgl[sect0+5*i+1]);
				if ((sectOffset > 0) && ((sect0+sectOffset) < maxSize)) {
					double freq = 108 + (minS0freq + i) * .05;
					int rangeTableOffset = 0;
					// for each latitude range
					while (bgl[sect0 + sectOffset + rangeTableOffset] == 0x15) {
						short minLat = *((UNALIGNED short *) &bgl[sect0 + sectOffset + rangeTableOffset+1]);
						short maxLat = *((UNALIGNED short *) &bgl[sect0 + sectOffset + rangeTableOffset+3]);
						int vorOffset = *((UNALIGNED int *) &bgl[sect0 + sectOffset + rangeTableOffset+5]);
						// for each station in this range
						while ((bgl[sect0 + vorOffset] == 0x04) || (bgl[sect0 + vorOffset] == 0x05)) {
							char range = *((char *) &bgl[sect0 + vorOffset + 1]);
							short magdev = *((UNALIGNED short *) &bgl[sect0 + vorOffset + 2]);
							int lat = read3bytes(&bgl[sect0 + vorOffset + 5]);
							int lon = read3bytes(&bgl[sect0 + vorOffset + 8]);
							short alt = *((UNALIGNED short *) &bgl[sect0 + vorOffset + 11]);
							char vorID[6];
							strncpy(vorID, &bgl[sect0 + vorOffset + 15], 5);
							vorID[5] = 0x00;
							char vorName[25];
							strncpy(vorName, &bgl[sect0 + vorOffset + 20], 24);
							vorName[24] = 0x00;

							// read additional info for ILS
							if (bgl[sect0 + vorOffset] == 0x05)	{
								short approach = *((UNALIGNED short *) &bgl[sect0 + vorOffset + 13]);
								int gslat = read3bytes(&bgl[sect0 + vorOffset + 44]);
								int gslon = read3bytes(&bgl[sect0 + vorOffset + 47]);
								short gsalt = *((UNALIGNED short *) &bgl[sect0 + vorOffset + 50]);
								addDB(2, (double) gslat * 90.0 / 5000875.0, (double) gslon / (double) 0x1000000 * 360.0, gsalt, freq, vorID, vorName, (double) magdev * 360./ 0x10000, (double) approach * 360. / 0x10000, range * 2048.0 / 1000.0); 				
							} else {
								addDB(3, (double) lat * 90.0 / 5000875.0, (double) lon / (double) 0x1000000 * 360.0, alt, freq, vorID, vorName, (double) magdev * 360./ 0x10000, 0, range * 2048.0 / 1000.0); 				
							}
							count++;
							if (searchingBGLFiles==TRUE) {
								if (UnmapViewOfFile(bgl) == FALSE)
									AfxMessageBox("UnmapViewOfFile failed");
								if (CloseHandle(filemapping) == FALSE)
									AfxMessageBox("Could not close filemapping handle");
								if (CloseHandle(filehandle) == FALSE)
									AfxMessageBox("Could not close file handle");
								return(0);
							}
							if (bgl[sect0 + vorOffset] == 0x05)	
								vorOffset += 54;
							else 
								vorOffset += 44;
						}
						rangeTableOffset += 9;
					}
				}
			}
		}
	}

	// section 16: airports
	if ((sect16 != 0) && (!includeAFDinfo||!searchingBGLFiles))	{
		int rangeTableOffset = 0;
		// for each latitude range
		while (bgl[sect16 + rangeTableOffset] == 0x15) {
			short minLat = *((UNALIGNED short *) &bgl[sect16 + rangeTableOffset + 1]);
			short maxLat = *((UNALIGNED short *) &bgl[sect16 + rangeTableOffset + 3]);
			int areaOffset = *((UNALIGNED int *) &bgl[sect16 + rangeTableOffset + 5]);
			// for each area
			while ((bgl[sect16 + areaOffset] > 0x03) && (bgl[sect16 + areaOffset] < 0x08)) {
				char areaRec = *((char *) &bgl[sect16 + areaOffset + 1]);
				// check to see if its an airport record
				if (bgl[sect16 + areaOffset] == 0x04) {	// inner marker
					int lat = read3bytes(&bgl[sect16 + areaOffset + 2]);
					int lon = read3bytes(&bgl[sect16 + areaOffset + 5]);
					short alt = *((UNALIGNED short *) &bgl[sect16 + areaOffset + 8]);
					addDB(5, (double) lat * 90.0 / 5000875.0, (double) lon / (double) 0x1000000 * 360.0, alt, 0, "", "", 0, 0, 0); 				
				}
				if (bgl[sect16 + areaOffset] == 0x05) {	// middle marker
					int lat = read3bytes(&bgl[sect16 + areaOffset + 2]);
					int lon = read3bytes(&bgl[sect16 + areaOffset + 5]);
					short alt = *((UNALIGNED short *) &bgl[sect16 + areaOffset + 8]);
					addDB(6, (double) lat * 90.0 / 5000875.0, (double) lon / (double) 0x1000000 * 360.0, alt, 0, "", "", 0, 0, 0); 				
				}
				if (bgl[sect16 + areaOffset] == 0x06) {	// outer marker
					int lat = read3bytes(&bgl[sect16 + areaOffset + 2]);
					int lon = read3bytes(&bgl[sect16 + areaOffset + 5]);
					short alt = *((UNALIGNED short *) &bgl[sect16 + areaOffset + 8]);
					addDB(7, (double) lat * 90.0 / 5000875.0, (double) lon / (double) 0x1000000 * 360.0, alt, 0, "", "", 0, 0, 0); 				
				}
				if (bgl[sect16 + areaOffset] == 0x07) {	// airport
					int lat = read3bytes(&bgl[sect16 + areaOffset + 2]);
					int lon = read3bytes(&bgl[sect16 + areaOffset + 5]);
					double dlat = (double) lat * 90.0 / 5000875.0;
					double dlon = (double) lon / (double) 0x1000000 * 360.0;
					short alt = *((UNALIGNED short *) &bgl[sect16 + areaOffset + 9]);
  					char airportName[25];
					strncpy(airportName, &bgl[sect16 + areaOffset + 22], 24);
					airportName[24] = 0x00;
					int len = (int) strlen(airportName) + 1;
					// primary runways?
					int rid1 = bgl[sect16 + areaOffset + 22 + len] & 0x1F;
					int rid2 = bgl[sect16 + areaOffset + 23 + len] & 0x1F;
			
					if (!duplicateAirport(airportName, dlat, dlon)) {
						addDB(4, dlat, dlon, alt, 0, "", airportName, 0, 0, 0); 				
						count++;
						if (searchingBGLFiles==TRUE) {
							if (UnmapViewOfFile(bgl) == FALSE)
								AfxMessageBox("UnmapViewOfFile failed");
							if (CloseHandle(filemapping) == FALSE)
								AfxMessageBox("Could not close filemapping handle");
							if (CloseHandle(filehandle) == FALSE)
								AfxMessageBox("Could not close file handle");
							return(0);
						}
					}
				}
				areaOffset += areaRec;
			}
			rangeTableOffset += 9;
		}
	}

	// section 11: airports & runways (menu items)
	if ((sect11 != 0) && (!includeAFDinfo||!searchingBGLFiles))	{
		int areaOffset = 0;
		// for each area
		while (bgl[sect11 + areaOffset] == 0x01) {
			short areaRec = *((UNALIGNED short *) &bgl[sect11 + areaOffset + 1]);
			int runwayTable = *((UNALIGNED int *) &bgl[sect11 + areaOffset + 3]);
			char* areaName = &bgl[sect11 + areaOffset + 7]; //menu section
			// for each runway in the area
			int runwayOffset = 0;
			while (bgl[sect11 + runwayTable + runwayOffset] == 0x03) {
				short runwayRec = *((UNALIGNED short *) &bgl[sect11 + runwayTable + runwayOffset + 1]);
				char* runwayName = &bgl[sect11 + runwayTable + runwayOffset + 27]; //menu item
				double lat = readDM(&bgl[sect11 + runwayTable + runwayOffset + 3]);
				double lon = readDM(&bgl[sect11 + runwayTable + runwayOffset + 7]);
				double alt = *((UNALIGNED int *) &bgl[sect11 + runwayTable + runwayOffset + 11]) / 256;
				lat *= 90.0 / 5000875.0;
				lon = lon / (double) 0x1000000 * 360;

				// trim runway from name
				CString airport = runwayName;
				int end = airport.Find("- Runway");
				if (end<0)  // can't find "Runway", try other method
					end = airport.ReverseFind('-');
				if (end<0)  // still can't find anything, try Scotland methods
					end = airport.Find("Rwy");	
				if (end<0)  // still can't find anything, try Scotland methods
					end = airport.Find("at");	
				if (end > 0)
					airport = airport.Mid(0, end);
				airport.TrimRight();

				if (!duplicateAirport(airport.GetBuffer(1), lat, lon)) {
					addDB(4, lat, lon, (short) alt, 0, "", airport.GetBuffer(1), 0, 0, 0); 							
					count++;
					airport.ReleaseBuffer();
					if (searchingBGLFiles==TRUE) {
						if (UnmapViewOfFile(bgl) == FALSE)
							AfxMessageBox("UnmapViewOfFile failed");
						if (CloseHandle(filemapping) == FALSE)
							AfxMessageBox("Could not close filemapping handle");
						if (CloseHandle(filehandle) == FALSE)
							AfxMessageBox("Could not close file handle");
						return(0);
					}
				}
				runwayOffset += runwayRec;
			}
			areaOffset += areaRec;
		}
	}

	// section 9: runways
	if ((sect9 != 0) && (!includeAFDinfo||!searchingBGLFiles)) {
		// scan all of section 9 looking for the runway pattern
		for (int i=0; i<(size-sect9); i++) {
			char rwRec = *((char *) &bgl[sect9 + i]);
			if (rwRec == 68) {
				char j1 = *((char *) &bgl[sect9 + i + 1]);
				if (j1 != 0) continue;
				char j2 = *((char *) &bgl[sect9 + i + 18]);
				if (j2 != 0) continue;
				char j3 = *((char *) &bgl[sect9 + i + 19]);
				if (j3 != 0) continue;
				char j4 = *((char *) &bgl[sect9 + i + 29]);
				if (j4 != 0) continue;
				// lat and lon are center of runway
				double lat = readMicro6Lat(&bgl[sect9 + i + 2]);
				lat *= 45.0 / 5000875.0;
				double lon = readMicro6Lon(&bgl[sect9 + i + 8]);
				lon = lon / (double) 0x1000000 * 360.0;	  
				double alt = readMicro4(&bgl[sect9 + i + 14]);
				double course = *((UNALIGNED short *) &bgl[sect9 + i + 20]) * 360.0 / 0x10000;
				if (course < 0.0)
					course += 360.0;
				short length = *((UNALIGNED short *) &bgl[sect9 + i + 22]);
				short width = *((UNALIGNED short *) &bgl[sect9 + i + 24]);
				unsigned char rwID = *((unsigned char *) &bgl[sect9 + i + 27]);
				char rwID2 = ' ';
				if (rwID > 192) {
					rwID -= (unsigned char) 192;
					rwID2 = 'C';
				} else if (rwID > 128) {
					rwID -= (unsigned char) 128;
					rwID2 = 'R';
				} else if (rwID > 64) {
					rwID -=  (unsigned char) 64;
					rwID2 = 'L';
				}
				char surface = *((char *) &bgl[sect9 + i + 30]);

				// the procedure is far from perfect, so throw out "obviously" bad records
				if (rwID == 0) continue; // should be 36 for a valid runway
				if (fabs(lat) > 90.0) continue;
				if (fabs(lon) > 180.0) continue;
				if (alt > 10000) continue;
				if ((length <= 0) || (length > 20000)) continue;
				if (width <= 20) continue;
				if ((surface > 5) || (surface <0)) continue;

				char rwName[6];
				int c;
				c=sprintf(rwName, "%d", (unsigned) rwID);
				sprintf(&rwName[c], "%c", rwID2);
				addDB(8, lat, lon, (int) alt, (double) surface, rwName, "Runway", (double) width, course, (double) length); 

				i+=29;
				count++;
				if (searchingBGLFiles==TRUE) {
					if (UnmapViewOfFile(bgl) == FALSE)
						AfxMessageBox("UnmapViewOfFile failed");
					if (CloseHandle(filemapping) == FALSE)
						AfxMessageBox("Could not close filemapping handle");
					if (CloseHandle(filehandle) == FALSE)
						AfxMessageBox("Could not close file handle");
					return(0);
				}
			}
		}
	}
	// section 10: Dynamic Objects
	if ((sect10 != 0) && (!includeAFDinfo||!searchingBGLFiles)) {
		CString name(filename);
		addDB(11, 0 , 0, 0, 0, "DYNOBJ", "", 0, 0, 0); 				
		name.ReleaseBuffer();
		count++;
		if (searchingBGLFiles==TRUE) {
			if (UnmapViewOfFile(bgl) == FALSE)
				AfxMessageBox("UnmapViewOfFile failed");
			if (CloseHandle(filemapping) == FALSE)
				AfxMessageBox("Could not close filemapping handle");
			if (CloseHandle(filehandle) == FALSE)
				AfxMessageBox("Could not close file handle");
			return(0);
		}
	}

	// section 15: Dynamic Scenery 
	if ((sect15 != 0) && (!includeAFDinfo||!searchingBGLFiles)) {
		CString name(filename);
		addDB(12, 0 , 0, 0, 0, "DYNSCN", "", 0, 0, 0); 				
		name.ReleaseBuffer();
		count++;
		if (searchingBGLFiles==TRUE) {
			if (UnmapViewOfFile(bgl) == FALSE)
				AfxMessageBox("UnmapViewOfFile failed");
			if (CloseHandle(filemapping) == FALSE)
				AfxMessageBox("Could not close filemapping handle");
			if (CloseHandle(filehandle) == FALSE)
				AfxMessageBox("Could not close file handle");
			return(0);
		}
	}

	// section 18: MagVar info (should only be in World3.bgl or Magdec.bgl)
	if (sect18 != 0) {
		magVarTable.SetSize(181*360,1);
		unsigned short deg;
		for (int lat = -90; lat <=90; lat++) {
			for (int lon = 0; lon <360; lon++) {
				deg = *((unsigned short *) &bgl[8 + sect18 + ((lon * 181) + (lat + 90)) * 2]);	
				magVarTable[lat+90 + 181*lon] = deg; 
			}
		}
		// save magvar table here
		FILE *mv;
		POSITION p = GetFirstViewPosition();
		CNavView *v = (CNavView *) GetNextView(p);
		CString fn = v->orgDir + "\\magvar.nav";
		mv = fopen((LPCTSTR) fn, "wb");
		fwrite(&bgl[sect18],2,181*360,mv); 
		fclose(mv);
		magVarTableExists = TRUE;
	}

	// unmap file and close
	if (UnmapViewOfFile(bgl) == FALSE)
		AfxMessageBox("UnmapViewOfFile failed");
	if (CloseHandle(filemapping) == FALSE)
		AfxMessageBox("Could not close filemapping handle");
	if (CloseHandle(filehandle) == FALSE)
		AfxMessageBox("Could not close file handle");
	return(0);
}

void CNavDoc::OnFileAdd() {
	CString path;
	CArray<CString, CString> filelist;
	filelist.SetSize(0,50);

	int count = 0;
	CString name;
	char c;

	// build existing file list

	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	CString fn = v->orgDir + "\\" + listFileName;
	FILE *fp = fopen((LPCTSTR) fn,"r");
	if (fp != NULL) {

		int go = TRUE;
		while (go) {		
			name = "";	// more convoluted to handle directory names with spaces
			while ((c=fgetc(fp)) != '\n') {
				if (c == EOF) {
					go = FALSE;
					break;
				}
				name = name + c;
			}
			if (name == "" && go == FALSE)
				break;
			if (name != "") {
				path = name;
				path.MakeLower();
				count++;
				filelist.Add(path);
			}
		}
		fclose(fp);

		// make sure its not already in the list
		fName.MakeLower();
		for (int i=0; i<count; i++) {
			if (filelist[i] == fName)
				return;;
		}
	}

	// add it
	int ch = 'a';
	fp = fopen((LPCTSTR) fn,"a");
	char *b = fName.GetBuffer(1);	
	fName.ReleaseBuffer();
	for(; (ch != EOF) && (*b != '\0'); b++ )
		ch = putc( *b, fp);
	putc('\n', fp);
	fclose(fp);
	filelist.RemoveAll();
	filelist.FreeExtra();
	compiledSceneryList = FALSE;
}

void CNavDoc::OnUpdateFileAdd(CCmdUI* pCmdUI) {
	pCmdUI->Enable(fName != "");			
}

// load the data from a scenery list
void CNavDoc::OnFileLoad() {
	CWaitCursor wait;	// display wait cursor
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	CString loadfile, name;
	ClearDatabases();
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	FILE *fp;
	v->mag = 1;
	v->mag2 = 1;
	v->plancount= 0;
	CString fn = v->orgDir + "\\" + listFileName;

	if (v->planning) { // close flight plan window, if open
		v->planning = FALSE;
		v->planD->DestroyWindow();	// let it deallocate its own memory
	}

	// check for compiled scenery list
	if (fileNewer(fn + ".c", fn)) {
		if (ReadCompiledScenery(fn + ".c")) {
			ReadAirports();
			v->lastSceneryList = listFileName;
			WritePrivateProfileString("View", "lastSceneryList", listFileName, "nav.ini");
			return;
		}
	}

	fp = fopen((LPCTSTR) fn,"r");
	if (fp == NULL) {
		AfxMessageBox("Could not load scenery list from " + listFileName);
		waypointsExist = FALSE;
		ReadWaypoints();
		Index();
		ReadAirports();
		v->ZoomToFit();
		return;
	}
	
	dbFileIndex = 0;

	char c;
	int go = TRUE;
	while (go) {		
		name = "";	// more convoluted to handle directory names with spaces
		while ((c=fgetc(fp)) != '\n') {
			if (c == EOF) {
				go = FALSE;
				break;
			}
			name = name + c;
		}
		if (name == "" && go == FALSE)
			break;
		if (name != "") {
			fName = name;
 			dbFiles.Add(fName);
			loadfile = "Loading " + fName;
			loadfile.TrimRight();
			main->SetStatusBar(loadfile);
			// take over message processing to simulate threading 
	//		MSG msg;  
	//		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
	//			if (msg.message == WM_QUIT) { 
	//		 		PostQuitMessage(1);
	//		 		return;      
	//			}	 
	//		 	TranslateMessage(&msg);
	//		 	DispatchMessage(&msg);
	//		}
			// continue
			if (0 != readFile((LPCTSTR) name)) // load failed
				break;
			dbFileIndex++;
		}
	}
	waypointsExist = FALSE;
	ReadWaypoints();
	Index();
	ReadAirports();

	v->lastSceneryList = listFileName;
	WritePrivateProfileString("View", "lastSceneryList", listFileName, "nav.ini");
	char buff[200];
	sprintf(buff, "%d files loaded, %d items in database", dbFileIndex, dbCount);
	((CMainFrame *) AfxGetMainWnd())->SetStatusBar(buff);
	fclose(fp);
	SetTitle("Scenery List: " + listFileName);
	fName = "";
	Index();
	v->ZoomToFit();
}

void CNavDoc::addDB(int type, double lat, double lon, int alt, double freq, char *id, char *name, double magdev, double approach, double range, char* filename) {
	dbRec r;
	db.Add(r);

	db[dbCount].type = type;
	db[dbCount].lat = lat;
	db[dbCount].lon = lon;
	db[dbCount].alt = alt;
	db[dbCount].freq = freq;
	strncpy(db[dbCount].id, id, 5);
	db[dbCount].id[5] = '\x00'; 
	strncpy(db[dbCount].name, name, 30);
	db[dbCount].name[30] = '\x00'; 
	db[dbCount].magdev = magdev;
	if (approach < 0)
		approach += 360;
	db[dbCount].approach = approach;
	db[dbCount].range = range;

	if (0 != strlen(filename)) { // make sure filename is in the list 
		int found = FALSE;
		for (int i=0; i<dbFiles.GetSize(); i++ ) {
			if (dbFiles[i] == CString(filename)) {
				found = TRUE;
				break;
			}
		}
		if (!found) {
			dbFiles.Add(CString(filename));
			db[dbCount].file = dbFiles.GetSize()-1;
			dbFileIndex++;
		} else
			db[dbCount].file = i;
		dbCount++;
		return;
	}
	
	db[dbCount].file = dbFileIndex;
	dbCount++;
}

void CNavDoc::formatData(CString& details, int dbindex) {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	char line[50];

	double lat = db[dbindex].lat;
	double lon = db[dbindex].lon;
	char dirx, diry;
	if (lon < 0.) 
		dirx = 'W'; 
	else 
		dirx = 'E';
	if (lat < 0.) 
		diry = 'S'; 
	else 
		diry = 'N';
	int xdeg, xmin, ydeg, ymin;
	double xsec, ysec;
	lat = fabs(lat);
	lon = fabs(lon);

	xdeg = (int) lon;
	xmin = (int) ((lon - xdeg) * 60);
	xsec = (lon - xdeg - xmin / 60.0) * 3600;
	if (xsec >= 59.995) { // round to .01 seconds
		xsec = 0.0;
		xmin += 1;
	}
	if (xmin >= 60) {
		xmin -= 60;
		xdeg += 1;
	}

	ydeg = (int) lat;
	ymin = (int) ((lat - ydeg) * 60);
	ysec = (lat - ydeg - ymin / 60.0) * 3600;
	if (ysec >= 59.995) { // round to .01 seconds
		ysec = 0.0;
		ymin += 1;
	}
	if (ymin >= 60) {
		ymin -= 60;
		ydeg += 1;
	}

	if (1 == db[dbindex].type) {
		sprintf(line, " NDB\r\n");
		details += line;
		sprintf(line, " Id: %s\r\n", db[dbindex].id);
		details += line;
		sprintf(line, " Full name: %s\r\n", db[dbindex].name);
		details += line;
		sprintf(line, " Frequency: %5.1f KHz\r\n", db[dbindex].freq);
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
		if (db[dbindex].range != 0.0) {
			sprintf(line, " Range: %d nm\r\n", (int) (db[dbindex].range/1.852));
			details += line;
		}
	} else if (2 == db[dbindex].type) {
		sprintf(line, " ILS\r\n");
		details += line;
		sprintf(line, " Id: %s\r\n", db[dbindex].id);
		details += line;
		sprintf(line, " Full name: %s\r\n", db[dbindex].name);
		details += line;
		if (airportIndex.GetSize() > 0 ) {
			CString airport;
			double dist = 1e10, d;
	  		for (int i=0; i<airportIndex.GetSize(); i++) {
				d = v->gcDist(db[airportIndex[i]].lat, db[airportIndex[i]].lon, db[dbindex].lat, db[dbindex].lon); 
				if (d < dist) {
					dist = d;
					airport = db[airportIndex[i]].name;
				}
			}
			d = dist/1852;
			if (d > 5)
				sprintf(line, " Closest airport (%.1lf nm away): %s\r\n", d, (LPCTSTR) airport);
			else
				sprintf(line, " Closest airport: %s\r\n", (LPCTSTR) airport);
			details += line;
		}
		sprintf(line, " Frequency: %6.2f MHz\r\n", db[dbindex].freq);
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
		if (db[dbindex].range != 0.0) {
			sprintf(line, " Range: %d nm\r\n", (int) (db[dbindex].range/1.852));
			details += line;
		}
		double magvar = GetMagVar(db[dbindex].lat, db[dbindex].lon);
		if (magvar < 0.0)
			sprintf(line, " Mag Deviation: %.1f degrees (East)\r\n",  magvar);
		else 
			sprintf(line, " Mag Deviation: %.1f degrees (West)\r\n",  magvar);
		details += line;
		int app = (int) (magvar + db[dbindex].approach);
		app = app%360;
		if (app == 0) app = 360;
		sprintf(line, " Approach course: %d degrees magnetic\r\n", app);
		details += line;
	} else if (3 == db[dbindex].type) {
		sprintf(line, " VOR\r\n");
		details += line;
		sprintf(line, " Id: %s\r\n", db[dbindex].id);
		details += line;
		sprintf(line, " Full name: %s\r\n", db[dbindex].name);
		details += line;
		sprintf(line, " Frequency: %6.2f MHz\r\n", db[dbindex].freq);
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
		if (db[dbindex].range != 0.0) {
			sprintf(line, " Range: %d nm\r\n", (int) (db[dbindex].range/1.852));
			details += line;
		}
		if (db[dbindex].magdev < 0.0)
			sprintf(line, " Mag Deviation: %.1f degrees (East)\r\n",  db[dbindex].magdev);
		else 
			sprintf(line, " Mag Deviation: %.1f degrees (West)\r\n",  db[dbindex].magdev);
		details += line;
	} else if (4 == db[dbindex].type) {
		sprintf(line, " Airport\r\n");
		details += line;
		sprintf(line, " Full name: %s\r\n", db[dbindex].name);
		details += line;
		CString name;
		int icaoFound;
		if (icaoFound=lookUpAirport(name, db[dbindex].lat, db[dbindex].lon)) {
			sprintf(line, " Nearest ICAO ID: %s\r\n", name);
			details += line;
		}
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
		int runways[50], r, ils1, ils2;
		double magvar = GetMagVar(db[dbindex].lat, db[dbindex].lon);
		if (r=lookUpRunways(dbindex, runways)) { // show details for this airports runways
			for (int i=0; i<r; i++) {
				ASSERT(8 == db[runways[i]].type);
				dbindex = runways[i];
				sprintf(line, " Runway ID: %s, ", db[dbindex].id);
				details += line;
				int app = (int) (magvar + db[dbindex].approach);
				app = app%360;
				if (app == 0) app = 360;
				int appt = (int) db[dbindex].approach;
				if (appt == 0) appt = 360;
				sprintf(line, "Magnetic Heading: %d (True: %d)", app, appt);
				details += line;
				sprintf(line, "\r\n     Length: %d ft, ", (int) (db[dbindex].range ));
				details += line;
				sprintf(line, "Width: %d ft, ", (int) (db[dbindex].magdev ));
				details += line;
				if (1 == (int) db[dbindex].freq)
					sprintf(line, "Dirt");
				else if (2 == (int) db[dbindex].freq)
					sprintf(line, "Concrete");
				else if (3 == (int) db[dbindex].freq)
					sprintf(line, "Asphalt");
				else if (4 == (int) db[dbindex].freq)
					sprintf(line, "Grass");
				else if (5 == (int) db[dbindex].freq)
					sprintf(line, "Gravel");
				else
					sprintf(line, "Surface type: %d", (int) db[dbindex].freq);
				details += line;
				if (lookUpILS(dbindex, ils1, ils2)) { // show details for this runways ILSs
					sprintf(line, "\r\n");
					details += line;
					if (ils1 != -1) {
						app = (int) (magvar + db[dbindex].approach);
						app = app%360;
						if (app == 0) app = 360;
						if (app < 180.0)
							sprintf(line, "     ILS on heading %d: %.2lf MHz", app, db[ils1].freq);
						else {
							app = (int) (magvar + db[dbindex].approach + 180);
							app = app%360;
	 						if (app == 0) app = 360;
							sprintf(line, "     ILS on heading %d: %.2lf MHz", app, db[ils1].freq);		 
						}
						details += line;
					}
					if (ils2 != -1) {
						int app = (int) (magvar + db[dbindex].approach);
						app = app%360;
						if (app == 0) app = 360;
						if (app >= 180.0)
							sprintf(line, "     ILS on heading %d: %.2lf MHz", app, db[ils2].freq);
						else {
							int app = (int) (magvar + db[dbindex].approach + 180);
							app = app%360;
							if (app == 0) app = 360;
							sprintf(line, "     ILS on heading %d: %.2lf MHz", app, db[ils2].freq);		 
						}
						details += line;
					}
				}
				sprintf(line, "\r\n");
				details += line;
			}
		}
		if (icaoFound) { // look for DPs and STARs
			DpStarAirport *dpDpAirport = dpStar.lookupAirport(name);
			if (dpDpAirport != NULL) {
				DpStarRoute* dpStarRoute;
				dpStarRoute = dpDpAirport->firstDP();
				while (dpStarRoute != NULL) {
					details += " DP: " + dpStarRoute->name + "\r\n";
					dpStarRoute = dpDpAirport->nextDP();
				}
				dpStarRoute = dpDpAirport->firstSTAR();
				while (dpStarRoute != NULL) {
					details += " STAR: " + dpStarRoute->name + "\r\n";
					dpStarRoute = dpDpAirport->nextSTAR();
				}
			}
		}
	} else if (5 == db[dbindex].type) {
		sprintf(line, " Inner marker\r\n");
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
	} else if (6 == db[dbindex].type) {
		sprintf(line, " Middle marker\r\n");
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
	} else if (7 == db[dbindex].type) {
		sprintf(line, " Outer marker\r\n");
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
	} else if (8 == db[dbindex].type) {
		sprintf(line, " Runway\r\n");
		details += line;
		sprintf(line, " ID: %s\r\n", db[dbindex].id);
		details += line;
		if (airportIndex.GetSize() > 0 ) {
			CString airport;
			double dist = 1e10, d;
	  		for (int i=0; i<airportIndex.GetSize(); i++) {
				d = v->gcDist(db[airportIndex[i]].lat, db[airportIndex[i]].lon, db[dbindex].lat, db[dbindex].lon); 
				if (d < dist) {
					dist = d;
					airport = db[airportIndex[i]].name;
				}
			}
			d = dist/1852;
			if (d > 5)
				sprintf(line, " Closest airport (%.1lf nm away): %s\r\n", d, (LPCTSTR) airport);
			else
				sprintf(line, " Closest airport: %s\r\n", (LPCTSTR) airport);
			details += line;
		}
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		sprintf(line, " Altitude: %d ft\r\n", (int) (db[dbindex].alt / .3048));
		details += line;
		double magvar = GetMagVar(db[dbindex].lat, db[dbindex].lon);
		int app = (int) (magvar + db[dbindex].approach);
		app = app%360;
		if (app == 0) app = 360;
		int appt = (int) db[dbindex].approach;
		if (appt == 0) appt = 360;
		sprintf(line, " Magnetic Heading: %d (True: %d), ", app, appt);
		details += line;
		sprintf(line, " Length: %d ft\r\n", (int) (db[dbindex].range ));
		details += line;
		sprintf(line, " Width: %d ft\r\n", (int) (db[dbindex].magdev ));
		details += line;
		if (1 == (int) db[dbindex].freq)
			sprintf(line, " Surface type: Dirt \r\n");
		else if (2 == (int) db[dbindex].freq)
			sprintf(line, " Surface type: Concrete \r\n");
		else if (3 == (int) db[dbindex].freq)
			sprintf(line, " Surface type: Asphalt \r\n");
		else if (4 == (int) db[dbindex].freq)
			sprintf(line, " Surface type: Grass \r\n");
		else if (5 == (int) db[dbindex].freq)
			sprintf(line, " Surface type: Gravel \r\n");
		else
			sprintf(line, " Surface type: %d \r\n", (int) db[dbindex].freq);
		details += line;
		int ils1, ils2;
		if (lookUpILS(dbindex, ils1, ils2)) { // show details for this runways ILSs
			if (ils1 != -1) {
				app = (int) (magvar + db[dbindex].approach);
				app = app%360;
				if (app == 0) app = 360;
				if (app < 180.0)
					sprintf(line, " ILS on heading %d: %.2lf MHz\r\n", app, db[ils1].freq);
				else {
					app = (int) (magvar + db[dbindex].approach + 180);
					app = app%360;
	 				if (app == 0) app = 360;
					sprintf(line, " ILS on heading %d: %.2lf MHz\r\n", app, db[ils1].freq);		 
				}
				details += line;
			}
			if (ils2 != -1) {
				int app = (int) (magvar + db[dbindex].approach);
				app = app%360;
				if (app == 0) app = 360;
				if (app >= 180.0)
					sprintf(line, " ILS on heading %d: %.2lf MHz\r\n", app, db[ils2].freq);
				else {
					int app = (int) (magvar + db[dbindex].approach + 180);
					app = app%360;
					if (app == 0) app = 360;
					sprintf(line, " ILS on heading %d: %.2lf MHz\r\n", app, db[ils2].freq);		 
				}
				details += line;
			}
		}
	} else if (9 == db[dbindex].type) {
		sprintf(line, " GPS fix (user defined waypoint)\r\n");
		details += line;
		sprintf(line, " ID: %s\r\n", db[dbindex].id);
		details += line;
		sprintf(line, " Name: %s\r\n", db[dbindex].name);
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		if (db[dbindex].magdev < 0.0)
			sprintf(line, " Mag Deviation: %.1f degrees (East)\r\n",  db[dbindex].magdev);
		else 
			sprintf(line, " Mag Deviation: %.1f degrees (West)\r\n",  db[dbindex].magdev);
		details += line;
	} else if (10 == db[dbindex].type) {
		sprintf(line, " ATIS station\r\n");
		details += line;
		sprintf(line, " Frequency: %6.2f MHz\r\n", db[dbindex].freq);
		details += line;
		if (airportIndex.GetSize() > 0 ) {
			CString airport;
			double dist = 1e10, d;
	  		for (int i=0; i<airportIndex.GetSize(); i++) {
				d = v->gcDist(db[airportIndex[i]].lat, db[airportIndex[i]].lon, db[dbindex].lat, db[dbindex].lon); 
				if (d < dist) {
					dist = d;
					airport = db[airportIndex[i]].name;
				}
			}
			d = dist/1852;
			if (d > 5)
				sprintf(line, " Closest airport (%.1lf nm away): %s\r\n", d, (LPCTSTR) airport);
			else
				sprintf(line, " Closest airport: %s\r\n", (LPCTSTR) airport);
			details += line;
		}
		sprintf(line, " Name: %s\r\n", db[dbindex].name);
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
		if (db[dbindex].range != 0.0) {
			unsigned int allRunways = (unsigned int)db[dbindex].range;
			char pr4 = (allRunways & 0x3F000000) >> 24;
			char pr3 = (allRunways & 0x003F0000) >> 16;
			char pr2 = (allRunways & 0x00003F00) >> 8;
			char pr1 = (allRunways & 0x0000003F);
			char pr4c = (allRunways & 0xc0000000) >> 30;
			char pr3c = (allRunways & 0x00c00000) >> 22;
			char pr2c = (allRunways & 0x0000c000) >> 14;
			char pr1c = (allRunways & 0x000000c0) >> 6;
			pr1c==0?pr1c=' ':(pr1c==1?pr1c='L':(pr1c==2?pr1c='R':(pr1c='C'))); // ah, the joys of C
			pr2c==0?pr2c=' ':(pr2c==1?pr2c='L':(pr2c==2?pr2c='R':(pr2c='C')));
			pr3c==0?pr3c=' ':(pr3c==1?pr3c='L':(pr3c==2?pr3c='R':(pr3c='C')));
			pr4c==0?pr4c=' ':(pr4c==1?pr4c='L':(pr4c==2?pr4c='R':(pr4c='C')));
			sprintf(line, " Preferred runways:\r\n");
			details += line;
			sprintf(line, "   North wind: %d%c\r\n", pr1, pr1c);
			details += line;
			sprintf(line, "   East wind:  %d%c\r\n", pr2, pr2c);
			details += line;
			sprintf(line, "   South wind: %d%c\r\n", pr3, pr3c);
			details += line;
			sprintf(line, "   West wind:  %d%c\r\n", pr4, pr4c);
			details += line;
		}
	} else if (13 == db[dbindex].type) {
		sprintf(line, " Waypoint\r\n");
		details += line;
		sprintf(line, " Name: %s\r\n", db[dbindex].name);
		details += line;
		sprintf(line, " Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
	} else if (14 == db[dbindex].type) {
		sprintf(line, " Route segment\r\n");
		details += line;
		sprintf(line, " Name: %s\r\n", db[dbindex].name);
		details += line;
		sprintf(line, " Start Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Start Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;

		lat = db[dbindex].approach;
		lon = db[dbindex].range;
		if (lon < 0.) 
			dirx = 'W'; 
		else 
			dirx = 'E';
		if (lat < 0.) 
			diry = 'S'; 
		else 
			diry = 'N';
		lat = fabs(lat);
		lon = fabs(lon);
		xdeg = (int) lon;
		xmin = (int) ((lon - xdeg) * 60);
		xsec = (lon - xdeg - xmin / 60.0) * 3600;
		if (xsec >= 59.995) { // round to .01 seconds
			xsec = 0.0;
			xmin += 1;
		}
		if (xmin >= 60) {
			xmin -= 60;
			xdeg += 1;
		}
		ydeg = (int) lat;
		ymin = (int) ((lat - ydeg) * 60);
		ysec = (lat - ydeg - ymin / 60.0) * 3600;
		if (ysec >= 59.995) { // round to .01 seconds
			ysec = 0.0;
			ymin += 1;
		}
		if (ymin >= 60) {
			ymin -= 60;
			ydeg += 1;
		}
		sprintf(line, " Stop Latitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lat, diry, ydeg, ymin, ysec, diry);
		details += line;		 
		sprintf(line, " Stop Longitude: % 8.6f %c (%d %d' %4.2f\" %c)\r\n", lon, dirx, xdeg, xmin, xsec, dirx);;
		details += line;
	}
	if (9 != db[dbindex].type) {
		int fileindex=db[dbindex].file;
		details += " File: " + dbFiles[fileindex];
	}
}

void CNavDoc::OnFileSearch() {
	CArray<CString, CString> filelist;	 
	filelist.SetSize(0,50);

	searchingBGLFiles=TRUE;
	CString name;
	int count = 0;
	int startCount;
	SerBgl d;

	d.m_recurse = TRUE;
	d.m_skip = TRUE;
	d.m_drive = "c:\\";
	int saveIncludeAFDinfo = includeAFDinfo;
	d.m_useAFD = 0; // always default to off
	again:;
	if (d.DoModal() == IDOK) {
		// check for a valid drive
		int result;
		CString path;
		path = d.m_drive;
		result = _access((LPCTSTR) path, 0);
		if (result == -1L) {
			char buf[350];
			sprintf(buf, "%s is not a valid directory", d.m_drive);
			AfxMessageBox(buf);
			goto again;
		}
		includeAFDinfo = d.m_useAFD;

		// build existing file list
		POSITION p = GetFirstViewPosition();
		CNavView *v = (CNavView *) GetNextView(p);
		CString fn = v->orgDir + "\\" + listFileName;
		FILE *fp = fopen((LPCTSTR) fn,"r");
		if (fp != NULL) {
			char c;
			int go = TRUE;
			while (go) {		
				name = "";	// more convoluted to handle directory names with spaces
				while ((c=fgetc(fp)) != '\n') {
					if (c == EOF) {
						go = FALSE;
						break;
					}
					name = name + c;
				}
				if (name == "" && go == FALSE)
					break;
				if (name != "") {
					path = name;
					path.MakeLower();
					filelist.Add(path); 
					count++;
				}
			}
			fclose(fp);
		}

		CWaitCursor wait;	// display wait cursor
		startCount = count;
		findFile(d.m_drive, filelist, count, d.m_recurse, d.m_skip);

		// write new list to file
		fp = fopen((LPCTSTR) fn,"w");
		for(int i=0; i<filelist.GetSize(); i++) 
			fprintf(fp, "%s\n", filelist[i]);
		fclose(fp);

		CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
		OnNewDocument(); // start off empty to clear loaded file
		SetTitle("Untitled");
		main->SetStatusBar("Ready");

		char buffer[50];
		sprintf(buffer, "%d new files found and added to scenery list %s", count-startCount, (LPCTSTR) listFileName);
		AfxMessageBox(buffer);
	}			
	searchingBGLFiles=FALSE;
	includeAFDinfo = saveIncludeAFDinfo;
	filelist.RemoveAll();
	filelist.FreeExtra();
}

// recursively search for bgl files
void CNavDoc::findFile(CString path, CArray<CString, CString> &exist, int &count, int recurse, int skip) {
	CWaitCursor wait;	// display wait cursor
	if (path[path.GetLength()-1] == '\\')	// remove trailing backslash
		path = path.Left(path.GetLength()-1);

	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	char buffer[300];
	sprintf(buffer, "Searching %s", path);
	main->SetStatusBar(buffer);

	// take over message processing to simulate threading 
//	MSG msg;  
//	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
//		if (msg.message == WM_QUIT) { 
//	 		PostQuitMessage(1);
//			return;      
//		}	 
//	 	TranslateMessage(&msg);
//	 	DispatchMessage(&msg);
//	}

	int result, handle;
	_finddata_t fd;
	CString startPath, subPath;
	startPath = path + "\\*.*";
	result = handle = _findfirst((LPCTSTR) startPath, &fd);
	while ((result != -1L) && recurse) { // look for directories
		if (fd.attrib &	_A_SUBDIR) {
			if ((strcmp(fd.name, ".") != 0) && (strcmp(fd.name, "..") != 0)) {
				subPath = path + "\\" + fd.name;
				findFile(subPath, exist, count, recurse, skip);
			}
		}
		result = _findnext(handle, &fd);
	}
	_findclose(handle);
	// find files now
	startPath = path + "\\*.bgl";
	result = handle = _findfirst((LPCTSTR) startPath, &fd);
	while (result != -1) { 
		subPath = path + "\\" + fd.name;
		subPath.MakeLower();
		// make sure its not already in the list
		int found = FALSE;
		for (int i=0; i<count; i++) {
			if (exist[i] == subPath)
				found = TRUE;
		}
		if (found == FALSE) {
			if (skip) {
				// load the file and see if it adds anything to the database
				dbCount = 0;
				db.RemoveAll();
 				db.FreeExtra();
				db.SetSize(0,5000);
				readFile(subPath);
				SetTitle(subPath);
				if (dbCount > 0) { // true if it added stuff
					exist.Add(subPath);
					count++;
				}
			} else {
				exist.Add(subPath);
				count++;
			}
		}		
		result = _findnext(handle, &fd);
	}
	_findclose(handle);
}

void CNavDoc::OnUpdateFileExport(CCmdUI* pCmdUI) {
	pCmdUI->Enable(dbCount > 0);
}

void CNavDoc::OnFileExport() {
	((CMainFrame *) AfxGetMainWnd())->SetStatusBar("Exporting (can take a long time)...");
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	CString fn = v->orgDir;
	CExport d;
	d.m_center = "";
	d.m_state  = "";
	if (d.DoModal() == IDOK) {
		CWaitCursor wait;
		if (d.m_center == "")
			d.m_center = "            ";
		if (d.m_state == "")
			d.m_state = "    ";
		FILE *fpn = fopen(fn+"nav.n", "w");
		FILE *fpa = fopen(fn+"nav.a", "w");
		FILE *fpr = fopen(fn+"nav.r", "w");
		if (d.m_headings != FALSE) { // useful column headings for debugging
			fprintf(fpn,"         1         2         3         4         5         6         7         8         9         0         1         2\n");				
			fprintf(fpn,"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123\n");	
			fprintf(fpn,"ID    Name                                 St  Latitude        Longitude        MagV Elev    ARTCC          Freq    Type\n");				
			fprintf(fpa,"         1         2         3         4         5         6         7         8         9         0         1         2\n");				
			fprintf(fpa,"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123\n");	
			fprintf(fpa,"ID    Name                                 St  Latitude        Longitude        MagV Elev    ARTCC         UNICOM   CTAF\n");				
			fprintf(fpr,"         1         2         3         4         5         6         7         8\n");				
			fprintf(fpr,"12345678901234567890123456789012345678901234567890123456789012345678901234567890\n");	
			fprintf(fpr,"ID    Rwy Nos    Len   Width R1# R1h R1ILS-t   R1ILS-f R2# R2h R2-ILS-t  R2ILS-f\n");				
		}
		char line[150];
		int totalILS;
		CArray<dbRec, dbRec> runways;
		runways.SetSize(0,200);
		CArray<dbRec, dbRec> ils;
		ils.SetSize(0,200);
		CArray<dbRec, dbRec> airports;
		airports.SetSize(0,200);
		CArray<CString, CString> AID;
		AID.SetSize(0,200);
		MSG msg; 
		int maxIls;

		ExportProgress *prog;
		prog = new ExportProgress();
		prog->Create(IDD_EXPORTPROGRESS);
		prog->ShowWindow(SW_SHOW);

		for (int i=0; i<dbCount; i++) {
			if (prog->cancelled != FALSE)
				goto cancelled;
			prog->m_navaids.SetPos((100*i)/dbCount);

			if (v->flatWorld) {  // limit export to current window (not implemented for sphere world)
	 			if (db[i].lon < v->left) continue;	 
				if (db[i].lon > v->right) continue;
				if (db[i].lat < v->bottom) continue;
				if (db[i].lat > v->top) continue;
			}

			// take over message processing to simulate threading 
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
				if (msg.message == WM_QUIT) { 
		 			PostQuitMessage(1);
		 			return;      
				}	 
		 		TranslateMessage(&msg);
		 		DispatchMessage(&msg);
			}
			// continue
			memset(line, ' ', 150);
			int type = db[i].type;
			double lat = fabs(db[i].lat);
			double lon = fabs(db[i].lon);
			int latd = (int) (lat);
			int latm = (int) ((lat-latd) * 60);
			double lats = ((lat-latd)*60-latm)*60;
			int lond = (int) (lon);
			int lonm = (int) ((lon-lond) * 60);
			double lons = ((lon-lond)*60-lonm)*60;
			char dirx, diry;
			if (db[i].lon < 0.) 
				dirx = 'W'; 
			else 
				dirx = 'E';
			if (db[i].lat < 0.) 
				diry = 'S'; 
			else 
				diry = 'N';
			if ((type == 1) || (type == 3)) {  // NDB, VOR
				sprintf(line,    "%s      ", db[i].id);
				sprintf(line+6,  "%s                                        ", db[i].name);
				sprintf(line+43, "%s   ", d.m_state);
				sprintf(line+47, "%c-%02d-%02d-%06.3lf    ", diry, latd, latm, lats);
				sprintf(line+63, "%c-%03d-%02d-%06.3lf    ", dirx, lond, lonm, lons);
				double mag = db[i].magdev;
				if (type != 3)  
					mag = GetMagVar(db[i].lat, db[i].lon);
				char dirm = 'E';
				if (mag > 0)
					dirm = 'W';
				sprintf(line+80, "%c%02d    ", dirm, abs((int) mag));
				if (db[i].alt != -9999)
					sprintf(line+85, "%06d    "  , (int) (db[i].alt / .3048));
				else  // -9999 is apparently used to signify an unknown value, don't convert it to feet
					sprintf(line+85, "%06d    "  , (int) (db[i].alt));
				sprintf(line+93, "%s               "  , d.m_center);
				sprintf(line+108,"%7.3lf   "  , db[i].freq);
				if (type == 1) // NDB
					sprintf(line+116,"NDB    "  , db[i].freq);
				else // can't tell VOR type, but an entry is required
					sprintf(line+116,"VOR/UNK"  , db[i].freq);
				line[123] = '\0';
				fprintf(fpn, "%s\n", line);
			} else if (type == 4) {	 // Airport	(just add to database this pass)
				airports.Add(db[i]);
				// generate ID if it doesn't exist
				CString ID = db[i].id;
				if (ID == "")
					ID = GenUniqueId();
				AID.Add(ID);
			} else if (type == 8) {  // Runway (just add to database this pass)
				runways.Add(db[i]);
			} else if (type == 2) {  // ILS    (just add to database this pass)
				ils.Add(db[i]);
			}
		}
		totalILS = ils.GetSize();
		// runway pass
	  	for (i=0; i<runways.GetSize(); i++) {
			if (prog->cancelled != FALSE)
				goto cancelled;
			prog->m_runways.SetPos((100*i)/runways.GetSize());
			// take over message processing to simulate threading 
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
				if (msg.message == WM_QUIT) { 
		 			PostQuitMessage(1);
		 			return;      
				}	 
		 		TranslateMessage(&msg);
		 		DispatchMessage(&msg);
			}
			// continue
			memset(line, ' ', 150);
			CString ID;
			// scan existing airports for one within 3 nm of this runway
			int found = FALSE;
			for (int j=0; j<airports.GetSize(); j++) {
				double dist = v->gcDist(airports[j].lat, airports[j].lon, runways[i].lat, runways[i].lon);
				if (dist/1852.0 < 3.0) {
					found = TRUE;
					ID = AID[j];  // use ID from found airport
					break;
				}
			}
			if (found == FALSE) { // else generate new ID and new airport
				ID = GenUniqueId();
				AID.Add(ID);
				dbRec a;
				a.type = 4;
				a.lat = runways[i].lat;
				a.lon = runways[i].lon;
				a.alt = runways[i].alt;
				strncpy(a.id, (LPCTSTR) ID, 6);
				CString name = "Name Unknown";
				strncpy(a.name, (LPCTSTR) name, 25);
				airports.Add(a);
			}
			sprintf(line,    "%s      ", ID);
			int rwID1;
			char rwID2 = ' ';
			sscanf(runways[i].id, "%d", &rwID1);
			if (((runways[i].id)[1] == 'L') || ((runways[i].id)[2] == 'L')) rwID2 = 'L';
			if (((runways[i].id)[1] == 'R') || ((runways[i].id)[2] == 'R')) rwID2 = 'R';
			if (((runways[i].id)[1] == 'C') || ((runways[i].id)[2] == 'C')) rwID2 = 'C';
			int rwApp = (int) runways[i].approach;
			if(rwID1 > 18) {
				rwApp = (rwApp + 180) % 360;
				if (rwApp == 0) rwApp = 360;
				rwID1 -= 18;
				if (rwID2 == 'L') rwID2 = 'R';
				else if (rwID2 == 'R') rwID2 = 'L';
			}
			int rwAppo = (rwApp + 180) % 360;
			if (rwAppo == 0) rwAppo = 360;
			int rwID1o = rwID1 + 18;
			char rwID2o = ' ';
			if (rwID2 == 'L') rwID2o = 'R';
			else if (rwID2 == 'R') rwID2o = 'L';
			else if (rwID2 == 'C') rwID2o = 'C';
			if (rwID2 == ' ')
				sprintf(line+6 ,    "%02d/%02d              ", rwID1, rwID1o);
			else
				sprintf(line+6 ,    "%02d%c/%02d%c              ", rwID1, rwID2, rwID1o, rwID2o);

			// scan existing ils for one within 0.6 nm of each end of this runway
			int f1found, f2found;
			f1found = f2found = FALSE;
			double f1, f2;
			int maxIls = ils.GetSize();
			for (j=0; j<maxIls; j++) {
				double dist = v->gcDist(ils[j].lat, ils[j].lon, runways[i].lat, runways[i].lon)/1852.0; // in nm
				if (dist < 3.0) {  // first just check to see if its close
					double d2 = (runways[i].range/2) * .3046 / 1852.; // half runway length in nm
					if (fabs(dist - d2) < 0.6) { // ils is in the proper ring around the runway center
						if (fabs(ils[j].approach - v->bearing) <= 5.0) { // heading is within 5 degrees
							double head = (ils[j].approach - rwApp + 360.0);
							if (head > 360.0)
								head -= 360.0;
							if ((head <= 5.0) || (head >= 355.0)) { // its the first heading
								f1found = TRUE;
								f1 = ils[j].freq;
								ils.RemoveAt(j); // remove this ils so it will not be used again
								j--;
								maxIls--;
							} else if ((head <= 185.0) || (head >= 175.0)) { // its the second heading								  // its the second heading
								f2found = TRUE;
								f2 = ils[j].freq;
								ils.RemoveAt(j); // remove this ils so it will not be used again
								j--;
								maxIls--;
							}
							if (f1found && f2found)
								break;
						}
					}
				}
			}
			
			sprintf(line+17,    "%05d      ", (int) runways[i].range);  // really length
			sprintf(line+23,    "%05d      ", (int) runways[i].magdev); // really width
			sprintf(line+29,    "%02d%c    ", rwID1, rwID2); 
			sprintf(line+33,    "%03d                           ", rwApp); 
			if (f1found) {
				// can't tell type, but an entry is required
				sprintf(line+37,"ILS/Unk         "); 
				sprintf(line+47,"%7.3lf          ", f1); 
			}
			sprintf(line+55,    "%02d%c          ", rwID1o, rwID2o); 
			sprintf(line+59,    "%03d                           ", rwAppo); 
  			if (f2found) {
				// can't tell type, but an entry is required
				sprintf(line+63,"ILS/Unk         "); 
				sprintf(line+73,"%7.3lf           ", f2); 
			}
			line[80] = '\0';
			fprintf(fpr, "%s\n", line);
		}
		// airport pass
	  	for (i=0; i<airports.GetSize(); i++) {
			if (prog->cancelled != FALSE)
				goto cancelled;
			prog->m_airports.SetPos((100*i)/airports.GetSize());
			// take over message processing to simulate threading 
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
				if (msg.message == WM_QUIT) { 
		 			PostQuitMessage(1);
		 			return;      
				}	 
		 		TranslateMessage(&msg);
		 		DispatchMessage(&msg);
			}
			// continue
			memset(line, ' ', 150);
			int type = airports[i].type;
			double lat = fabs(airports[i].lat);
			double lon = fabs(airports[i].lon);
			int latd = (int) (lat);
			int latm = (int) ((lat-latd) * 60);
			double lats = ((lat-latd)*60-latm)*60;
			int lond = (int) (lon);
			int lonm = (int) ((lon-lond) * 60);
			double lons = ((lon-lond)*60-lonm)*60;
			char dirx, diry;
			if (airports[i].lon < 0.) 
				dirx = 'W'; 
			else 
				dirx = 'E';
			if (airports[i].lat < 0.) 
				diry = 'S'; 
			else 
				diry = 'N';
			CString ID = AID[i];
			sprintf(line,    "%s      ", ID);
			sprintf(line+6,  "%s                                        ", airports[i].name);
			sprintf(line+43, "%s   ", d.m_state);
			sprintf(line+47, "%c-%02d-%02d-%06.3lf    ", diry, latd, latm, lats);
			sprintf(line+63, "%c-%03d-%02d-%06.3lf    ", dirx, lond, lonm, lons);
			double mag = airports[i].magdev;
			mag = GetMagVar(airports[i].lat, airports[i].lon);
			char dirm = 'E';
			if (mag > 0)
				dirm = 'W';
			sprintf(line+80, "%c%02d    ", dirm, abs((int) mag));
			if (airports[i].alt != -9999)
				sprintf(line+85, "%06d    "  , (int) (airports[i].alt / .3048));
			else  // -9999 is apparently used to signify an unknown value, don't convert it to feet
				sprintf(line+85, "%06d    "  , (int) (airports[i].alt));
			sprintf(line+93, "%s               "  , d.m_center);
			// INICOM and CTAF information would go here if it was available
			line[123] = '\0';
			fprintf(fpa, "%s\n", line);
		}
		maxIls = ils.GetSize();
		if (maxIls > 0) { // some ils were not accounted for, write to error file
			char line[120];
			FILE *fpe = fopen(fn+"nav.e", "w");
			fprintf(fpe, "%d of %d ILSs were not accounted for:\n", maxIls, totalILS);
			for (int j=0; j<maxIls; j++) {
				CString details;
				sprintf(line, " Id: %s, ", ils[j].id);
				details += line;
				sprintf(line, " Full name: %s, ", ils[j].name);
				details += line;
				sprintf(line, " Freq: %6.2f, ", ils[j].freq);
				details += line;
				sprintf(line, " Lat: % 8.4f, ", ils[j].lat);
				details += line;		 
				sprintf(line, " Lon: % 8.4f, ", ils[j].lon);;
				details += line;
				sprintf(line, " Approach: %d\n", (int) ils[j].approach);
				details += line;
				fprintf(fpe, "%s", details);
			}
			fclose(fpe);
		} else 
			remove("nav.e"); // delete old file if present
cancelled:
		delete prog;
		fclose(fpn);
		fclose(fpa);
		fclose(fpr);
		runways.RemoveAll();
		runways.FreeExtra();
		ils.RemoveAll();
		ils.FreeExtra();
		airports.RemoveAll();
		airports.FreeExtra();
		AID.RemoveAll();
		AID.FreeExtra();
	}
	((CMainFrame *) AfxGetMainWnd())->SetStatusBar("Ready");
}

CString CNavDoc::GenUniqueId() { // generate a unique Airport ID counting backward from ZZZ
	int i3 = idcounter % 26;
	int i2 = idcounter/26 % 26;
	int i1 = idcounter/(26*26) % 26;
	idcounter++;
	return(CString('Z' - i1)+CString('Z' - i2)+CString('Z' - i3));
}

CString CNavDoc::GenUniqueNumber() {
	numbercounter++;
	char buf[10];
	sprintf(buf, "%d", numbercounter);
	CString t(buf); 
	return(t);
}
 
int CNavDoc::OnMapInsertgpsfix(double lat, double lon, CString name, CString id) {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	char buf[27];

	if (id == "")
		id = GenUniqueNumber();

	if (name == "") { // generate DMS name
		char dirx, diry;
		if (lon < 0.) 
			dirx = 'W'; 
		else 
			dirx = 'E';
		if (lat < 0.) 
			diry = 'S'; 
		else 
			diry = 'N';
		int xdeg, xmin, ydeg, ymin;
		double xsec, ysec, gpslat, gpslon;
		gpslat = fabs(lat);
		gpslon = fabs(lon);
		xdeg = (int) gpslon;
		xmin = (int) ((gpslon - xdeg) * 60);
		xsec = (gpslon - xdeg - xmin / 60.0) * 3600;
		if (xsec >= 59.995) { // round to .01 seconds
			xsec = 0.0;
			xmin += 1;
		}
		if (xmin >= 60) {
			xmin -= 60;
			xdeg += 1;
		}
		ydeg = (int) gpslat;
		ymin = (int) ((gpslat - ydeg) * 60);
		ysec = (gpslat - ydeg - ymin / 60.0) * 3600;
		if (ysec >= 59.995) { // round to .01 seconds
			ysec = 0.0;
			ymin += 1;
		}
		if (ymin >= 60) {
			ymin -= 60;
			ydeg += 1;
		}

		sprintf(buf, "%d %d'%2.0f\"%c, %d %d'%2.0f\"%c", ydeg, ymin, ysec, diry, xdeg, xmin, xsec, dirx);
		name = buf;
	}

	double magdev = GetMagVar(lat, lon);
	addDB(9, lat, lon, 0, 0, id.GetBuffer(1), name.GetBuffer(1), magdev, 0, 0);
	id.ReleaseBuffer();
	name.ReleaseBuffer();
	gpsIndex.Add(dbCount - 1);
	return(dbCount - 1);
}

void CNavDoc::OnMapInsertgpsfix() {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	double lat, lon;
	if (v->flatWorld) {
		lat = (v->top + v->bottom)/2.0;	
		lon = (v->left + v->right)/2.0;
	} else {
		lat = v->centerLat;	
		lon = v->centerLon;
	}

	CString id = GenUniqueNumber();
	char dirx, diry;// generate DMS name
	if (lon < 0.) 
		dirx = 'W'; 
	else 
		dirx = 'E';
	if (lat < 0.) 
		diry = 'S'; 
	else 
		diry = 'N';
	int xdeg, xmin, ydeg, ymin;
	double xsec, ysec, gpslat, gpslon;
	char buf[27];
	gpslat = fabs(lat);
	gpslon = fabs(lon);
	xdeg = (int) gpslon;
	xmin = (int) ((gpslon - xdeg) * 60);
	xsec = (gpslon - xdeg - xmin / 60.0) * 3600;
	if (xsec >= 59.995) { // round to .01 seconds
		xsec = 0.0;
		xmin += 1;
	}
	if (xmin >= 60) {
		xmin -= 60;
		xdeg += 1;
	}

	ydeg = (int) gpslat;
	ymin = (int) ((gpslat - ydeg) * 60);
	ysec = (gpslat - ydeg - ymin / 60.0) * 3600;
	if (ysec >= 59.995) { // round to .01 seconds
		ysec = 0.0;
		ymin += 1;
	}
	if (ymin >= 60) {
		ymin -= 60;
		ydeg += 1;
	}

	sprintf(buf, "%d %d'%2.0f\"%c, %d %d'%2.0f\"%c", ydeg, ymin, ysec, diry, xdeg, xmin, xsec, dirx);
	CString name = buf;
	
	InsertGPSDialog d;
	d.lat = lat;
	d.lon = lon;
	d.m_name = name;
	d.m_id = id;
	d.DoModal();
}

// return index of nearest navaid, -1 if none
int CNavDoc::findNearestNavaid(double lat, double lon, int type) {
	int found = -1;

	double startDist = 10001750.0 * 2;	// halfway around Earth	(maximum distance)
	double newdist;
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	for (int k=0; k<dbCount; k++) {
		if (db[k].type != type) 
			continue;
/*		// take over message processing to simulate threading 
		MSG msg; 
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if (msg.message == WM_QUIT) { 
				PostQuitMessage(1);
				return(-1);      
			}	 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// continue
*/		newdist = v->gcDist(lat, lon, db[k].lat, db[k].lon);
		if (newdist < startDist) {
			startDist = newdist;
			found = k;
		}
	} 
	return found;
}

int CNavDoc::findNearestInlineNavaid(double slat, double slon, double elat, double elon, double startDist, int type) {
	double newdist, newHeading, headingDiff, headingDiff2;
	int found = -1;
	MSG msg; 
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);

	v->gcDist(slat, slon, elat, elon);
	double desiredHeading = v->bearing;

	for (int k=0; k<dbCount; k++) {
		if (db[k].type != type) goto next;
		// take over message processing to simulate threading 
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if (msg.message == WM_QUIT) { 
				PostQuitMessage(1);
				return(-1);      
			}	 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		newdist = v->gcDist(db[k].lat, db[k].lon, elat, elon);
		newHeading = v->bearing;
		headingDiff = fabs(newHeading-desiredHeading);
		if (headingDiff>180)
			headingDiff = 360 - headingDiff;

		v->gcDist(slat, slon, db[k].lat, db[k].lon);
		headingDiff2 = fabs(v->bearing-desiredHeading);
		if (headingDiff2>180)
			headingDiff2 = 360 - headingDiff2;

		newdist *= 1.0+(headingDiff+2*headingDiff2)*0.02 ; // scale distance up if heading is bad
		if (newdist < startDist) {
			startDist = newdist;
			found = k;
		}
		next:;
	} 
	return found;
}

double CNavDoc::GetMagVar(double lat, double lon) { 
	// use data from MagVar table if it is available
	if (magVarTableExists == TRUE) {
		lat = floor(lat+.5); // round to nearest integer
		lon = floor(lon+.5);
		int d = magVarTable[(int) lat+90 + 181*(((int) lon+360) % 360)];
		double magV = (360.0 * d) / 0x10000;
		if (magV > 180)
			magV -= 360;
		return(-magV);
	}

	// use very slow searching-for-nearest-VOR algorithm if MagVar table doesn't exist
	double mdev = 0.0;	
	int closest = findNearestNavaid(lat, lon);
	if (closest != -1)
    	mdev = db[closest].magdev;
	return(mdev); 
}

void CNavDoc::ReadMagVarTable()	{
	if (magVarTableExists == TRUE)
		return;
	char currentDir[260];
	GetCurrentDirectory(260, currentDir);
	CString navDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	SetCurrentDirectory(navDir);
	FILE *mv;
	if ((mv = fopen("magvar.nav", "rb")) == NULL) {
		magVarTableExists = FALSE;
		return;
	}
	unsigned short buf[181*360];
	fread(&buf[0],2,181*360,mv); 
	fclose(mv);
	SetCurrentDirectory(currentDir);
	magVarTable.SetSize(181*360,1);
	for (int lat = -90; lat <=90; lat++) 
		for (int lon = 0; lon <360; lon++) 
			magVarTable[lat+90 + 181*lon] = buf[lat+90 + 181*lon]; 
	magVarTableExists = TRUE;
}

void CNavDoc::ReadMaps()	{
	if (mapsExist == TRUE)
		return;
	mapTable.SetSize(0,20000);
	FILE *mv;
	int handle, lat, lon;
	_finddata_t fd;
	char currentDir[260];
	GetCurrentDirectory(260, currentDir);
	CString navDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	SetCurrentDirectory(navDir);
	handle = _findfirst("*.map", &fd);
	int keepgoing = handle;
	while (keepgoing != -1L) { 
		if ((mv = fopen(fd.name, "rb")) == NULL) {
			mapsExist = FALSE;
			CString err = "Error opening map file ";
			err += fd.name;
			AfxMessageBox(err);
			return;
		}
		while(!feof(mv)) {
 			fread(&lat, 1, 4, mv);
			if(feof(mv))
				break;
 			fread(&lon, 1, 4, mv);
			mapTable.Add(lat);
			mapTable.Add(lon);
		}
		fclose(mv);
		keepgoing = _findnext(handle, &fd);
	}
	_findclose(handle);
	SetCurrentDirectory(currentDir);
	mapsExist = TRUE;
}

BOOL CNavDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	// If pHandlerInfo is NULL, then handle the message
	if (pHandlerInfo == NULL) {
		// Filter the commands sent to a list file menu option
		for (UINT i = WM_NAV; i < WM_NAV+3*(UINT)listFiles; i++) {
			if (nID == i) {
				if (nCode == CN_COMMAND) {
					// Handle WM_COMMAND message
					// build list of files	
					CArray<CString, CString> filelist;	 
					filelist.SetSize(0,5);
					int result, handle;
					_finddata_t fd;
					CString fileSpec;
					fileSpec = "*.lst";
					char currentDir[260];
					GetCurrentDirectory(260, currentDir);
					CString navDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
					SetCurrentDirectory(navDir);
					result = handle = _findfirst((LPCTSTR) fileSpec, &fd);
					while (result != -1L) { 
						if (_stricmp(fd.name, "Nav.lst") != 0)
							filelist.Add(fd.name);
						result = _findnext(handle, &fd);
					}
					_findclose(handle);
  					SetCurrentDirectory(currentDir);
					if (i-WM_NAV < (UINT)listFiles) {
						listFileName = filelist[i-WM_NAV];
						OnFileAdd();
					} else if (i-WM_NAV < 2*(UINT)listFiles) {
						listFileName = filelist[(i-WM_NAV)%listFiles];
						OnFileSearch();
					} else {
						listFileName = filelist[(i-WM_NAV)%listFiles];
						OnFileLoad();
					}
					filelist.RemoveAll();
					filelist.FreeExtra();
				} else if (nCode == CN_UPDATE_COMMAND_UI) {
					// Update UI element state
					if (i-WM_NAV < (UINT)listFiles)
						((CCmdUI*)pExtra)->Enable(fName != "");			
					else
						((CCmdUI*)pExtra)->Enable(TRUE);
				}
				return TRUE;
			}
		}
	}

	// If we didn't process the command, call the base class
	// version of OnCmdMsg so the message-map can handle the message
	return CDocument::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CNavDoc::OnUpdateFileAddtoscenerylistNew(CCmdUI* pCmdUI) {
	// build list of files	
	CArray<CString, CString> filelist;	 
	filelist.SetSize(0,5);
	int result, handle;
	_finddata_t fd;
	CString fileSpec;
	fileSpec = "*.lst";
	char currentDir[260];
	GetCurrentDirectory(260, currentDir);
	CString navDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	SetCurrentDirectory(navDir);
	result = handle = _findfirst((LPCTSTR) fileSpec, &fd);
	while (result != -1L) { 
		if (_stricmp(fd.name, "Nav.lst") != 0)
			filelist.Add(fd.name);
		result = _findnext(handle, &fd);
	}
	_findclose(handle);
  	SetCurrentDirectory(currentDir);

	// Locate the File submenu
	CMenu* pFileMenu = NULL;
	CMenu* pTopMenu = AfxGetMainWnd()->GetMenu();
	int iPos;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_FILE_OPEN) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);
	pTopMenu = pFileMenu;
	pFileMenu = NULL;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_FILE_ADDTOSCENERYLIST_NEW) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);

	// Update the Add submenu 
	// First, delete all items but ID_FILE_LIST_OPEN_NAVLST at position 0
	for (iPos = pFileMenu->GetMenuItemCount()-1; iPos > 0; iPos--)
		pFileMenu->DeleteMenu(iPos, MF_BYPOSITION);
	// Then, add an item for each available file
	pFileMenu->AppendMenu(MF_SEPARATOR);
	pFileMenu->AppendMenu(MF_STRING, ID_FILE_ADDTOSCENERYLIST_NAVLST, "N&av.lst");
	for (int i = 0; i < filelist.GetSize(); i++)	{
		pFileMenu->AppendMenu(MF_STRING, WM_NAV+i, (LPCTSTR) filelist[i]);
	}
	listFiles = i;

	pFileMenu = NULL;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_FILE_SEARCHFORBGLFILES_NEW) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);
	// Update the Search submenu 
	// First, delete all items but ID_FILE_LIST_SAVE_NEW at position 0
	for (iPos = pFileMenu->GetMenuItemCount()-1; iPos > 0; iPos--)
		pFileMenu->DeleteMenu(iPos, MF_BYPOSITION);
	// Then, add a separator and an item for each available file
	pFileMenu->AppendMenu(MF_SEPARATOR);
	pFileMenu->AppendMenu(MF_STRING, ID_FILE_SEARCHFORBGLFILES_NAVLST, "N&av.lst");
	for (i = 0; i < filelist.GetSize(); i++)	{
		pFileMenu->AppendMenu(MF_STRING, WM_NAV+i+listFiles, (LPCTSTR) filelist[i]);
	}

	pFileMenu = NULL;
	for (iPos = pTopMenu->GetMenuItemCount()-1; iPos >= 0; iPos--) {
		CMenu* pMenu = pTopMenu->GetSubMenu(iPos);
		if (pMenu && pMenu->GetMenuItemID(0) == ID_FILE_LOADSCENERYLIST_NAVLST) {
			pFileMenu = pMenu;
			break;
		}
	}
	ASSERT(pFileMenu != NULL);
	// Update the Load submenu 
	// First, delete all items but Nav.lst at position 0
	for (iPos = pFileMenu->GetMenuItemCount()-1; iPos > 0; iPos--)
		pFileMenu->DeleteMenu(iPos, MF_BYPOSITION);
	// Then, add an item for each available file
	for (i = 0; i < filelist.GetSize(); i++)	{
		pFileMenu->AppendMenu(MF_STRING, WM_NAV+i+2*listFiles, (LPCTSTR) filelist[i]);
	}

	filelist.RemoveAll();
	filelist.FreeExtra();

	pCmdUI->Enable(fName != "");			
}

void CNavDoc::OnFileAddtoscenerylistNavlst() {
	listFileName = "Nav.lst";
	OnFileAdd();
}

void CNavDoc::OnUpdateFileAddtoscenerylistNavlst(CCmdUI* pCmdUI) {
	pCmdUI->Enable(fName != "");			
}

void CNavDoc::OnFileLoadscenerylistNavlst() {
	OnFileLoad("Nav.lst");
}

void CNavDoc::OnFileSearchforbglfilesNavlst() {
	listFileName = "Nav.lst";
	OnFileSearch();
}

void CNavDoc::OnFileAddtoscenerylistNew() {
	if (newList() == TRUE)	
		OnFileAdd();
}

void CNavDoc::OnFileSearchforbglfilesNew() {
	if (newList() == TRUE)
		OnFileSearch();		
}

int CNavDoc::newList() {
	NewList d;
	d.m_name = "Untitled.lst";
	if (d.DoModal() == IDOK) {
		CString fileName = d.m_name;
		// make sure the name ends with .lst
		CString ext = d.m_name.Right(4);
		if (ext.CompareNoCase(".lst") != 0)
			fileName = fileName + ".lst";
		// make sure the file doesn't already exist
		// build list of files	
		CArray<CString, CString> filelist;	 
		filelist.SetSize(0,5);
		int result, handle;
		_finddata_t fd;
		CString fileSpec;
		fileSpec = "*.lst";
		char currentDir[260];
		GetCurrentDirectory(260, currentDir);
		CString navDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
		SetCurrentDirectory(navDir);
		result = handle = _findfirst((LPCTSTR) fileSpec, &fd);
		while (result != -1L) { 
			filelist.Add(fd.name);
			result = _findnext(handle, &fd);
		}
		_findclose(handle);
  		SetCurrentDirectory(currentDir);
		for (int i = 0; i < filelist.GetSize(); i++) {
			if (fileName == filelist[i]) {
				AfxMessageBox(fileName + " already exists!");
				filelist.RemoveAll();
				filelist.FreeExtra();
				return(FALSE);
			}
		}
		filelist.RemoveAll();
		filelist.FreeExtra();
		// set the filename
		listFileName = fileName;
		return(TRUE);
	} else 
		return(FALSE);
}

void CNavDoc::Index() {
	runwayIndex.RemoveAll();
	runwayIndex.FreeExtra();
	ilsIndex.RemoveAll();
	ilsIndex.FreeExtra();
	vorIndex.RemoveAll();
	vorIndex.FreeExtra();
	ndbIndex.RemoveAll();
	ndbIndex.FreeExtra();
	markerIndex.RemoveAll();
	markerIndex.FreeExtra();
	gpsIndex.RemoveAll();
	gpsIndex.FreeExtra();
	airportIndex.RemoveAll();
	airportIndex.FreeExtra();
	atisIndex.RemoveAll();
	atisIndex.FreeExtra();
	dynobjIndex.RemoveAll();
	dynobjIndex.FreeExtra();
	dynscnIndex.RemoveAll();
	dynscnIndex.FreeExtra();
	waypointIndex.RemoveAll();
	waypointIndex.FreeExtra();
	routeIndex.RemoveAll();
	routeIndex.FreeExtra();
	for (int i=0; i<dbCount; i++) {
		if (1 == db[i].type)
			ndbIndex.Add(i);
		else if (2 == db[i].type)
			ilsIndex.Add(i);
		else if (3 == db[i].type)
			vorIndex.Add(i);
		else if (4 == db[i].type)
			airportIndex.Add(i);
		else if (5 == db[i].type)
			markerIndex.Add(i);
		else if (6 == db[i].type)
			markerIndex.Add(i);
		else if (7 == db[i].type)
			markerIndex.Add(i);
		else if (8 == db[i].type)
			runwayIndex.Add(i);
		else if (9 == db[i].type)
			gpsIndex.Add(i);   
		else if (10 == db[i].type)
			atisIndex.Add(i);   
		else if (11 == db[i].type)
			dynobjIndex.Add(i);   
		else if (12 == db[i].type)
			dynscnIndex.Add(i);   
		else if (13 == db[i].type)
			waypointIndex.Add(i);   
		else if (14 == db[i].type)
			routeIndex.Add(i);   
	}
}

void CNavDoc::OnFileLoad(CString fileListName) {
	listFileName = fileListName;
	OnFileLoad();
}

int CNavDoc::duplicateAirport(char *name, double lat, double lon) {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	double dis;
	CString cname = CString(name), dbName;
	cname.TrimRight();
	cname.MakeLower();
	for (int n=0; n<dbCount; n++) {
		if (db[n].type != 4) 
			continue;				// this db item is not an airport
		if (fabs(db[n].lat-lat) < .1) {		// quick check - within .1 degree
			if (fabs(db[n].lon-lon) < .1) {
				dis = v->gcDist(lat, lon, db[n].lat, db[n].lon);
				if (dis < 1000.0) 
					return TRUE;	// so close they must? be the same
				dbName = CString(db[n].name);
				dbName.TrimRight();
				dbName.MakeLower();
				if (cname == dbName)  
					return TRUE;			// same name
			}
		}
	}
	return FALSE;
}

/* 
waypoint format is optional tag (that is ignored, if present), followed by 
name, then the latitude and longitude (which can be either in decimal or 
degree.minute.second format):
JUNKTAG AALEN N48.36.10.0 E010.07.10.0
AALEN N48.36.10 E010.07.10
JUNKTAG AALEN  48.60277  10.11944
AALEN 48.60277 10.11944

route format is a name, then the starting latitude and longitude followed by 
the ending latitude and longitude (which can be either in decimal or 
degree.minute.second format):
J10-231 N034.06.44.000 W115.46.11.700 N034.12.23.600 W115.20.34.400
J10-231 34.1122 -115.7699 34.2065 -115.3428

Whitespace is ignored, but the "name" must have no spaces

It takes a long time to load these text files, so they are compiled
to a fast loading binary format the first time they are read.
*/
void CNavDoc::ReadWaypoints() {
	if (waypointsExist == TRUE)
		return;

	char junk[255], name[255], latdir, londir, latdir2, londir2;
	int latd, latm, lond, lonm, latd2, latm2, lond2, lonm2, converted;
	double lat, lon, lat2, lon2;
	char currentDir[260];
	GetCurrentDirectory(260, currentDir);
	CString navDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	SetCurrentDirectory(navDir);
	FILE *mv, *out;
	_finddata_t fd;
	long filePos;
	int dmsFlag;

	// parse waypoint files
	int handle = _findfirst("*.wpt", &fd);
	int keepgoing = handle;
	while (keepgoing != -1L) { 
		int success = FALSE;

        // check for alternate binary version of this file for faster loading 
        char altName[260];
		strcpy(altName, fd.name);
		int len = strlen(altName);
		strcpy(&altName[len], ".c");

		if (fileNewer(altName, fd.name)) {
			if ((mv = fopen(altName, "rb")) != NULL) {
				//read and process binary file
				float flat1, flon1;
				char nLength, name[260];
				while(!feof(mv)) {
					fread(&nLength, 1, 1, mv);
					if (feof(mv))
						break;
					fread(&name, 1, nLength, mv);
 					fread(&flat1, 1, 4, mv);
 					fread(&flon1, 1, 4, mv);
					name[nLength] = 0x00;
					addDB(13, (float)flat1, (float)flon1, 0, 0, name, name, 0, 0, 0);
				}
				dbFileIndex++;
				dbFiles.Add(fd.name);
				fclose(mv);
			}
		} else {
			// read and process text file, generate binary file
			if ((out = fopen(altName, "wb")) == NULL) {
				CString err = "Error opening output file for binary compiled waypoints: ";
				err += fd.name;
				AfxMessageBox(err);
				break;
			}

			if ((mv = fopen(fd.name, "r")) == NULL) {
				waypointsExist = FALSE;
				CString err = "Error opening waypoint file: ";
				err += fd.name;
				AfxMessageBox(err);
				fclose(out);
				break;
			}
			
			// parse lines
			int bailOut = FALSE;
			while(!feof(mv)) {
				filePos=ftell(mv);
				converted = fscanf(mv, "%s %lf %lf\n", name, &lat, &lon);
				if (converted == 3) 
					dmsFlag=FALSE;
				else {
					fseek(mv, filePos, SEEK_SET);
					converted = fscanf(mv, "%s %s %lf %lf\n", junk, name, &lat, &lon);
					if (converted == 4) 
						dmsFlag=FALSE;
					else {
						fseek(mv, filePos, SEEK_SET);
						converted = fscanf(mv, "%s %c%d.%d.%lf %c%d.%d.%lf\n", name, &latdir, &latd, &latm, &lat, &londir, &lond, &lonm, &lon);
						if (converted == 9) 
							dmsFlag=TRUE;
						else {
							fseek(mv, filePos, SEEK_SET);
							converted = fscanf(mv, "%s %s %c%d.%d.%lf %c%d.%d.%lf\n", junk, name, &latdir, &latd, &latm, &lat, &londir, &lond, &lonm, &lon);
							if (converted == 10) 
								dmsFlag=TRUE;
							else {
								fseek(mv, filePos, SEEK_SET); // kludge for SB format files without whitespace
								name[0]=getc(mv);
								if ((name[0] == ';') || (name[0] == ';')) { //skip line
									while(name[0] != '\n')
										name[0]=getc(mv);
									continue;
								}
								for (int k=1; k<5; k++) {
									if ((name[k]=getc(mv)) == ' ')
										name[k]=0x00;
								}
								name[5]=0x00;
								converted = fscanf(mv, "%lf %lf\n", &lat, &lon);
								if ((converted == 2) && (strlen(name) > 0)) 
									dmsFlag=FALSE;
								else {
									CString err = "Error reading waypoint file: ";
									err += fd.name;
									AfxMessageBox(err);
									bailOut = TRUE;
								}
							}
						}
					}
				}

				if (!bailOut) {
					success = TRUE;		

					if (dmsFlag) {
						lat = lat/3600.0 + latm/60.0 + latd;
						if (latdir == 'S')
							lat = 0.0-lat;
						lon = lon/3600.0 + lonm/60.0 + lond;
						if (londir == 'W')
							lon = 0.0-lon;
					}		

					addDB(13, lat, lon, 0, 0, name, name, 0, 0, 0);
					// write compiled version
					float flat1 = (float) lat;
					float flon1 = (float) lon;
					char nLength = (char) strlen(name);
					fwrite(&nLength, 1, 1, out);
					fwrite(&name, 1, nLength, out);
					fwrite(&flat1, 1, 4, out);
					fwrite(&flon1, 1, 4, out);
				} else
					break;
			}
			if (success) {
				dbFileIndex++;
				dbFiles.Add(fd.name);
			}
			fclose(out);
			fclose(mv);
			if (bailOut) 
				DeleteFile(altName);
		}
		keepgoing = _findnext(handle, &fd);
	}
	_findclose(handle);

	// parse route files
	handle = _findfirst("*.rt", &fd);
	keepgoing = handle;
	while (keepgoing != -1L) { 
	    int success = FALSE;
        // check for alternate binary version of this file for faster loading 
        char altName[260];
		strcpy(altName, fd.name);
		int len = strlen(altName);
		strcpy(&altName[len], ".c");

		if (fileNewer(altName, fd.name)) {
			if ((mv = fopen(altName, "rb")) != NULL) {
				//read and process binary file
				float flat1, flon1, flat2, flon2;
				char nLength, name[260];
				while(!feof(mv)) {
					fread(&nLength, 1, 1, mv);
					if (feof(mv))
						break;
					fread(&name, 1, nLength, mv);
 					fread(&flat1, 1, 4, mv);
 					fread(&flon1, 1, 4, mv);
 					fread(&flat2, 1, 4, mv);
 					fread(&flon2, 1, 4, mv);
					name[nLength] = 0x00;
					addDB(14, (float)flat1, (float)flon1, 0, 0, name, name, 0, (float)flat2, (float)flon2);
				}
				dbFileIndex++;
				dbFiles.Add(fd.name);
				fclose(mv);
			}
		} else {
			// read and process text file, generate binary file
			if ((out = fopen(altName, "wb")) == NULL) {
				CString err = "Error opening output file for binary compiled route: ";
				err += fd.name;
				AfxMessageBox(err);
				return;
			}

			if ((mv = fopen(fd.name, "r")) == NULL) {
				waypointsExist = FALSE;
				CString err = "Error opening route file: ";
				err += fd.name;
				AfxMessageBox(err);
				fclose(out);
				return;
			}
			
			// parse lines
			while(!feof(mv)) {
				filePos=ftell(mv);
				converted = fscanf(mv, "%s %c%d.%d.%lf %c%d.%d.%lf %c%d.%d.%lf %c%d.%d.%lf\n", name, &latdir, &latd, &latm, &lat, &londir, &lond, &lonm, &lon, &latdir2, &latd2, &latm2, &lat2, &londir2, &lond2, &lonm2, &lon2);
				if (converted == 17) 
					dmsFlag=TRUE;
				else {
					fseek(mv, filePos, SEEK_SET);
					converted = fscanf(mv, "%s %lf %lf %lf %lf\n", name, &lat, &lon, &lat2, &lon2);
					if (converted == 5) 
						dmsFlag=FALSE;
					else {
						CString err = "Error reading route file: ";
						err += fd.name;
						AfxMessageBox(err);
						break;
					}
				}
				success = TRUE;

				if (dmsFlag) {
					lat = lat/3600.0 + latm/60.0 + latd;
					if (latdir == 'S')
						lat = 0.0-lat;
					lon = lon/3600.0 + lonm/60.0 + lond;
					if (londir == 'W')
						lon = 0.0-lon;
					lat2 = lat2/3600.0 + latm2/60.0 + latd2;
					if (latdir2 == 'S')
						lat2 = 0.0-lat2;
					lon2 = lon2/3600.0 + lonm2/60.0 + lond2;
					if (londir2 == 'W')
						lon2 = 0.0-lon2;
				}
				addDB(14, lat, lon, 0, 0, name, name, 0, lat2, lon2);
				// write compiled version
				float flat1 = (float) lat;
				float flon1 = (float) lon;
				float flat2 = (float) lat2;
				float flon2 = (float) lon2;
				char nLength = (char) strlen(name);
				fwrite(&nLength, 1, 1, out);
				fwrite(&name, 1, nLength, out);
				fwrite(&flat1, 1, 4, out);
				fwrite(&flon1, 1, 4, out);
				fwrite(&flat2, 1, 4, out);
				fwrite(&flon2, 1, 4, out);
			}
			if (success) {
				dbFileIndex++;
				dbFiles.Add(fd.name);
			}
		    fclose(mv);
			fclose(out);
		}
		keepgoing = _findnext(handle, &fd);
	}
	_findclose(handle);

	SetCurrentDirectory(currentDir);
	waypointsExist = TRUE;
}

void CNavDoc::OnFileSavecompiledscenerylist() {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	CString fileName = v->orgDir + "\\" + listFileName + ".c";

	FILE *fp = fopen(fileName,"wb");
	if (fp == NULL) {
		AfxMessageBox("Could not save compiled scenery list to " + fileName);
		compiledSceneryList = FALSE;
		return;
	}

	int dbFilesNum = dbFiles.GetSize();
	fwrite(&dbFilesNum, 1, 4, fp);
	for (int f=0; f<dbFilesNum; f++) {
		int filenameLength = dbFiles[f].GetLength();
		fwrite(&filenameLength, 1, 4, fp);
		fwrite(dbFiles[f].GetBuffer(1), 1, filenameLength, fp);
		dbFiles[f].ReleaseBuffer(1);
	}
	
	dbRec r;
	int dbNum = db.GetSize();
	fwrite(&dbNum, 1, 4, fp);
	for (int d=0; d<dbNum; d++) {
		r = db[d];
		fwrite(&r, 1, sizeof(dbRec), fp);
	}
	fwrite(&waypointsExist, 1, 4, fp);

	fclose(fp);
	compiledSceneryList = TRUE;
}

void CNavDoc::OnUpdateFileSavecompiledscenerylist(CCmdUI* pCmdUI) {
	pCmdUI->Enable(!compiledSceneryList && (listFileName!=""));	
}

void CNavDoc::OnFileDeletecompiledscenerylist() {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	CString fileName = v->orgDir + "\\" + listFileName + ".c";
	unlink(fileName);	
	compiledSceneryList = FALSE;
}

void CNavDoc::OnUpdateFileDeletecompiledscenerylist(CCmdUI* pCmdUI) {
	pCmdUI->Enable(compiledSceneryList);	
}

int CNavDoc::ReadCompiledScenery(CString fileName) {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	FILE *fp = fopen(fileName,"rb");
	if (fp == NULL) {
		AfxMessageBox("Could not read compiled scenery list from " + fileName);
		compiledSceneryList = FALSE;
		return FALSE;
	}

	char dbFileName[260];
	int dbFilesNum, filenameLength;
	fread(&dbFilesNum, 1, 4, fp);
	for (int f=0; f<dbFilesNum; f++) {
		fread(&filenameLength, 1, 4, fp);
		fread(dbFileName, 1, filenameLength, fp);
		dbFileName[filenameLength] = 0x00;
		dbFiles.Add(dbFileName);
		dbFileIndex++;
	}

	dbRec r;
	int dbNum;
	fread(&dbNum, 1, 4, fp);
	for (int d=0; d<dbNum; d++) {
		fread(&r, 1, sizeof(dbRec), fp);
		db.Add(r);
		// re-index here for speed instead of calling Index()
		if (1 == r.type)
			ndbIndex.Add(d);
		else if (2 == r.type)
			ilsIndex.Add(d);
		else if (3 == r.type)
			vorIndex.Add(d);
		else if (4 == r.type)
			airportIndex.Add(d);
		else if (5 == r.type)
			markerIndex.Add(d);
		else if (6 == r.type)
			markerIndex.Add(d);
		else if (7 == r.type)
			markerIndex.Add(d);
		else if (8 == r.type)
			runwayIndex.Add(d);
		else if (9 == r.type)
			gpsIndex.Add(d);   
		else if (10 == r.type)
			atisIndex.Add(d);   
		else if (11 == r.type)
			dynobjIndex.Add(d);   
		else if (12 == r.type)
			dynscnIndex.Add(d);   
		else if (13 == r.type)
			waypointIndex.Add(d);   
		else if (14 == r.type)
			routeIndex.Add(d);   
		dbCount++;
	}
	fread(&waypointsExist, 1, 4, fp);

	fclose(fp);
	compiledSceneryList = TRUE;

	char buff[200];
	sprintf(buff, "%d files loaded, %d items in database", dbFileIndex, dbCount);
	((CMainFrame *) AfxGetMainWnd())->SetStatusBar(buff);
	SetTitle("Scenery List: " + listFileName);
	fName = "";
	v->ZoomToFit();

	return TRUE;
}

void CNavDoc::ReadAirports() {
	if (airportsExist == TRUE)
		return;

	airportTable.SetSize(0,1000);	
	FILE *mv;
	int len;
	float lat, lon;
	char name[30];
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);

	CString fileNameC = v->orgDir + "\\airports.dat.c";
	CString fileName = v->orgDir + "\\airports.dat";

	if (fileNewer(fileNameC, fileName)) {
		// attempt to read compiled version
		if ((mv = fopen(fileNameC, "rb")) != NULL) {
			int airportTableNum;
			fread(&airportTableNum, 1, 4, mv);
			for (int f=0; f<airportTableNum; f++) {
				fread(&len, 1, 4, mv);
				fread(name, 1, len, mv);
				fread(&lat, 1, 4, mv);
				fread(&lon, 1, 4, mv);
				name[len] = 0x00;
				airportRec r;
				strcpy(r.name, name);
				r.lat = lat;
				r.lon = lon;
				airportTable.Add(r);
			}

			fclose(mv);
			airportsExist = TRUE;
			return;
		}
	}

	// attempt to read un-compiled version
	if ((mv = fopen(fileName, "r")) == NULL) 
		return;
	
	int converted, cont=TRUE, c;
	while (cont) {
		c = getc(mv);
		if (';' == c) { // skip rest of line
			while ('\n' != (c = getc(mv))) 
				if (EOF == c) {
					cont = FALSE;
					break;
				}
			continue;
		} else 
			ungetc(c, mv);
		if (3 == (converted = fscanf(mv, "%s %f%f\n", name, &lat, &lon))) {
			airportRec r;
			r.lat = lat;
			r.lon = lon;
			strcpy(r.name, name);
			airportTable.Add(r);
			continue;
		}
		break;
	}

	fclose(mv);
	airportsExist = TRUE;

	// sort airports? (speed doesn't seem to be a problem)
	// save compiled version
	fileName = v->orgDir + "\\airports.dat.c";
	
	FILE *fp = fopen(fileName,"wb");
	if (fp == NULL) 
		return;

	airportRec r;
	int airportTableNum = airportTable.GetSize();
	fwrite(&airportTableNum, 1, 4, fp);
	for (int f=0; f<airportTableNum; f++) {
		r = airportTable[f];
		len = strlen(r.name);
		fwrite(&len, 1, 4, fp);
		fwrite(r.name, 1, len, fp);
		fwrite(&(r.lat), 1, 4, fp);
		fwrite(&(r.lon), 1, 4, fp);
	}
	fclose(fp);
}

// optional dbIndex for setting db[dbIndex].id to ICAO
int CNavDoc::lookUpAirport(CString &name, double lat, double lon, int dbIndex) {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);

	// use existing id, if any
	if (dbIndex != -100) {
		if (db[dbIndex].id[0] != 0x00) {
			name = CString(db[dbIndex].id);
			return TRUE;
		} 
	} else {
		int foundIndex=findNearestNavaid(lat, lon, 4);
		if (foundIndex != -1) {
			double d = v->gcDist(lat, lon, db[foundIndex].lat, db[foundIndex].lon);
			if ((d < 5000) && (db[foundIndex].id[0] != 0x00)) {
				name = CString(db[foundIndex].id);
				return TRUE;
			} 
		}
	}

	if (airportsExist == FALSE)
		return FALSE;

	double minLat = lat - 0.5;
	double maxLat = lat + 0.5;
	double minLon = lon - 0.5;
	double maxLon = lon + 0.5;
	double minDist=10000000;
	double dist=10000000;
	airportRec r;
	CString foundName;

	int airportTableNum = airportTable.GetSize();
	for (int f=0; f<airportTableNum; f++) {
		r = airportTable[f];
		if (r.lon < minLon)
			continue;
		if (r.lon > maxLon)
			continue;
		if (r.lat < minLat)
			continue;
		if (r.lat > maxLat)
			continue;
		dist = v->gcDist(lat, lon, r.lat, r.lon);
		if (dist < 500) { // close enough for Gov work (reduced from 5000 to fix problem at Honolulu) 
			if (strlen(r.name) == 4) { // try to use 4 letter versions of ID
				name = CString(r.name);
				if (dbIndex != -100) { // store ICAO in db record
					strncpy(db[dbIndex].id, r.name, 5);
					db[dbIndex].id[5] = 0x00;
				}
				return TRUE;
			}
		}
		if (dist < minDist) {
			if (strlen(r.name) != 4)  // bias against non-4 letter versions of ID
				dist += 0.01;
			foundName = CString(r.name);
			minDist = dist;
		}
	}
	if (minDist < 20000) { // close enough for everyone
		name = foundName;
		if (dbIndex != -100) { // store ICAO in db record
			strncpy(db[dbIndex].id, name.GetBuffer(1), 5);
			name.ReleaseBuffer();
			db[dbIndex].id[5] = 0x00;
		}
		return TRUE;
	} else
		return FALSE;
}

// return TRUE if fileName1 was modified more recently than fileName2
int CNavDoc::fileNewer(CString fileName1, CString fileName2) {
	FILETIME ft1, ft2;
	CFile f1, f2;

	if (0 == f1.Open(fileName1, CFile::modeRead)) 
		return FALSE;
	if (0 == GetFileTime((HANDLE)f1.m_hFile, NULL, NULL, &ft1)) {
		f1.Close();
		return FALSE;
	}
	f1.Close();

	if (0 == f2.Open(fileName2, CFile::modeRead)) 
		return FALSE;
	if (0 == GetFileTime((HANDLE)f2.m_hFile, NULL, NULL, &ft2)) {
		f2.Close();
		return FALSE;
	}
	f2.Close();

	if (ft1.dwHighDateTime > ft2.dwHighDateTime)
		return TRUE;
	if (ft1.dwHighDateTime == ft2.dwHighDateTime)
		if (ft1.dwLowDateTime > ft2.dwLowDateTime)
			return TRUE;
	return FALSE;
}

// return the number of runways that probably go with the given airport
int CNavDoc::lookUpRunways(int airport, int *runwaysIndexList) {
	if (db[airport].type != 4)
		return 0;
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	double minLat = db[airport].lat - 0.5;
	double maxLat = db[airport].lat + 0.5;
	double minLon = db[airport].lon - 0.5;
	double maxLon = db[airport].lon + 0.5;
	double dist;
	int found = 0;

	for (int i=0; i<runwayIndex.GetSize(); i++) {
		if (db[runwayIndex[i]].lon < minLon)
			continue;
		if (db[runwayIndex[i]].lon > maxLon)
			continue;
		if (db[runwayIndex[i]].lat < minLat)
			continue;
		if (db[runwayIndex[i]].lat > maxLat)
			continue;
		dist = v->gcDist(db[airport].lat, db[airport].lon, db[runwayIndex[i]].lat, db[runwayIndex[i]].lon);
		if (dist < 5000) { // 5000 is too much! some small airports are less than 1km aparts
			int nearestApt = findNearestNavaid(db[runwayIndex[i]].lat, db[runwayIndex[i]].lon, 4);
			if (nearestApt == airport)
				runwaysIndexList[found++] = runwayIndex[i];
		}
	}

	// delete duplicates
	for (i=0; i<found; i++) {
		for (int j=0; j<i; j++) {
			dist = v->gcDist(db[runwaysIndexList[j]].lat, db[runwaysIndexList[j]].lon, db[runwaysIndexList[i]].lat, db[runwaysIndexList[i]].lon);
			if (dist < 20) { 
				if (fabs(db[runwaysIndexList[j]].approach-db[runwaysIndexList[i]].approach) < 5) {
					// too close, shift remaining 
					for (int k=j; k<found-1; k++) 
						runwaysIndexList[k] = runwaysIndexList[k+1];
					found--;
				}
			}
		}
	}
	return found;
}

// return the number of ILSs that probably go with the given runway
// firstILS is the east most ILS, secondILS is the west most ILS
int CNavDoc::lookUpILS(int runway, int &firstILS, int &secondILS) {
	firstILS = secondILS = -1;
	if (db[runway].type != 8)
		return 0;
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	double minLat = db[runway].lat - 0.5;
	double maxLat = db[runway].lat + 0.5;
	double minLon = db[runway].lon - 0.5;
	double maxLon = db[runway].lon + 0.5;
	int found = 0;
	double dist;

	for (int i=0; i<ilsIndex.GetSize(); i++) {
		if (db[ilsIndex[i]].lon < minLon)
			continue;
		if (db[ilsIndex[i]].lon > maxLon)
			continue;
		if (db[ilsIndex[i]].lat < minLat)
			continue;
		if (db[ilsIndex[i]].lat > maxLat)
			continue;

		// calculate ils dist & heading
		double fullRwLength = db[runway].range*.3048; // in meters (changed from half to full to accomodate AFD ILSs) 
		dist = v->gcDist(db[ilsIndex[i]].lat, db[ilsIndex[i]].lon, db[runway].lat, db[runway].lon);
		if (dist < fullRwLength) { // within radius of runway
			double rwCourse = db[runway].approach;
			double ilsCourse = db[ilsIndex[i]].approach;
			if((fabs(courseDiff(v->bearing, rwCourse)) < 5)&&(fabs(courseDiff(v->bearing, ilsCourse)) < 5)) { // within 5 degrees
				if (rwCourse >= 180.0) 
					secondILS = ilsIndex[i];
				else 
					firstILS = ilsIndex[i];
			} else {
				rwCourse = rwCourse + 180;  // try reciprocal heading
				if (rwCourse > 360)
					rwCourse -= 360;
				if((fabs(courseDiff(v->bearing, rwCourse)) < 5)&&(fabs(courseDiff(v->bearing, ilsCourse)) < 5)) { // within 5 degrees
					if (rwCourse >= 180.0) 
						secondILS = ilsIndex[i];
					else 
						firstILS = ilsIndex[i];
				}
			}
		}
	
	}
	if (firstILS != -1)
		found++;
	if (secondILS != -1)
		found++;
	return found;
}

// try to find the index of something in the database that matches this
// used by DP/STAR insertion, by default it prefers nothing
int CNavDoc::findNearbyNavaid(double lat, double lon, CString id, int preferType) {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	double dist;
	int found = -1;
	for (int i=0; i<dbCount; i++) {
		if (fabs(db[i].lat-lat) > 0.1) 
			continue;
		if (fabs(db[i].lon-lon) > 0.1) 
			continue;
		dist = v->gcDist(db[i].lat, db[i].lon, lat, lon);
		if (dist < 10000) { // assume two things this close won't have the same id
			CString dbId = CString(db[i].id);
			dbId.TrimRight();
			dbId.MakeLower();
			id.TrimRight();
			id.MakeLower();
			if (id == dbId) {
				if ((preferType==0) || (preferType == db[i].type))
					return i;
				else // keep looking for a preferred match
					found = i;
			}
		}
	}
	return found; 
}

// try to find the index of something in the database that matches this
int CNavDoc::findIndex(int type, double lat, double lon, int alt, double freq, char * id, char * name, double magdev, double approach, double range, int howClose, int findMin) {
	POSITION p = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(p);
	double dist;
	int min = -1;  // not found
	for (int i=0; i<dbCount; i++) {
		if (db[i].type != type)
			continue;
		if (fabs(db[i].lat-lat) > 0.1) 
			continue;
		if (fabs(db[i].lon-lon) > 0.1) 
			continue;
		dist = v->gcDist(db[i].lat, db[i].lon, lat, lon);
		if(dist > howClose)   // howClose defaults to 50 meters
			continue;
		if (!findMin)
			return i;
		else {
			min = i;
			howClose = (int) dist;
		}
	}
	return min; 
}

// return the difference from first course to second course, 
// wrapping at 360 degrees, clockwise is positive
double CNavDoc::courseDiff(double c1, double c2) {
	double d = c2 - c1;
	if (fabs(d) < 180.0)
		return d;
	else if ((d == 180.0) || (d == -180.0))
		return 180.0;
	else if (d > 180.0)
		return 360.0 - d;
	else // if (d < -180.0)
		return -360.0 - d;
}

void CNavDoc::OnFileExtractfs2000waypointsandroutes() {
	POSITION vp = GetFirstViewPosition();
	CNavView *v = (CNavView *) GetNextView(vp);
	// try to find fs2000 installation here to set starting path	
	CString fs2000Dir = "";
	HKEY key; 
	if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\Microsoft Games\\Flight Simulator\\7.0", 0, KEY_READ, &key)) {
		unsigned char pathbuf[1024];
		DWORD type, length = 1024;
		if (ERROR_SUCCESS ==  RegQueryValueEx(key, "path", NULL, &type, pathbuf, &length)) {
			fs2000Dir = CString(pathbuf);
		}
		RegCloseKey(key);
	}
	if (fs2000Dir == "") {
		AfxMessageBox("FS2000 does not seem to be installed");
		return;
	}

	EnterCriticalSection(&crit);

	char currentDir[260];
	GetCurrentDirectory(260, currentDir);
	CString navDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	SetCurrentDirectory(navDir);

	CWaitCursor wait;	// display wait cursor
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	char buffer[300];

	// arrays of strings
	CArray<CString, CString> allBGLFiles;
	CArray<CString, CString> routes;
	findAllBGLFiles(fs2000Dir, allBGLFiles);

	// hash table mapping strings to latLon pairs
	CMap< CString, LPCSTR, latLon, latLon& > waypoints;
	waypoints.InitHashTable(51111);  ;// size for 39236

	// extract 
	for (int i=0; i<allBGLFiles.GetSize(); i++) {
		sprintf(buffer, "Extracting waypoint and route data from %s", allBGLFiles[i]);
		main->SetStatusBar(buffer);
		extractWaypointsAndRoutes(allBGLFiles[i], waypoints, routes);
	}

	// remove duplicate routes
	sprintf(buffer, "Checking for duplicate routes");
	main->SetStatusBar(buffer);
	for (i=0; i<routes.GetSize(); i++) {
		for (int j=0; j<i; j++) {
			if (routes[i] == routes[j]) {
				routes[i] = routes[routes.GetUpperBound()];
				routes.RemoveAt(routes.GetUpperBound(), 1);
				i--;
				break;
			}
		}
	}

	// iterate through all waypoints, write waypoint file
	sprintf(buffer, "Writing new waypoint file");
	main->SetStatusBar(buffer);
	remove("World.wpt.c"); // delete old compiled version if present
	FILE *out = fopen("World.wpt", "w");
	CString id;
	latLon loc;
	POSITION p = waypoints.GetStartPosition();
	while (p != NULL) {
		waypoints.GetNextAssoc(p, id, loc);
		// strip 2 letter duplicate preventing prefix
		fprintf(out, "%5s\t%lf\t%lf\n", id.GetBuffer(1)+2, loc.lat, loc.lon);
		id.ReleaseBuffer();
	}
	fclose(out);

	// iterate through all routes, write route file (just informational)
	sprintf(buffer, "Writing new route file");
	main->SetStatusBar(buffer);
	out = fopen("routes.txt", "w");
	for (i=0; i<routes.GetSize(); i++)
		fprintf(out, "%s\n", routes[i]);
	fclose(out);

	// iterate through all routes, get lats and lons
	sprintf(buffer, "Looking up route locations");
	main->SetStatusBar(buffer);
	remove("VCanada.rt");     // delete old version if present
	remove("VCanada.rt.c");   // delete old compiled version if present
	remove("JCanada.rt");     // delete old version if present
	remove("JCanada.rt.c");   // delete old compiled version if present
	remove("victors.rt");     // delete old version if present
	remove("victors.rt.c");   // delete old compiled version if present
	remove("jetways.rt");     // delete old version if present
	remove("jetways.rt.c");   // delete old compiled version if present
	remove("allRoutes.rt");   // delete old version if present
	remove("allRoutes.rt.c"); // delete old compiled version if present
	CString name, word, route, lastWord;
	int count;
	latLon start, end;
	out = fopen("allRoutes.rt", "w");
	for (i=0; i<routes.GetSize(); i++) {
		route = routes[i];
		count = 0;
		while ("" != (word = parseString(route))) {
			if (0 == count) {
				name = word;
			} else if (1 == count) {
				if (0 == waypoints.Lookup(word, start)) {
					continue; // ignore missing waypoint 
				}
			}
			else {
				if (0 == waypoints.Lookup(word, end)) {
					continue; // ignore missing waypoint 
				}
				double bigDist = v->gcDist(start.lat, start.lon, end.lat, end.lon); 
				if ((bigDist>0)&&(bigDist<3000000)) // many apparently bogus routes are long or zero length, limit to 3000km
					fprintf(out, "%s %lf %lf %lf %lf\n", name, start.lat, start.lon, end.lat, end.lon);
				memcpy(&start, &end, sizeof(latLon));
				lastWord = word;
			}
			count++;
		}
	}
	fclose(out);

	int newWaypointCount = waypoints.GetCount();
	int newRouteCount = routes.GetSize();
	routes.RemoveAll();
	waypoints.RemoveAll();
	allBGLFiles.RemoveAll();
	allBGLFiles.FreeExtra();
	SetCurrentDirectory(currentDir);

	// reload & recompile waypoints
	sprintf(buffer, "Reloading and recompiling waypoints");
	main->SetStatusBar(buffer);
	OnNewDocument();

	Beep(100,4000);
	sprintf(buffer, "%d waypoints found, %d routes found", newWaypointCount, newRouteCount);
	main->SetStatusBar(buffer);
	LeaveCriticalSection(&crit);
}

void CNavDoc::findAllBGLFiles(CString startPath, CArray<CString, CString> &results) {
	CMainFrame *main = (CMainFrame *) AfxGetMainWnd();
	char buffer[350];
	sprintf(buffer, "Searching %s", startPath);
	main->SetStatusBar(buffer);
	
	// recurse directories first
	int result, handle;
	_finddata_t fd;
	CString path, subPath;
	path = startPath + "\\*.*";
	result = handle = _findfirst((LPCTSTR) path, &fd);
	while (result != -1L) { // look for directories
		if (fd.attrib &	_A_SUBDIR) {
			if ((strcmp(fd.name, ".") != 0) && (strcmp(fd.name, "..") != 0)) {
				subPath = startPath + "\\" + fd.name;
				findAllBGLFiles(subPath, results);
			}
		}
		result = _findnext(handle, &fd);
	}
	_findclose(handle);

	// find files now
	path = startPath + "\\*.bgl";
	result = handle = _findfirst((LPCTSTR) path, &fd);
	while (result != -1) { 
		subPath = startPath + "\\" + fd.name;
		subPath.MakeLower();
		results.Add(subPath);
		result = _findnext(handle, &fd);
	}
	_findclose(handle);
	main->SetStatusBar("");
}

int CNavDoc::extractWaypointsAndRoutes(CString filename, CMap< CString, LPCSTR, latLon, latLon& > &waypoints, CArray<CString, CString> &routes) {
	char *fname=filename.GetBuffer(1);
	char *bgl;
	char buffer[100];

	WIN32_FIND_DATA fd;	// find file size
	HANDLE findHandle = FindFirstFile(filename, &fd);
	if (findHandle == INVALID_HANDLE_VALUE) {
		sprintf(buffer, "Could not open %s\n", filename);
		AfxMessageBox(buffer);
		return(2);
	}	
	int size = (int) fd.nFileSizeLow; // assume its not more than 32 bits
	FindClose(findHandle);
	if (size < 122)
		return 0;

	HANDLE filehandle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (filehandle == INVALID_HANDLE_VALUE) {
		sprintf(buffer, "Could not open %s\n", filename);
		AfxMessageBox(buffer);
		return(2);
	}	

	HANDLE filemapping = CreateFileMapping(filehandle, NULL, PAGE_READONLY, 0, 0, "bglfile");
	DWORD err = GetLastError();
	if ((filemapping != NULL) && (err == ERROR_ALREADY_EXISTS)) {
		sprintf(buffer, "File mapping failed, object already exists\n, file: %s", filename);
		AfxMessageBox(buffer);
		CloseHandle(filemapping);
		CloseHandle(filehandle);
		return(3);
	} else if (filemapping == NULL) {
		sprintf(buffer, "File mapping failed, error code %d, file: %s\n", err, filename);
		AfxMessageBox(buffer);
		CloseHandle(filehandle);
		return(3);
	}	

	bgl = (char *) MapViewOfFile(filemapping, FILE_MAP_READ, 0, 0, 0); 
	if (bgl == NULL) {
		CloseHandle(filemapping);
		CloseHandle(filehandle);
		sprintf(buffer, "Could not map file: %s\nMaybe your virtual memory setting is too low", filename);
		AfxMessageBox(buffer);
		return(4);
	}

  	int sect20 = *((int *) &bgl[118]); 
	latLon l;
	CString route;
	if ((sect20 == 0x87654321) && (includeAFDinfo||!searchingBGLFiles)) {
		int descOffset = *((UNALIGNED int *) &bgl[0xA4]);
		if (descOffset != 0) {
			int chunk = 0;
			while(*((UNALIGNED int *) &bgl[descOffset+chunk]) == 0x00000001) {
				int chunkSize = *((UNALIGNED int *) &bgl[descOffset+chunk + 4]);
				int type = *((UNALIGNED int *) &bgl[descOffset+chunk + 8]);
				if (type == 6) { // waypoint
					int subChunk = descOffset+chunk + 16;
					while (0x00010008 == *((UNALIGNED int *) &bgl[subChunk])) {
						int subChunkSize = *((UNALIGNED int *) &bgl[subChunk + 4]);
						int lati = *((UNALIGNED int *) &bgl[subChunk + 12]); 
						int loni = *((UNALIGNED int *) &bgl[subChunk + 20]); 
						double lat = ((double)lati) / degToMeters;
						double lon = ((double)loni) / 0x7fffffff * 180;
						// use 8 char ids instead of 6 (duplicate preventing prefix)
						char name[8];
						strncpy(name, &bgl[subChunk + 28], 7);
						name[7]=0x00;
						char *search;
						while (NULL != (search=strchr(name, ' ')))
							*search = '_';
						ASSERT(strlen(name) > 2);
						ASSERT((lat != 0.0) || (lon != 0.0));
						l.lat = lat;
						l.lon = lon;
						waypoints[CString(name)] = l;
						subChunk += subChunkSize + 8;
					}
				} else if (type == 7) { // victor airway
					int subChunk = descOffset+chunk + 16;
					if (0x00010006 == *((UNALIGNED int *) &bgl[subChunk])) {
						int nameSize = *((UNALIGNED int *) &bgl[subChunk + 4]);
						char awyName[20];
						strncpy(awyName, &bgl[subChunk + 8], nameSize);
						awyName[nameSize]=0x00;
						char *search;
						while (NULL != (search=strchr(awyName, ' ')))
							*search = '_';
						route=CString(awyName);
						int item = subChunk + 8 + nameSize;
						while (0x00011002 == *((UNALIGNED int *) &bgl[item])) {
							int itemSize = *((UNALIGNED int *) &bgl[item + 4]);
 							// use 8 char ids instead of 6 (duplicate preventing prefix)
							char name[8];
							strncpy(name, &bgl[item + 8], 7);
							name[7]=0x00;
							while (NULL != (search=strchr(name, ' ')))
								*search = '_';
							ASSERT(strlen(name) > 2);
							route = route + CString(" ") + CString(name);
							item += itemSize + 8; 
						}
						routes.Add(route);
					}
				} else if (type == 8) { // jet airway
					int subChunk = descOffset+chunk + 16;
					if (0x00010006 == *((UNALIGNED int *) &bgl[subChunk])) {
						int nameSize = *((UNALIGNED int *) &bgl[subChunk + 4]);
						char awyName[20];
						strncpy(awyName, &bgl[subChunk + 8], nameSize);
						awyName[nameSize]=0x00;
						char *search;
						while (NULL != (search=strchr(awyName, ' ')))
							*search = '_';
						route=CString(awyName);
						int item = subChunk + 8 + nameSize;
						while (0x00011002 == *((UNALIGNED int *) &bgl[item])) {
							int itemSize = *((UNALIGNED int *) &bgl[item + 4]);
							// use 8 char ids instead of 6 (duplicate preventing prefix)
							char name[8];
							strncpy(name, &bgl[item + 8], 7);
							name[7]=0x00;
							while (NULL != (search=strchr(name, ' ')))
								*search = '_';
							ASSERT(strlen(name) > 2);
							route = route + CString(" ") + CString(name);
							item += itemSize + 8; 
						}
					}
					routes.Add(route);
				}
				chunk += chunkSize + 8;
			}
		}
	}

	if (UnmapViewOfFile(bgl) == FALSE)
		AfxMessageBox("UnmapViewOfFile failed");
	if (CloseHandle(filemapping) == FALSE)
		AfxMessageBox("Could not close filemapping handle");
	if (CloseHandle(filehandle) == FALSE)
		AfxMessageBox("Could not close file handle");
	return(0);
}

// return the first word of a line ("" if there are none)
// delete the word from line
CString CNavDoc::parseString(CString &line) {
	line.TrimLeft();
	if (line.GetLength() == 0)
		return CString("");
//	int nextWhite = line.FindOneOf("-");
	int nextWhite = line.FindOneOf(" \t");
	if (nextWhite != -1) { // there is more whiteSpace
		CString result = line.Left(nextWhite);
		line = line.Right(line.GetLength() - nextWhite - 1);
		return result;
	} else { // there is no more whiteSpace
		CString result = line;
		line = CString("");
		return result;
	}
}
