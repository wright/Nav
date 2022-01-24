// DpStar.cpp: implementation of the DpStar class.
//
//////////////////////////////////////////////////////////////////////
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
#include "DpStar.h"
#include <stdio.h>
#include "io.h"
#include "xmlparse.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// return TRUE if fileName1 was modified more recently than fileName2
int fileNewer(CString fileName1, CString fileName2) {
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DpStar::DpStar() {

}

DpStar::~DpStar() {

}

DpStar::DpStar(DpStar &rhs) {
	airports.RemoveAll();
	int size = rhs.airports.GetSize();
	for (int i=0; i<size; i++)
		addAirport(rhs.airports[i]);				
}

struct xmlState {
	int level;
	int error; // 0 = no error, 1 = complain, 2 = ignore
	DpStarAirport *a;
	DpStarRoute *d;
	DpStar *t;
};
 
void fixupError(xmlState *state) {
	if (state->a != NULL) 
		delete state->a;
	state->a = NULL;
	if (state->d != NULL) 
		delete state->d;
	state->d = NULL;
}

void startElement(void *userData, const char *name, const char **atts) {
	xmlState *state = (xmlState *) userData;
	
	if (state->error == 1) { // clean up after error
		fixupError(state);
	} else if ((state->level == 0) && (CString(name) != "DPs-STARs")) {
		state->error = 2; // wrong type of XML file, ignore the file
		fixupError(state);
	} else if ((state->level == 0) && (CString(name) == "DPs-STARs")) {
		; // do nothing
	} else if (CString(name) == "Airport") {
		CString icao = (char *) atts[1];
		state->a = new DpStarAirport(icao);
	} else if (CString(name) == "DP") {
		CString name = (char *) atts[1];
		state->d = new DpStarRoute(name);
	} else if (CString(name) == "STAR") {
		CString name = (char *) atts[1];
		state->d = new DpStarRoute(name);
	} else if (CString(name) == "Waypoint") {
		CString name = (char *) atts[1];
		double lat, lon;
		sscanf(atts[3], "%lf", &lat);
		sscanf(atts[5], "%lf", &lon);
		state->d->addWaypoint(DpStarWaypoint(name, lat, lon));
	} else { // error
		state->error = 1;
		fixupError(state);
	}
	state->level = state->level+1;
}

void endElement(void *userData, const char *name) {
	xmlState *state = (xmlState *) userData;
	state->level = state->level-1;

	if (state->error == 1) { // clean up after error
		fixupError(state);
	} else if (CString(name) == "DPs-STARs") {
		; // do nothing
	} else if (CString(name) == "Airport") {
		state->t->addAirport(*(state->a));
		delete state->a;
	} else if (CString(name) == "DP") {
		state->a->addDP(*(state->d));
		delete state->d;
	} else if (CString(name) == "STAR") {
		state->a->addSTAR(*(state->d));
		delete state->d;
	} else if (CString(name) == "Waypoint") {
		; // do nothing
	} else { // error
		state->error = 1;
		fixupError(state);
	}
}

void DpStar::readFile(CString filename) {
	char oldCurrentDir[260]; // set default directory
	GetCurrentDirectory(260, oldCurrentDir);
	SetCurrentDirectory(((CNavApp*)AfxGetApp())->GetNavDirectory());

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {// silently fail
		SetCurrentDirectory(oldCurrentDir);
		return;
	}
	DpStar temp; // use temp copy in case there are input errors

	char buf[BUFSIZ];
	XML_Parser parser = XML_ParserCreate(NULL);
	int done;
	xmlState state;
	state.level = 0;
	state.error = 0;
	state.a = NULL;
	state.d = NULL;
	state.t = &temp;
	XML_SetUserData(parser, &state);
	XML_SetElementHandler(parser, startElement, endElement);
	do {
		size_t len = fread(buf, 1, sizeof(buf), fp);
		done = len < sizeof(buf);
		if (!XML_Parse(parser, buf, len, done)) {
			state.error = 1;
			fixupError(&state);
			break;
		}
	} while (!done && !state.error);

	fclose(fp);

	if (state.error == 1) {
		char err[500];
		if (XML_ErrorString(XML_GetErrorCode(parser)) != NULL)
			sprintf(err, "Error parsing file %s\n%s at line %d\n", filename, XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser));
		else
			sprintf(err, "Error parsing file %s\non line %d, File ignored\n", filename, XML_GetCurrentLineNumber(parser));
		AfxMessageBox(err);
	}

	XML_ParserFree(parser);

	if (state.error == 0) {
		int size = temp.airports.GetSize();
		for (int i=0; i<size; i++)
			addAirport(temp.airports[i]);				
	}

	SetCurrentDirectory(oldCurrentDir);
}

void writeCString(CString s, FILE *fp) {
	int len = s.GetLength();
	fwrite(&len, sizeof(int), 1, fp);
	fwrite(s.GetBuffer(1), 1, len, fp);
	s.ReleaseBuffer();
}

CString readCString(FILE *fp) {
	int len;
	char buf[256];
	fread(&len, sizeof(int), 1, fp);
	fread(buf, 1, len, fp);
	return CString(buf, len);
}

void DpStar::readCompiledFile(CString filename) {
	FILE *fp = fopen(filename, "rb");

	int asize, rsize, wsize;
	double lat, lon;
	fread(&asize, sizeof(int), 1, fp); // number of airports
	for (int i=0; i<asize; i++) {
		CString icao = readCString(fp); // airport icao
		DpStarAirport a(icao);
		fread(&rsize, sizeof(int), 1, fp); // number of DPs
		for (int j=0; j<rsize; j++) {
			CString rname = readCString(fp); // name of DP
			DpStarRoute r(rname);
			fread(&wsize, sizeof(int), 1, fp); // number of waypoints
			for (int w=0; w<wsize; w++) {
				CString wname = readCString(fp);    // name of waypoint
				fread(&lat, sizeof(double), 1, fp); // latitude
				fread(&lon, sizeof(double), 1, fp); // latitude
				r.addWaypoint(DpStarWaypoint(wname, lat, lon));
			}
			a.addDP(r);
		}
		fread(&rsize, sizeof(int), 1, fp); // number of STARs
		for (j=0; j<rsize; j++) {
			CString rname = readCString(fp); // name of STAR
			DpStarRoute r(rname);
			fread(&wsize, sizeof(int), 1, fp); // number of waypoints
			for (int w=0; w<wsize; w++) {
				CString wname = readCString(fp);    // name of waypoint
				fread(&lat, sizeof(double), 1, fp); // latitude
				fread(&lon, sizeof(double), 1, fp); // latitude
				r.addWaypoint(DpStarWaypoint(wname, lat, lon));
			}
			a.addSTAR(r);
		}
		addAirport(a);
	}

	fclose(fp);
}

void DpStar::writeCompiledFile(CString filename) {
	FILE *fp = fopen(filename, "wb");
	DpStarRoute *r;
	DpStarWaypoint *w;

	int asize = airports.GetSize();
	fwrite(&asize, sizeof(int), 1, fp); // number of airports
	for (int i=0; i<asize; i++) {
		writeCString(airports[i].icao, fp); //  airport icao
		int rsize = airports[i].DPs.GetSize();
		fwrite(&rsize, sizeof(int), 1, fp); // number of DPs
		r = airports[i].firstDP();
		while (r != NULL) {
			writeCString(r->name, fp); // name of DP
			int wsize = r->waypoints.GetSize();
			fwrite(&wsize, sizeof(int), 1, fp); // number of waypoints
			w = r->firstWaypoint();
			while (w != NULL) {
				writeCString(w->name, fp); // name of waypoint
				fwrite(&w->latitude, sizeof(double), 1, fp);  // latitude
				fwrite(&w->longitude, sizeof(double), 1, fp); // longitude
				w = r->nextWaypoint();
			}
			r = airports[i].nextDP();
		}
		rsize = airports[i].STARs.GetSize();
		fwrite(&rsize, sizeof(int), 1, fp); // number of STARs
		r = airports[i].firstSTAR();
		while (r != NULL) {
			writeCString(r->name, fp); // name of STAR
			int wsize = r->waypoints.GetSize();
			fwrite(&wsize, sizeof(int), 1, fp); // number of waypoints
			w = r->firstWaypoint();
			while (w != NULL) {
				writeCString(w->name, fp); // name of waypoint
				fwrite(&w->latitude, sizeof(double), 1, fp); // latitude
				fwrite(&w->longitude, sizeof(double), 1, fp); // longitude
				w = r->nextWaypoint();
			}
			r = airports[i].nextSTAR();
		}
	}

	fclose(fp);
}

void DpStar::readAllFiles() {
	char oldCurrentDir[260]; // set default directory
	GetCurrentDirectory(260, oldCurrentDir);
	SetCurrentDirectory(((CNavApp*)AfxGetApp())->GetNavDirectory());

	airports.RemoveAll(); // start with clean slate

	if (fileNewer("dpstar.xml.c", "dpstar.xml")) 
		readCompiledFile("dpstar.xml.c"); // read compiled version instead
	else {
		readFile("dpstar.xml"); // read base DP and Star file
		writeCompiledFile("dpstar.xml.c");// write compiled version
	}

	// then read and merge in all others
	_finddata_t fd;
	int handle = _findfirst("*.xml", &fd);
	int keepgoing = handle;
	while (keepgoing != -1L) {
		CString file = fd.name;
		file.MakeLower();
		if (file != "dpstar.xml") 
			readFile(fd.name);
		keepgoing = _findnext(handle, &fd);
	}
	_findclose(handle);

	SetCurrentDirectory(oldCurrentDir);
}

void DpStar::writeFile(CString filename) {
	char oldCurrentDir[260]; // set default directory
	GetCurrentDirectory(260, oldCurrentDir);
	SetCurrentDirectory(((CNavApp*)AfxGetApp())->GetNavDirectory());

	FILE *dump = fopen(filename, "w");
	DpStarAirport a;
	DpStarRoute *r;
	DpStarWaypoint *w;

	// header
	fprintf(dump, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
	fprintf(dump, "<!DOCTYPE DPs-STARs [\n");
	fprintf(dump, "<!ELEMENT DPs-STARs (Airport*)>\n");
	fprintf(dump, "<!ELEMENT Airport (DP*,STAR*)>\n");
	fprintf(dump, "<!ATTLIST Airport icao CDATA #REQUIRED>\n");
	fprintf(dump, "<!ELEMENT DP (Waypoint+)>\n");
	fprintf(dump, "<!ATTLIST DP name CDATA #REQUIRED>\n");
	fprintf(dump, "<!ELEMENT STAR (Waypoint+)>\n");
	fprintf(dump, "<!ATTLIST STAR name CDATA #REQUIRED>\n");
	fprintf(dump, "<!ELEMENT Waypoint EMPTY>\n");
	fprintf(dump, "<!ATTLIST Waypoint name CDATA #REQUIRED lat CDATA #REQUIRED lon CDATA #REQUIRED>\n");
	fprintf(dump, "]>\n");
	fprintf(dump, "<DPs-STARs>\n");

	for (int i=0; i<airports.GetSize(); i++) {
		a = airports[i];
		fprintf(dump, "\t<Airport icao=\"%s\">\n", a.icao);
		r = a.firstDP();
		while (r != NULL) {
	 		fprintf(dump, "\t\t<DP name=\"%s\">\n", r->name);
			w = r->firstWaypoint();
			while (w != NULL) {
		 		fprintf(dump, "\t\t\t<Waypoint name=\"%s\" lat=\"%lf\" lon=\"%lf\"/>\n", w->name, w->latitude, w->longitude);
				w = r->nextWaypoint();
			}
	 		fprintf(dump, "\t\t</DP>\n");
			r = a.nextDP();
		}
		r = a.firstSTAR();
		while (r != NULL) {
	 		fprintf(dump, "\t\t<STAR name=\"%s\">\n", r->name);
			w = r->firstWaypoint();
			while (w != NULL) {
		 		fprintf(dump, "\t\t\t<Waypoint name=\"%s\" lat=\"%lf\" lon=\"%lf\"/>\n", w->name, w->latitude, w->longitude);
				w = r->nextWaypoint();
			}
	 		fprintf(dump, "\t\t</STAR>\n");
			r = a.nextSTAR();
		}
		fprintf(dump, "\t</Airport>\n");
	}

	// trailer
	fprintf(dump, "</DPs-STARs>\n");
	fclose(dump);

	SetCurrentDirectory(oldCurrentDir);
}

void DpStar::addAirport(DpStarAirport A) {
	DpStarAirport* existing = lookupAirport(A.icao);
	if (existing == NULL)
		airports.Add(A);
	else { // merge airports
		DpStarRoute *r;
		r = A.firstDP();
		while (r != NULL) {
			existing->addDP(*r);
			r = A.nextDP();
		}
		r = A.firstSTAR();
		while (r != NULL) {
			existing->addSTAR(*r);
			r = A.nextSTAR();
		}
	}
}

void DpStarAirport::addDP(DpStarRoute R) {
	DpStarRoute* existing = lookupDP(R.name);
	if (existing == NULL) 
		DPs.Add(R);
	else
		*existing = R; // replace duplicate
}

void DpStarAirport::addSTAR(DpStarRoute R) {
	DpStarRoute* existing = lookupSTAR(R.name);
	if (existing == NULL) 
		STARs.Add(R);
	else
		*existing = R; // replace duplicate
}

void DpStarRoute::addWaypoint(DpStarWaypoint W) {
	waypoints.Add(W);
}

DpStarAirport::DpStarAirport(DpStarAirport &rhs) {
	icao = rhs.icao;
	lastDP = rhs.lastDP;
	lastSTAR = rhs.lastSTAR;
	DPs.RemoveAll();
	int size = rhs.DPs.GetSize();
	for (int i=0; i<size; i++)
		DPs.Add(rhs.DPs[i]);				
	STARs.RemoveAll();
	size = rhs.STARs.GetSize();
	for (i=0; i<size; i++)
		STARs.Add(rhs.STARs[i]);				
}

DpStarAirport &DpStarAirport::operator=(DpStarAirport &rhs) {
	icao = rhs.icao;
	lastDP = rhs.lastDP;
	lastSTAR = rhs.lastSTAR;
	DPs.RemoveAll();
	int size = rhs.DPs.GetSize();
	for (int i=0; i<size; i++)
		DPs.Add(rhs.DPs[i]);				
	STARs.RemoveAll();
	size = rhs.STARs.GetSize();
	for (i=0; i<size; i++)
		STARs.Add(rhs.STARs[i]);				
    return *this;  // Assignment operator returns left side.
}

DpStarAirport* DpStar::lookupAirport(CString icao) {
	icao.TrimRight();
	icao.MakeLower();
	int size = airports.GetSize();
	CString a;
	for (int i=0; i<size; i++) {
		a = airports[i].icao;
		a.TrimRight();
		a.MakeLower();
		if (a == icao)
			return &airports[i];
	}
	return NULL;
}

DpStarRoute* DpStarAirport::lookupDP(CString name){
	name.TrimRight();
	name.MakeLower();
	int size = DPs.GetSize();
	CString a;
	for (int i=0; i<size; i++)  {
		a = DPs[i].name;
		a.TrimRight();
		a.MakeLower();
		if (a == name)
			return &DPs[i];
	}
	return NULL;
}

DpStarRoute* DpStarAirport::lookupSTAR(CString name){
	name.TrimRight();
	name.MakeLower();
	int size = STARs.GetSize();
	CString a;
	for (int i=0; i<size; i++) {
		a = STARs[i].name;
		a.TrimRight();
		a.MakeLower();
		if (a == name)
			return &STARs[i];
	}
	return NULL;
}

DpStarRoute* DpStarAirport::firstDP() {
	lastDP = 0;
	if (DPs.GetSize() == 0)
		return NULL;
	else
		return &DPs[0];
	return NULL;
}

DpStarRoute* DpStarAirport::nextDP() {
	lastDP++;
	if (lastDP >= DPs.GetSize())
		return NULL;
	else
		return &DPs[lastDP];
}

DpStarRoute* DpStarAirport::firstSTAR() {
	lastSTAR = 0;
	if (STARs.GetSize() == 0)
		return NULL;
	else
		return &STARs[0];
	return NULL;
}

DpStarRoute* DpStarAirport::nextSTAR() {
	lastSTAR++;
	if (lastSTAR >= STARs.GetSize())
		return NULL;
	else
		return &STARs[lastSTAR];
}

void DpStarAirport::listSTARs(CArray<CString, CString> &result) {
	DpStarRoute *r = firstSTAR();
	while (r != NULL) {
		result.Add(r->name);
		r = nextSTAR();
	}
}

void DpStarAirport::listDPs(CArray<CString, CString> &result) {
	DpStarRoute *r = firstDP();
	while (r != NULL) {
		result.Add(r->name);
		r = nextDP();
	}
}

DpStarRoute::DpStarRoute(DpStarRoute &rhs) {
	lastWaypoint = rhs.lastWaypoint;
	name = rhs.name;
	waypoints.RemoveAll();
	int size = rhs.waypoints.GetSize();
	for (int i=0; i<size; i++)
		waypoints.Add(rhs.waypoints[i]);				
}

DpStarRoute &DpStarRoute::operator=(DpStarRoute &rhs) {
	lastWaypoint = rhs.lastWaypoint;
	name = rhs.name;
	waypoints.RemoveAll();
	int size = rhs.waypoints.GetSize();
	for (int i=0; i<size; i++)
		waypoints.Add(rhs.waypoints[i]);				
    return *this;  // Assignment operator returns left side.
}

DpStarWaypoint* DpStarRoute::firstWaypoint() {
	lastWaypoint = 0;
	if (waypoints.GetSize == 0)
		return NULL;
	else
		return &waypoints[0];
}

DpStarWaypoint* DpStarRoute::nextWaypoint() {
	lastWaypoint++;
	if (lastWaypoint >= waypoints.GetSize())
		return NULL;
	else
		return &waypoints[lastWaypoint];
}

