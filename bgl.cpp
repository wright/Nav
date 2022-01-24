#include "stdio.h"
#include "string.h"
#include "windows.h"
#include "math.h"

// read 3 bytes as a signed int
int read3bytes(char *b) {
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
double readDM(char *b) {
	unsigned char dec = (unsigned char) b[0];
	int lat = read3bytes(&b[1]);
	if (lat > 0)
		return( (double) lat + dec / 256.0 );
	else
		return( (double) lat - dec / 256.0 );
}

void printDDrange(double maxR, double minR) {
	double ddeg = maxR;
	int deg = (int) ddeg;
	int min = (int) ddeg / 60;
	double sec = (ddeg - deg - min * 60) /3600; // round to zero
	if ((sec < 0.00001) && (sec > -0.00001))
		sec = 0.0;
	printf("%d:%d:%f", deg, min, sec); 
	ddeg = minR;
	deg = (int) ddeg;
	min = (int) ddeg / 60;
	sec = (ddeg - deg - min * 60) /3600;  // round to zero
	if ((sec < 0.00001) && (sec > -0.00001))
		sec = 0.0;
	printf(" to %d:%d:%f\n", deg, min, sec); 
}

int main(int argc, char **argv) {
	HANDLE filehandle, filemapping;
	char *bgl;
	int count = 0;

	if (argc != 2) {
		printf("Usage: bgl scenery.bgl\n");
		return(1);
	}

	filehandle = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
	if (filehandle == INVALID_HANDLE_VALUE) {
		printf("Could not open %s\n", argv[1]);
		return(2);
	}	

	filemapping = CreateFileMapping(filehandle, NULL, PAGE_READONLY, 0, 0, "bglfile");
	if (filemapping == NULL) {
		printf("File mapping failed\n");
		return(3);
	}	

	bgl = (char *) MapViewOfFile(filemapping, FILE_MAP_READ, 0, 0, 0);
	// finished mapping file to memory


	unsigned short sceneryID = *((unsigned short *) &bgl[0]);
	
	// lat in decimal degrees 
	double maxNorth = readDM(&bgl[2]) * 90 / 10001750; 
	double minNorth = readDM(&bgl[6]) * 90 / 10001750;
	
	printf("latitude range  ");
	printDDrange(maxNorth, minNorth);
	 
	// lon in decimal degrees 
	double maxEast =  (read3bytes(&bgl[11]) / (double) 0x1000000) * 360; 
	double minEast =  (read3bytes(&bgl[15]) / (double) 0x1000000) * 360;

	printf("longitude range ");
	printDDrange(maxEast, minEast);

  	// find file size
	WIN32_FIND_DATA fd;
	FindFirstFile(argv[1], &fd);
	int size = (int) fd.nFileSizeLow; // assume its not more than 32 bits

  	int sect0  = *((int *) &bgl[18]); //used
  	int sect9  = *((int *) &bgl[58]); //used
	int sect10 = *((int *) &bgl[62]);
 	int sect11 = *((int *) &bgl[66]); //used 
  	int sect12 = *((int *) &bgl[70]);
  	int sect13 = *((int *) &bgl[74]);
 	int sect14 = *((int *) &bgl[78]); //used
  	int sect15 = *((int *) &bgl[82]);
 	int sect16 = *((int *) &bgl[102]); //used
  	int sect17 = *((int *) &bgl[106]);
  	int sect18 = *((int *) &bgl[110]);
  	int sect19 = *((int *) &bgl[114]);
  	int sect20 = *((int *) &bgl[118]);
  	int sect21 = *((int *) &bgl[122]);

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
 
/*
	// section 0: VOR and ILS
    short minS0freq = *((short *) &bgl[22]);
    short maxS0freq = *((short *) &bgl[24]);

	if (sect0 != 0)	{
		// for each frequency
		for (int i=0; i<=maxS0freq-minS0freq; i++) {
			if(bgl[sect0+5*i] == 0x01) {
				int sectOffset = *((int *) &bgl[sect0+5*i+1]);
				if (sectOffset != 0) {
					double freq = 108 + (minS0freq + i) * .05;
					int rangeTableOffset = 0;
					// for each latitude range
					while (bgl[sect0 + sectOffset + rangeTableOffset] == 0x15) {
						short minLat = *((short *) &bgl[sect0 + sectOffset + rangeTableOffset+1]);
						short maxLat = *((short *) &bgl[sect0 + sectOffset + rangeTableOffset+3]);
						int vorOffset = *((int *) &bgl[sect0 + sectOffset + rangeTableOffset+5]);
						// for each station in this range
						while ((bgl[sect0 + vorOffset] == 0x04) || (bgl[sect0 + vorOffset] == 0x05)) {
							if (bgl[sect0 + vorOffset] == 0x05)	{
								printf("ILS ");
							} else {
								printf("VOR ");
							}
							char range = *((char *) &bgl[sect0 + vorOffset + 1]);
							short magdev = *((short *) &bgl[sect0 + vorOffset + 2]);
							int lat = read3bytes(&bgl[sect0 + vorOffset + 5]);
							int lon = read3bytes(&bgl[sect0 + vorOffset + 8]);
							short alt = *((short *) &bgl[sect0 + vorOffset + 11]);
							char vorID[6];
							strncpy(vorID, &bgl[sect0 + vorOffset + 15], 5);
							vorID[5] = 0x00;
							char vorName[25];
							strncpy(vorName, &bgl[sect0 + vorOffset + 20], 24);
							vorName[24] = 0x00;

							printf("freq:%3.2f ", freq);
							printf("%s ", vorID);
							printf("%s\n", vorName);
							printf("lat:% 8.4f lon:% 9.4f ", lat * (double) 90.0 / 5000875, lon / (double) 0x1000000 * 360);
							printf("alt:%4.0dm magdev:% 3.0f ", (int) alt, (double) magdev * 360. / 0x10000);
							printf("range:%dkm ", (int) range * 2048 / 1000);
							printf("\n");
							
							// read additional info for ILS
							if (bgl[sect0 + vorOffset] == 0x05)	{
								short approach = *((short *) &bgl[sect0 + vorOffset + 13]);
								int gslat = read3bytes(&bgl[sect0 + vorOffset + 44]);
								int gslon = read3bytes(&bgl[sect0 + vorOffset + 47]);
								short gsalt = *((short *) &bgl[sect0 + vorOffset + 50]);
								
								printf("gslat:% 8.4f gslon:% 9.4f ", gslat * (double) 90.0 / 5000875, gslon / (double) 0x1000000 * 360);
								printf("gsalt:%4.0dm app:% 6.1f\n", (int) gsalt, (double) approach * 360. / 0x10000);
							}

							count++;
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

	// section 14: NDB
	if (sect14 != 0)	{
		int rangeTableOffset = 0;
		// for each latitude range
		while (bgl[sect14 + rangeTableOffset] == 0x15) {
			short minLat = *((short *) &bgl[sect14 + rangeTableOffset+1]);
			short maxLat = *((short *) &bgl[sect14 + rangeTableOffset+3]);
			int ndbOffset = *((int *) &bgl[sect14 + rangeTableOffset+5]);
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
				short alt = *((short *) &bgl[sect14 + ndbOffset + 10]);
				char range = *((char *) &bgl[sect14 + ndbOffset + 12]);
				char ndbID[6];
				strncpy(ndbID, &bgl[sect14 + ndbOffset + 13], 5);
				ndbID[5] = 0x00;
				char ndbName[25];
				strncpy(ndbName, &bgl[sect14 + ndbOffset + 18], 24);
				ndbName[24] = 0x00;

				printf("NDB ");
				printf("lat:% 8.4f lon:% 9.4f ", lat * (double) 90.0 / 5000875, lon / (double) 0x1000000 * 360);
				printf("range:% 4.0fkm freq:% 4.1f ", (double) range * 2048. / 1000., freq);
				printf("%s  %s\n", ndbID, ndbName);

				ndbOffset += 42;
				count++;
			}
			rangeTableOffset += 9;
		}
	}

	// section 11: airports & runways (menu items)
	if (sect11 != 0)	{
		int areaOffset = 0;
		// for each area
		while (bgl[sect11 + areaOffset] == 0x01) {
			short areaRec = *((short *) &bgl[sect11 + areaOffset + 1]);
			int runwayTable = *((int *) &bgl[sect11 + areaOffset + 3]);
			char* areaName = &bgl[sect11 + areaOffset + 7];
			printf("Area name: %s\n", areaName);
			// for each runway in the area
			int runwayOffset = 0;
			while (bgl[sect11 + runwayTable + runwayOffset] == 0x03) {
				short runwayRec = *((short *) &bgl[sect11 + runwayTable + runwayOffset + 1]);

				char* runwayName = &bgl[sect11 + runwayTable + runwayOffset + 27];
				printf("  Runway name: %s\n", runwayName);

				double lat = readDM(&bgl[sect11 + runwayTable + runwayOffset + 3]);
				double lon = readDM(&bgl[sect11 + runwayTable + runwayOffset + 7]);
				double alt = *((int *) &bgl[sect11 + runwayTable + runwayOffset + 11]) / 256;

				lat *= 90.0 / 5000875.0;
				lon *= 360.0 / (double) 0x1000000;

				count++;
				runwayOffset += runwayRec;
			}
			areaOffset += areaRec;
		}
	}

	// section 16: airports
	if (sect16 != 0)	{
		int rangeTableOffset = 0;
		// for each latitude range
		while (bgl[sect16 + rangeTableOffset] == 0x15) {
			short minLat = *((short *) &bgl[sect16 + rangeTableOffset + 1]);
			short maxLat = *((short *) &bgl[sect16 + rangeTableOffset + 3]);
			int areaOffset = *((int *) &bgl[sect16 + rangeTableOffset + 5]);
			// for each area
			while ((bgl[sect16 + areaOffset] > 0x03) && (bgl[sect16 + areaOffset] < 0x08)) {
				char areaRec = *((char *) &bgl[sect16 + areaOffset + 1]);
				// check to see if its an airport record
				if (bgl[sect16 + areaOffset] == 0x07) {
					int lat = read3bytes(&bgl[sect16 + areaOffset + 2]);
					int lon = read3bytes(&bgl[sect16 + areaOffset + 5]);
					short alt = *((short *) &bgl[sect16 + areaOffset + 9]);
  					char* airportName = &bgl[sect16 + areaOffset + 22];
					int len = (int) strlen(airportName) + 1;
					// primary runways?
					int rid1 = bgl[sect16 + areaOffset + 22 + len] & 0x1F;
					int rid2 = bgl[sect16 + areaOffset + 23 + len] & 0x1F;
					
					printf("Airport ");
					printf("lat:% 8.4f lon:% 9.4f ", lat * (double) 90.0 / 5000875, lon / (double) 0x1000000 * 360);
					printf("alt:%4.0dm ", (int) alt);
					printf("%s\n", airportName);
					count++;
				}
				areaOffset += areaRec;
			}
			rangeTableOffset += 9;
		}
	}
*/
	// section 9: runways
	if (sect9 != 0)	{
		// scan all of section 9 looking for the runway pattern
		for (int i=0; i<(size-sect9); i++) {
			char rwRec = *((char *) &bgl[sect9 + i]);
			if (rwRec == 68) {
				char j1 = *((char *) &bgl[sect9 + i + 1]);
				char j2 = *((char *) &bgl[sect9 + i + 18]);
				char j3 = *((char *) &bgl[sect9 + i + 19]);
				char j4 = *((char *) &bgl[sect9 + i + 29]);
				if ((j1 == 0) && (j2 == 0) && (j2 == 0) && (j4 == 0)) {
					int latd = *((int *) &bgl[sect9 + i + 4]);
					double lat = latd * (45.0 / 5000875.0);
					int lond = *((int *) &bgl[sect9 + i + 10]);
					double lon = lond / 46603.377 / 256;
					short alt = *((short *) &bgl[sect9 + i + 16]);
					double course = *((short *) &bgl[sect9 + i + 20]) / 182.04444;
					if (course < 0.0)
						course += 180.0;
					short length = *((short *) &bgl[sect9 + i + 22]);
					short width = *((short *) &bgl[sect9 + i + 24]);
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

					// throw out obviously bad records
					if (fabs(lat) > 90.0) goto skip;
					if (fabs(lon) > 180.0) goto skip;
					if (alt > 10000) goto skip;
					if ((surface > 5) || (surface <0)) goto skip;

//					printf("Rwy ");
					printf("lat:% 8.4f lon:% 9.4f ", lat, lon);
					printf("alt:%4hd crs:%5.1f ", alt, course);
					printf("len:%5hd wid:%3hd sur:%d ", length, width, (unsigned) surface);
					char rwName[6];
					int c;
					c=sprintf(rwName, "%d", (unsigned) rwID);
					sprintf(&rwName[c], "%c", rwID2);
					printf("id:%s\n", rwName);
					i+=29;
					count++;
					skip:;
				}
			}
		}
	}

	// other sections

	printf("%d records processed\n", count);
	 
	// unmap file and close
	UnmapViewOfFile(bgl);
	CloseHandle(filemapping);
	CloseHandle(filehandle);

	return(0);
}
