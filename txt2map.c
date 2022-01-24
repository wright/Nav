/*
    Copyright (C) 1997 Ted Wright 
    NASA Business: wright@lerc.nasa.gov, Remainder: wright@en.com

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

#include <stdio.h>

int main(int argc, char *argv[]) {
	double latf, lonf;
	double convertLat = 0x80000000/90.0; 
	double convertLon = 0x80000000/180.0; 
	int lat, lon;
	FILE *in, *out;
	if (argc != 2) {
		printf("Need a filename\n");
		return(1);
	}
	if ((in = fopen(argv[1], "r")) == NULL) {
		printf("Error opening input file");
		return(2);
	}
	if ((out = fopen("nav.map", "wb")) == NULL) {
		printf("Error opening output file");
		return(3);
	}
	while (fscanf(in, " %lf %lf", &latf, &lonf) != EOF) {
		lat = (int) (latf * convertLat);
		lon = (int) (lonf * convertLon);
		fwrite(&lat, 1, 4, out);
		fwrite(&lon, 1, 4, out);
	}
	fclose(out);
	fclose(in);
	printf("Results written to nav.map\n");
	return(0);
}
