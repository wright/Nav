// FancyMapper.cpp: implementation of the FancyMapper class.
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
#include "Dpstar.h"
#include "Navdoc.h"
#include "flightpl.h" 
#include "Navview.h"
#include "MapOptions.h"

#include <fcntl.h>
#include <io.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// vplot commands, mutilated to work with Windows
double viewlat, viewlong;
double vpscale;
double wScale;
int decimate;
int penDown = FALSE;
int width, height, centerX, centerY;
CDC *vplotDC;
CNavView *pView;
UINT DrawingThread( LPVOID pParam );
visibilityStruct penData;

double my_acos (double x) {
    if (fabs (x) >= 1.)
		return 0.;
    else
		return acos (x);
}

int clip(double x) {  // max GDI coordinate values for Win95, NT doesnt need this
	x = min(x, 32767);
	x = max(x, -32768);
	return (int) x;
}

// Sets a global scale factor for the plotting coordinates. Here just divides all coordinates by "viewscale" before using them.
void vp_scale (double viewscale1, double viewscale2) {
	vpscale = viewscale1 * wScale;
};

// Sets the output plotting color.
void vp_color (BYTE r, BYTE g, BYTE b) {
	if ((penData.r == r) && (penData.g == g) && (penData.b == b))
		return;	// no need to change
	
	// see if we need to force black
	if (pView->forceBlackPrinting) 
		if (vplotDC->IsPrinting()) 
			r=g=b=0;

	CPen *newPen = new CPen(PS_SOLID, 0, RGB(r,g,b));	// install a new pen
	penData.r = r;
	penData.g = g;
	penData.b = b;
	CPen *oldPen = (CPen *) vplotDC->SelectObject(newPen);
	delete oldPen;
};
 
//Lift the pen.... next line-drawing command will be a move.
void vp_penup () {
	penDown = FALSE;
};
 
//  Coordinates are in radians
//	Draw a line to this coordinate, if pen currently down. If pen currently up, move to this coordinate.
void vp_upendn (double x, double y) {
	x *= vpscale;
	y *= -vpscale;
	if (penDown)
		vplotDC->LineTo(clip(x+centerX), clip(y+centerY));
	else {
		vplotDC->MoveTo(clip(x+centerX), clip(y+centerY));
		penDown = TRUE;
	}
};
 
// Move current location to this coordinate.
void vp_move (double x, double y) {
	x *= vpscale;
	y *= -vpscale;
	penDown = TRUE;
	vplotDC->MoveTo(clip(x+centerX), clip(y+centerY));
};
 
// Draw line from current location to this coordinate.
void vp_draw (double x, double y) {
	x *= vpscale;
	y *= -vpscale;
	penDown = TRUE;
	vplotDC->LineTo(clip(x+centerX), clip(y+centerY));
}; 

#define SHORTFLAG 0x4000
#define CBD_MAGIC 0x20770002
#define BIT32 int
#define BIT16 short
#define PI 3.1415926536
#define DEG2RAD (PI / 180.)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FancyMapper::FancyMapper() {
	// check for map files
	mapsDetected = FALSE;
	CString orgDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	CString mapFiles = orgDir + "\\*.cbd";
	_finddata_t fd;	
	int handle = _findfirst(mapFiles, &fd);
	if (handle != -1L)
		mapsDetected = TRUE;
	_findclose(handle);

	showall = FALSE;
	decimateD = 256;
	loadVisibilty();
}

FancyMapper::~FancyMapper() {

}

void FancyMapper::SetOptions(void *view) {
	pView = (CNavView *) view;
	MapOptions d;

	d.m_decimation = decimateD;
	d.m_showall = showall;
	d.v = (visibilityStruct *) &visibility[0].mag;
	if (d.DoModal() == IDOK) {
		decimateD = d.m_decimation;
		showall = d.m_showall;
		saveVisibilty();
		pView->Invalidate(TRUE);
	}
}

void FancyMapper::DrawMaps(CDC * pDC, void *view, int flat, int mag, double centerLat, double centerLon) {
	pView = (CNavView *) view;
    viewlatD   = centerLat;
    viewlongD  = centerLon;
    viewscaleD = mag;
	flatMap = flat;
	vplotDC = pDC;

	CRect clientRect;	// get drawing dimensions
	if (vplotDC->IsPrinting()) {
		width = vplotDC->GetDeviceCaps(HORZRES);
		height = vplotDC->GetDeviceCaps(VERTRES);
		clientRect = CRect(0, 0, width, height);
	} else {
		pView->GetClientRect(clientRect);
		width = clientRect.right;
		height = clientRect.bottom;
	}
	centerX = width / 2;
	centerY = height / 2;

	if (flatMap) { // set scale from normalized units to window
		if (height < width/2)
			wScale = height;
		else 
			wScale = width/2;
	} else
		wScale = PI/2*min(height, width);

	decimate = (int) (decimateD / viewscaleD);

	CPen *currentPen = new CPen(PS_SOLID, 0, RGB(0,0,0));	// install a black pen
	CPen *oldPen = (CPen *) vplotDC->SelectObject(currentPen);
	penData.r = penData.g = penData.b = 0;

	// need to make sure we look in the right directory
	CString orgDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	CString mapFiles = orgDir + "\\*.cbd";
	_finddata_t fd;	// load map files
	int handle = _findfirst(mapFiles, &fd);
	int keepgoing = handle;
	while (!(pView->abort) && (keepgoing != -1L)) { 
		CString mapFile = orgDir + "\\";
		mapFile += fd.name;
		Load(mapFile.GetBuffer(1));
		mapFile.ReleaseBuffer();
		keepgoing = _findnext(handle, &fd);
	}
	_findclose(handle);

	currentPen = (CPen *) pDC->SelectObject(oldPen);	// restore original pen
	delete currentPen;
}

// project lat & lon (in radians) 
int FancyMapper::project (double lati, double longi) {
	double          x, y, z, xx, yy, zz;
	static double   xlast = 0., ylast = 0;

    if (!flatMap) {
		x = sin (longi - viewlong) * cos (lati);
		y = cos (longi - viewlong) * cos (lati);
		z = sin (lati);
		yy =  y * cos (viewlat) + z * sin (viewlat);
		zz = -y * sin (viewlat) + z * cos (viewlat);
		xx = x;
		if (yy < 0.) {	// far side of sphere
			vp_penup ();
			return FALSE;
		}
		vp_upendn (xx, zz);
	} else {
		x = longi - viewlong;
		y = lati - viewlat;
		if (fabs (ylast - y) > (90. * DEG2RAD))  // wrap over edge
			vp_penup ();
		if (fabs (xlast - x) > (180. * DEG2RAD)) // wrap over edge
			vp_penup ();
		xlast = x;
		ylast = y;
		vp_upendn (x, y);
    }
	return TRUE;
}

void FancyMapper::saveVisibilty() {
	FILE *fp;
	CString orgDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	CString fileName = orgDir + "\\Nav.vis";
	fp  = fopen(fileName, "w" );
	fprintf(fp, "%d\t%d\t%d\n", decimateD, flatMap, showall);
	for (int i=0; i< 64; i++) 
		fprintf(fp, "%d\t%d\t%d\t%d\n", visibility[i].mag, (int)visibility[i].r, (int)visibility[i].g, (int)visibility[i].b );
	fclose(fp);
}

void FancyMapper::loadVisibilty() {
	FILE *fp;
	CString orgDir = ((CNavApp *) AfxGetApp())->GetNavDirectory();
	CString fileName = orgDir + "\\Nav.vis";
	if( (fp  = fopen(fileName, "r" )) == NULL ) {
		AfxMessageBox("Nav.vis file not found, using defaults");
		for (int i=0; i< 64; i++) {
			visibility[i].mag = i%16;
			visibility[i].r = 0;
			visibility[i].g = 0;
			visibility[i].b = 0;
		}
		saveVisibilty();
	} else {
		int d, f, s, m, r, g, b;
		fscanf(fp, "%d\t%d\t%d\n", &d, &f, &s);
		decimateD = d;
		flatMap = f;
		showall = s;
		for (int i=0; i< 64; i++) {
			fscanf(fp, "%d\t%d\t%d\t%d\n", &m, &r, &g , &b);
			visibility[i].mag = m;
			visibility[i].r = r;
			visibility[i].g = g;
			visibility[i].b = b;
		}
		fclose(fp);
	}
}

int FancyMapper::Load(char * fname) {
	int     segcount, idx, idy, olt, oln, j, k, jstroke, iseg, filetype;
	char    string2[80], *sptr, buf[100], *file;
	BIT32   i32;
	BIT16   *segbuf;
	double  x, y, z, x1, x2, xc, y1, y2, yc, z1, z2, zc;
	double  dist, cent, clipdist;
	double  longi, lati;

	struct cbdhead {
		BIT32 magic;	/* Magic number */
		BIT32 dictaddr;	/* Offset of segment dictionary in file */
		BIT32 segcount;	/* Number of segments in file */
		BIT32 segsize;	/* Size of segment dictionary */
		BIT32 segmax;	/* Size of largest segment's strokes, /2 */
		BIT32 fill[(40 / sizeof (BIT32)) - 5];	/* Filler */
	} *header;

	struct segdict {
		BIT32           segid;
		BIT32           maxlat, minlat, maxlong, minlong;
		BIT32           absaddr;
		BIT16           nbytes;
		BIT16           rank;
	} *sd;

	struct segment {
		BIT32           orgx, orgy;
		BIT32           id;
		BIT16           nstrokes;
		BIT16           dummy;
	} *sb;

    viewlat = viewlatD * DEG2RAD;
    viewlong = viewlongD * DEG2RAD;
	vp_scale (viewscaleD / (180. * DEG2RAD), viewscaleD / (180. * DEG2RAD));

    clipdist = min(1, sqrt(width*width + height*height) / (min(width, height) * viewscaleD));
	clipdist = asin(clipdist);

    vp_penup ();
	vp_color (0, 0, 0);

    if (!flatMap) {
		for (j = 0; j <= 360 * 10; j++) 
			vp_upendn (cos (j * DEG2RAD / 10.), sin (j * DEG2RAD / 10.));
    } else {
		vp_move (-(180. * DEG2RAD)-viewlong, -(90. * DEG2RAD)-viewlat);
		vp_draw ( (180. * DEG2RAD)-viewlong, -(90. * DEG2RAD)-viewlat);
		vp_draw ( (180. * DEG2RAD)-viewlong,  (90. * DEG2RAD)-viewlat);
		vp_draw (-(180. * DEG2RAD)-viewlong,  (90. * DEG2RAD)-viewlat);
		vp_draw (-(180. * DEG2RAD)-viewlong, -(90. * DEG2RAD)-viewlat);
    }

	sptr = strrchr (fname, '.');
	if (sptr == NULL)
		return 1;
	if (_stricmp (sptr, ".cbd") != 0)
		return 1;

	strcpy (string2, sptr - 3);
	string2[3] = '\0';

	if (_stricmp (string2, "bdy") == 0)
		filetype = 0;
	else if (_stricmp (string2, "pby") == 0)
		filetype = 16;
	else if (_stricmp (string2, "cil") == 0)
		filetype = 32;
	else if (_stricmp (string2, "riv") == 0)
		filetype = 48;
	else
		return 1;

	// map file to memory
	HANDLE filehandle = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (filehandle == INVALID_HANDLE_VALUE) {
		sprintf(buf, "Could not open %s\n", fname);
		AfxMessageBox(buf);
		return(2);
	}	

	HANDLE filemapping = CreateFileMapping(filehandle, NULL, PAGE_READONLY, 0, 0, "mapfile");
	if (filemapping == NULL) {
		sprintf(buf, "File mapping failed\n");
		AfxMessageBox(buf);
		CloseHandle(filehandle);
		return(3);
	}	

	file = (char *) MapViewOfFile(filemapping, FILE_MAP_READ, 0, 0, 0); 
	if (file == NULL) {
		CloseHandle(filemapping);
		CloseHandle(filehandle);
		AfxMessageBox("Could not map file");
		return(4);
	}

	// Read in the file header, check it for the correct magic number, and learn the address of the segment dictionary
	header = (cbdhead *) file;
	if (header->magic != CBD_MAGIC) {
		sprintf (buf, "File has bad magic number %d != %d\n", header->magic, CBD_MAGIC);
		AfxMessageBox(buf);
		goto unmap;
	}

	// Go read in the segment dictionary (it's at the end of the file) 
	sd = (struct segdict *) &file[header->dictaddr];
	segcount = header->segcount;

	sd--;
	for (iseg = 1; iseg<=segcount; iseg++) {
		sd++;
		if (pView->abort) 
			goto unmap;

		if ((viewscaleD < visibility[filetype + sd->rank].mag) && !showall)
			continue;

		if (!flatMap) { // check to see if this segment is in view
			longi = sd->maxlong * DEG2RAD / 3600.;
			lati = sd->maxlat * DEG2RAD / 3600;

			x = sin (longi - viewlong) * cos (lati);
			y = cos (longi - viewlong) * cos (lati);
			z = sin (lati);

			y1 = y * cos (viewlat) + z * sin (viewlat);
			z1 = -y * sin (viewlat) + z * cos (viewlat);
			x1 = x;

			longi = sd->minlong * DEG2RAD / 3600;
			lati = sd->minlat * DEG2RAD / 3600;
			x = sin (longi - viewlong) * cos (lati);
			y = cos (longi - viewlong) * cos (lati);
			z = sin (lati);

			y2 = y * cos (viewlat) + z * sin (viewlat);
			z2 = -y * sin (viewlat) + z * cos (viewlat);
			x2 = x;

			xc = (x1 + x2) / 2.;
			yc = (y1 + y2) / 2.;
			zc = (z1 + z2) / 2.;
			x = sqrt (xc * xc + yc * yc + zc * zc);
			xc /= x;
			yc /= x;
			zc /= x;

			dist = my_acos (xc * x1 + yc * y1 + zc * z1);
			cent = my_acos (yc);

			if ((cent - dist) > clipdist)
				continue;
		} else {
			double minx = -centerX / vpscale  + viewlong;	
			if (minx > sd->maxlong * DEG2RAD / 3600)
				continue;
			double maxx = (width-centerX) / vpscale  + viewlong;	
			if (maxx < sd->minlong * DEG2RAD / 3600)
				continue;
			double miny = -centerY / -vpscale + viewlat;
			if (miny < sd->minlat * DEG2RAD / 3600)
				continue;
			double maxy = (height-centerY) / -vpscale + viewlat;
			if (maxy > sd->maxlat * DEG2RAD / 3600)
				continue;
		}

		vp_color (visibility[filetype+sd->rank].r, visibility[filetype+sd->rank].g, visibility[filetype+sd->rank].b);

		sb = (struct segment *) &file[sd->absaddr];
		segbuf = (BIT16 *) sb + 8;

		k = 0;
		oln = sb->orgx;
		olt = sb->orgy;

		vp_penup ();
		project (DEG2RAD * olt / 3600., DEG2RAD * oln / 3600.);

		for (jstroke = 1; jstroke <= sb->nstrokes; jstroke++) {
			if (pView->abort) 
				goto unmap;

			if (segbuf[k] & SHORTFLAG) { // Flag bit on: unpack a 16-bit field into dx and dy 
				i32 = segbuf[k++];
				if (i32 > 0)
					i32 &= ~SHORTFLAG;
				idy = i32 & 0xFF;
				if (idy & 0x80)
					idy |= ~0xFF;		// extend sign 
				idx = i32 >> 8;
				if (idx & 0x80)
					idx |= ~0xBF;		// extend sign 
			} else {					// Flag bit off: take dx and dy from 32-bit fields. 
				idx = segbuf[k++];
				if (idx < 0)
					idx |= SHORTFLAG;
				idx = (idx << 16) | (unsigned short) segbuf[k];
				k++;

				idy = segbuf[k];
				k++;
				if (idy < 0)
					idy |= SHORTFLAG;
				idy = (idy << 16) | segbuf[k];
				k++;
			}
			olt = olt + idy;
			oln = oln + idx;

			if (decimate > 1) 
				if (jstroke%decimate != 1)
					if (jstroke != sb->nstrokes)
						if (!showall)
							continue;

			project (DEG2RAD * olt / 3600., DEG2RAD * oln / 3600.);
		}
	}

unmap:
	// unmap file and close
	if (UnmapViewOfFile(file) == FALSE)
		AfxMessageBox("UnmapViewOfFile failed");
	if (CloseHandle(filemapping) == FALSE)
		AfxMessageBox("Could not close filemapping handle");
	if (CloseHandle(filehandle) == FALSE)
		AfxMessageBox("Could not close file handle");

	return 0;
}


