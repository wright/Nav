// $Header: /zofware/Fsasm6/Modules/Common/Fs6State.h 17    7/23/97 11:09p Zof $

// FS6 state data structure

// This file, except portions attributed to other authors, is:
// Copyright (C) 1996-1997 Adam Szofran. All Rights Reserved.
// Permission granted for non-commercial use only.
// Commercial users should contact the author for licensing details.

#ifndef _FS6State_h_
#define _FS6State_h_

// state structure requires 1-byte structure packing
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

// state table filler for unknown data
#define  UNKNOWN_DATA(id, first, last)  BYTE  _filler##id[last-first+1]

typedef struct tagFS6StateData
{
            UNKNOWN_DATA(000, 0x0000, 0x0237);
  BYTE     byClockHour;         // +0238
  BYTE     byClockMin;          // +0239
  BYTE     byClockSec;          // +023A
            UNKNOWN_DATA(005, 0x023B, 0x0267);
  DWORD    dwPauseFlag;         // +0268  set to 1 when paused, 0 when not
            UNKNOWN_DATA(010, 0x026C, 0x0273);
  DWORD    dwDeltaTime;         // +0274  sim time interval in seconds * 8000h
  DWORD    dwPanelUpdateFlags;  // +0278  set to 0xFFFFFFFF to update the panel
            UNKNOWN_DATA(015, 0x027C, 0x0287);
  BYTE     byLightsOn;          // +0288
  BYTE     byStrobeOn;          // +0289
  BYTE     byStrobeTimer;       // +028A
            UNKNOWN_DATA(020, 0x028B, 0x0297);
  BYTE     byLandingLightOn;    // +0298
            UNKNOWN_DATA(030, 0x0299, 0x029D);
  WORD     wSpoiler;            // +029E  0: retracted; 7FFF: extended
  WORD     wMagVar;             // +02A0
            UNKNOWN_DATA(035, 0x02A2, 0x02A7);
  WORD     wTickCount;          // +02A8  count of 55 msec ticks since FS6 started
  WORD     wFrameTime;          // +02AA  frame time interval in seconds * 8000h
            UNKNOWN_DATA(040, 0x02AC, 0x02B3);
  DWORD    dwTAS;               // +02B4  true airspeed (m/s * 256)
  DWORD    dwIAS;               // +02B8  indicated airspeed (m/s * 256)
            UNKNOWN_DATA(050, 0x02BC, 0x0355);
  WORD     wBcdAdfFreq;         // +0356
  WORD     wBcdCom1Freq;        // +0358
  WORD     wBcdNav1Freq;        // +035A
  WORD     wBcdNav2Freq;        // +035C
  WORD     wBcdXpdrCode;        // +035E
            UNKNOWN_DATA(060, 0x0360, 0x0451);
  char     szVisBaseName[256];  // +0452 VIS base file name (JF Vole)
            UNKNOWN_DATA(065, 0x0552, 0x05A7);
  DWORD    dwLatLo;             // +05A8 latitude  (low dword)
  DWORD    dwLatHi;             // +05AC latitude  (high dword)
  DWORD    dwLonLo;             // +05B0 longitude (low dword)
  DWORD    dwLonHi;             // +05B4 longitude (high dword)
  DWORD    dwAltLo;             // +05B8 altitude  (low dword)
  DWORD    dwAltHi;             // +05BC altitude  (high dword)
  DWORD    dwPitch;             // +05C0 pitch
  DWORD    dwBank;              // +05C4 bank
  DWORD    dwHeading;           // +05C8 heading
            UNKNOWN_DATA(070, 0x05CC, 0x065B);
  char     szMdlBaseName[256];  // +065C aircraft MDL base file name
            UNKNOWN_DATA(073, 0x075C, 0x08f7);

  // ---------------------------------------------------------------------
  // Autopilot info 
  // Courtesy of Ted Wright <wright@en.com>
  DWORD    dwAPMaster;          // +08f8 autopilot master switch (0 or 1)
  DWORD    dwAPWingLeveler;     // +08fc autopilot wing leveler enable (0 or 1)
  DWORD    dwAPNav1Hold;        // +0900 autopilot Nav1 hold enable (0 or 1)
  DWORD    dwAPHeadingHold;     // +0904 autopilot heading hold enable (0 or 1)
  DWORD    dwAPHeading;         // +0908 autopilot heading to hold (actual heading * 0x10000/360.0)
  DWORD    dwAPAltitudeHold;    // +090c autopilot altitude hold enable (0 or 1)
  DWORD    dwAPAltitude;        // +0910 autopilot altitude to hold (actual altitude in meters * 0x10000)
  DWORD    dwAPAttitudeHold;    // +0914 autopilot attitude (pitch & bank) hold enable (0 or 1)
  DWORD    dwAPGlideSlopeHold;  // +0918 autopilot glide slope hold enable (0 or 1)
  DWORD    dwAPLocalizerHold;   // +091c autopilot localizer hold enable (0 or 1)
  DWORD    dwAPBackCourseHold;  // +0920 autopilot back course hold enable (0 or 1)
  DWORD    dwAPYawDamper;       // +0924 autopilot yaw damper enable (0 or 1)
  // ---------------------------------------------------------------------

            UNKNOWN_DATA(075, 0x0928, 0x09A5);

  // ---------------------------------------------------------------------
  // Engine info 
  // Courtesy of Joaquin Manuel <joaquinm@medialabs.es>
  WORD     wN2RPM;              // +09A6 engine 1  N2
  WORD     wRPM;                // +09A8
            UNKNOWN_DATA(076, 0x09AA, 0x09F9);
  WORD     wN2RPM2;             // +09FA engine 1  N2
  WORD     wN1RPM2;             // +09FC engine 1  N1
            UNKNOWN_DATA(077, 0x09FE, 0x0A4D);
  WORD     wN2RPM3;             // +0A4E engine 1  N2
  WORD     wN1RPM3;             // +0A50 engine 1  N1
            UNKNOWN_DATA(078, 0x0A52, 0x0AA1);
  WORD     wN2RPM4;             // +0AA2 engine 1  N2
  WORD     wN1RPM4;             // +0AA4 engine 1  N1
  // ---------------------------------------------------------------------

            UNKNOWN_DATA(080, 0x0AA6, 0x0B67);

  DWORD    dwFuelLeftPct;       // +0B68  left fuel tank quantity (%)
  DWORD    dwFuelLeftCapGal;    // +0B6C  left fuel tank capacity (gallons)
  DWORD    dwFuelLeftAuxPct;    // +0B70  left aux fuel tank quantity (%)
  DWORD    dwFuelLeftAuxCapGal; // +0B74  left aux fuel tank capacity (gallons)
            UNKNOWN_DATA(090, 0x0B78, 0x0B7F);
  DWORD    dwFuelRightPct;      // +0B80  right fuel tank quantity (%)
  DWORD    dwFuelRightCapGal;   // +0B84  right fuel tank capacity (gallons)
  DWORD    dwFuelRightAuxPct;   // +0B88  right aux fuel tank quantity (%)
  DWORD    dwFuelRightAuxCapGal;// +0B8C  right aux fuel tank capacity (gallons)
            UNKNOWN_DATA(100, 0x0B90, 0x0BB9);
  WORD     wFlaps;              // +0BBA  0: retracted; 7FFF: extended
            UNKNOWN_DATA(110, 0x0BBC, 0x0BBF);
  WORD     wGear1;              // +0BC0  gear position (0=up, FFFF=down)
  WORD     wGear2;              // +0BC2
  WORD     wGear3;              // +0BC4
  WORD     wGear1Timer;         // +0BC6  gear position (0=up, 7FFF=down)
  WORD     wGear2Timer;         // +0BC8
  WORD     wGear3Timer;         // +0BCA
            UNKNOWN_DATA(120, 0x0BCC, 0x0BE5);
  WORD     wTimeRate;           // +0BE6  time rate (1.0 == 0100h)

  // ---------------------------------------------------------------------
  // Various panel display variables
  // Courtesy of JF Vole <oavi@gulliver.fr>
  char     szDisplayZoom[4];      // +0BE8 Display Zoom
  char     szDisplayClock[9];     // +0BEC Display Time	
  char     szDisplayDme1Dist[5];  // +0BF5 Display DME 1 Dist
  char     szDisplayDme1Speed[5]; // +0BFA Display DME 1 Dist
  char     szDisplayDme2Dist[5];  // +0BFF Display DME 2 Dist
  char     szDisplayDme2Speed[5]; // +0C04 Display DME 2 Dist
  // ---------------------------------------------------------------------

            UNKNOWN_DATA(125, 0x0C09, 0x0D0F);
  DWORD    dwViewFlags;         // +0D10
            UNKNOWN_DATA(130, 0x0D14, 0x0EE7);

  // ---------------------------------------------------------------------
  // Weather info 
  // Courtesy of Jason Grooms <Jason_Grooms@oxy.com>
  WORD     wCloud2Ceiling;      // +0EE8  (meters)
  WORD     wCloud2Floor;        // +0EEA  (meters)
  WORD     wCloud2Coverage;     // +0EEC
  WORD     wCloud2Deviation;    // +0EEE  (meters)
     
  WORD     wCloud1Ceiling;      // +0EF0  (meters)
  WORD     wCloud1Floor;        // +0EF2  (meters)
  WORD     wCloud1Coverage;     // +0EF4
  WORD     wCloud1Deviation;    // +0EF6  (meters)
     
  WORD     wTStormCeiling;      // +0EF8  (meters)
  WORD     wTStormFloor;        // +0EFA  (meters)
  WORD     wTStormCoverage;     // +0EFC
  WORD     wTStormDeviation;    // +0EFE  (meters)
     
  WORD     wTemp3Ceiling;       // +0F00  (meters)
  WORD     wTemp3Temp;          // +0F02  (celsius * 256)
     
  WORD     wTemp2Ceiling;       // +0F04  (meters)
  WORD     wTemp2Temp;          // +0F06  (celsius * 256)
     
  WORD     wTemp1Ceiling;       // +0F08  (meters)
  WORD     wTemp1Temp;          // +0F0A  (celsius * 256)
     
  WORD     wTemp0Ceiling;       // +0F0C  (meters)
  WORD     wTemp0Temp;          // +0F0E  (celsius * 256)
     
  WORD     wDayNightDrift;      // +0F10
  WORD     wDayNightVariation;  // +0F12
     
  WORD     wBarometricPressure; // +0F14
  WORD     wBarometricDrift;    // +0F16
     
  WORD     wWind3Ceiling;       // +0F18  (meters)
  WORD     wWind3Floor;         // +0F1A  (meters)
  WORD     wWind3Speed;         // +0F1C
  WORD     wWind3Direction;     // +0F1E
  WORD     wWind3Turbulence;    // +0F20
  WORD     wWind3GustySteady;   // +0F22
     
  WORD     wWind2Ceiling;       // +0F24  (meters)
  WORD     wWind2Floor;         // +0F26  (meters)
  WORD     wWind2Speed;         // +0F28
  WORD     wWind2Direction;     // +0F2A
  WORD     wWind2Turbulence;    // +0F2C
  WORD     wWind2GustySteady;   // +0F2E
     
  WORD     wWind1Ceiling;       // +0F30  (meters)
  WORD     wWind1Floor;         // +0F32  (meters)
  WORD     wWind1Speed;         // +0F34
  WORD     wWind1Direction;     // +0F36
  WORD     wWind1Turbulence;    // +0F38
  WORD     wWind1GustySteady;   // +0F3A
     
  WORD     wWind0Ceiling;       // +0F3C  (meters)
  union {
  WORD     wWind0Speed;         // +0F3E
  WORD     wW0Speed;            // +0F3E  (old name)
  };
  union {
  WORD     wWind0Direction;     // +0F40
  WORD     wW0Dir;              // +0F40  (old name)
  };
  union {
  WORD     wWind0Turbulence;    // +0F42
  WORD     wW0Trb;              // +0F42  (old name)
  };
  WORD     wWind0GustySteady;   // +0F44
     
  WORD     wCloud2Type;         // +0F46
  WORD     wCloud2Icing;        // +0F48
  WORD     wCloud2Turbulence;   // +0F4A
  WORD     wCloud1Type;         // +0F4C
  WORD     wCloud1Icing;        // +0F4E
  WORD     wCloud1Turbulence;   // +0F50
  WORD     wTStormsType;        // +0F52
  WORD     wTStormsIcing;       // +0F54
  WORD     wTStormsTurbulence;  // +0F56
     
  WORD     wVisibility;         // +0F58
  // ---------------------------------------------------------------------

            UNKNOWN_DATA(140, 0x0F5A, 0x113D);
  WORD     wTimeOfDay;          // +113E
            UNKNOWN_DATA(150, 0x1140, 0x114B);
  WORD     wDynAirTraffic1;     // +114C  dynamic scenery (view window 1)
  WORD     wDynGroundTraffic1;  // +114E
  WORD     wDynServiceTraffic1; // +1150
  WORD     wDynOutsideTraffic1; // +1152
  WORD     wDynFrequency1;      // +1154
  WORD     wDynAirTraffic2;     // +1156  dynamic scenery (view window 2)
  WORD     wDynGroundTraffic2;  // +1158
  WORD     wDynServiceTraffic2; // +115A
  WORD     wDynOutsideTraffic2; // +115C
  WORD     wDynFrequency2;      // +115E
            UNKNOWN_DATA(160, 0x1160, 0x1621);
  WORD     wTextureFlag;        // +1622
  WORD     wEngineOn;           // +1624
            UNKNOWN_DATA(170, 0x1626, 0x162B);
  WORD     wAileron;            // +162C
  WORD     wElevator;           // +162E
  WORD     wRudder;             // +1630
            UNKNOWN_DATA(180, 0x1632, 0x1687);
  void*    pMdlState;           // +1688;
            UNKNOWN_DATA(190, 0x168C, 0x16A9);
  WORD     wWheelSmoke;         // +16AA
            UNKNOWN_DATA(200, 0x16AC, 0x176B);
  WORD     wFsVersion;          // +176C
            UNKNOWN_DATA(210, 0x176D, 0x17BF);
  char     szAdvWav[256];  // +17C0 Wav File Name (JF Vole)
            UNKNOWN_DATA(220, 0x17C0, 0x1FFF);
} FS6StateData;

#undef UNKNOWN_DATA

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // _FS6State_h_
