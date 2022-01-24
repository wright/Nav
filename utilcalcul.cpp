// utilcalcul.cpp
// Copyright Jean-Yves LEBLEU <jeanyves.lebleu@hol.fr>
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
#include "utilcalcul.h"
#include <math.h>
#define pi			 3.1415946536
#define degToRad     (pi / 180.0)

void LatToString(double lat, CString& strlat)
{
	char diry;

	if (lat < 0.) 
		diry = 'S'; 
	else 
		diry = 'N';
	double cyy = fabs(lat);
	int cyd = (int) (cyy);
	int cym = (int) ((cyy-cyd) * 60);
	double cys = (double) (((cyy-cyd)*60-cym)*60);

	strlat.Format("%c%d°%d'%2.2f\"",diry,cyd,cym,cys);
	//strlat.Format("%c %d% %d %2.2f",diry,cyd,cym,cys);

}

void LongToString(double longit, CString& strlong)
{
	char dirx;

	if (longit < 0.) 
		dirx = 'W'; 
	else 
		dirx = 'E';

	double cxx = fabs(longit);
	int cxd = (int) (cxx);
	int cxm = (int) ((cxx-cxd) * 60);
	double cxs = (double) (((cxx-cxd)*60-cxm)*60);

	strlong.Format("%c%d°%d'%2.2f\"",dirx,cxd,cxm,cxs);

}

//
// warning use a distance in nautical miles
// good for distances < 200 nm
//
void DistHeadToCoord(double distance,double heading, 
								double& latitude, double& longitude)
{
	double deltaLat;
	distance = distance;
	deltaLat = distance * sin((90-heading)* degToRad);

	longitude = longitude + (distance * cos((90-heading)* degToRad))/(cos((latitude + deltaLat/120)*degToRad))/60;
	latitude = latitude + (deltaLat/60);

}
