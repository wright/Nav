// $Header: /zofware/Fsasm6/Modules/FS6IPC/Client/Fs98State.h 6     9/08/97 11:00p Zof $

// FS98 (FS6.1) state data structure
// Unfortunately, the state structure for FS98 is different from FS95

// This file, except portions attributed to other authors, is:
// Copyright (C) 1996-1997 Adam Szofran. All Rights Reserved.
// Permission granted for non-commercial use only.
// Commercial users should contact the author for licensing details.

#ifndef _FS98State_h_
#define _FS98State_h_

// state structure requires 1-byte structure packing
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

// state table filler for unknown data
#define  UNKNOWN_DATA(id, first, last)  BYTE  _filler##id[last-first+1]

typedef struct tagFS98StateData
{
            UNKNOWN_DATA(000, 0x0000, 0x0237);
  BYTE     byClockHour;         // +0238
  BYTE     byClockMin;          // +0239
  BYTE     byClockSec;          // +023A
//            UNKNOWN_DATA(005, 0x023B, 0x0267);
//  DWORD    dwPauseFlag;         // +0268  set to 1 when paused, 0 when not
//            UNKNOWN_DATA(010, 0x026C, 0x0273);
//  DWORD    dwDeltaTime;         // +0274  sim time interval in seconds * 8000h
//  DWORD    dwPanelUpdateFlags;  // +0278  set to 0xFFFFFFFF to update the panel
            UNKNOWN_DATA(015, 0x023B, 0x027F);
  BYTE     byLightsOn;          // +0280
  BYTE     byStrobeOn;          // +0281
  BYTE     byStrobeTimer;       // +0282
            UNKNOWN_DATA(020, 0x0283, 0x028B);
  union {
  DWORD    dwLandingLightOn;    // +028C  
  BYTE     byLandingLightOn;    // +028C  FS6State compatability
  };
            UNKNOWN_DATA(030, 0x0290, 0x029D);
  WORD     wSpoiler;            // +029E  0: retracted; 7FFF: extended
  WORD     wMagVar;             // +02A0
//            UNKNOWN_DATA(035, 0x02A2, 0x02A7);
//  WORD     wTickCount;          // +02A8  count of 55 msec ticks since FS6 started
//  WORD     wFrameTime;          // +02AA  frame time interval in seconds * 8000h
            UNKNOWN_DATA(040, 0x02A2, 0x02B7);
  DWORD    dwTAS;               // +02B8  true airspeed (m/s * 256)
//  DWORD    dwIAS;               // +02B8  indicated airspeed (m/s * 256)
            UNKNOWN_DATA(050, 0x02BC, 0x034B);
  WORD     wBcdAdfFreq;         // +034C
  WORD     wBcdCom1Freq;        // +034E
  WORD     wBcdNav1Freq;        // +0350
  WORD     wBcdNav2Freq;        // +0352
  WORD     wBcdXpdrCode;        // +0354
            UNKNOWN_DATA(060, 0x0356, 0x0445);
  char     szVisBaseName[256];  // +0446 VIS base file name (JF Vole)
            UNKNOWN_DATA(065, 0x0546, 0x055F);
  DWORD    dwLatLo;             // +0560 latitude  (low dword)
  DWORD    dwLatHi;             // +0564 latitude  (high dword)
  DWORD    dwLonLo;             // +0568 longitude (low dword)
  DWORD    dwLonHi;             // +056C longitude (high dword)
  DWORD    dwAltLo;             // +0570 altitude  (low dword)
  DWORD    dwAltHi;             // +0574 altitude  (high dword)
  DWORD    dwPitch;             // +0578 pitch
  DWORD    dwBank;              // +057C bank
  DWORD    dwHeading;           // +0580 heading
//            UNKNOWN_DATA(070, 0x05CC, 0x065B);
//  char     szMdlBaseName[256];  // +065C aircraft MDL base file name
            UNKNOWN_DATA(073, 0x0584, 0x07BB);

  // ---------------------------------------------------------------------
  // Autopilot info 
  // Courtesy of Ted Wright <wright@en.com>
  // FS98 offsets courtesy of David Drouin <sw3david@globetrotter.qc.ca>
  DWORD    dwAPMaster;          // +07BC autopilot master switch (0 or 1)
  DWORD    dwAPWingLeveler;     // +07C0 autopilot wing leveler enable (0 or 1)
  DWORD    dwAPNav1Hold;        // +07C4 autopilot Nav1 hold enable (0 or 1)
  DWORD    dwAPHeadingHold;     // +07C8 autopilot heading hold enable (0 or 1)
  DWORD    dwAPHeading;         // +07CC autopilot heading to hold (actual heading * 0x10000/360.0)
  DWORD    dwAPAltitudeHold;    // +07D0 autopilot altitude hold enable (0 or 1)
  DWORD    dwAPAltitude;        // +07D4 autopilot altitude to hold (actual altitude in meters * 0x10000)
  DWORD    dwAPAttitudeHold;    // +07D8 autopilot attitude (pitch & bank) hold enable (0 or 1)
  DWORD    dwAPAirSpeedHold;    // +07DC autopilot air speed hold enable (0 or 1)
            UNKNOWN_DATA(075, 0x07E0, 0x07E1);
  WORD     wAPAirSpeed;         // +07E2 autopilot air speed to hold (in knots)
  DWORD    dwAPMachNumberHold;  // +07E4 autopilot Mach number hold enable (0 or 1)
  DWORD    dwAPMachNumber;      // +07E8 autopilot Mach number to hold (percent Mach * 0xFFFF)
            UNKNOWN_DATA(076, 0x07EC, 0x07F1);
  WORD     wAPVerticalSpeed;    // +07F2 autopilot vertical speed to hold (in feet/minute)
            UNKNOWN_DATA(077, 0x07F4, 0x07FB);
  DWORD    dwAPGlideSlopeHold;  // +07FC autopilot glide slope hold enable (0 or 1) (enable with dwAPLocalizerHold for approach hold)
  DWORD    dwAPLocalizerHold;   // +0800 autopilot localizer hold enable (0 or 1)
  DWORD    dwAPBackCourseHold;  // +0804 autopilot back course hold enable (0 or 1)
  DWORD    dwAPYawDamper;       // +0808 autopilot yaw damper enable (0 or 1)
  DWORD    dwAPToGa;			// +080C autopilot take-off/go-around enable (0 or 1)
  DWORD    dwAPAutoThrottle;    // +0810 autopilot auto throttle enable (0 or 1)
  // ---------------------------------------------------------------------
            UNKNOWN_DATA(078, 0x0814, 0x0BE7);

  // ---------------------------------------------------------------------
  // Engine info 
  // Courtesy of Joaquin Manuel <joaquinm@medialabs.es>
//  WORD     wN2RPM;              // +09A6 engine 1  N2
//  WORD     wRPM;                // +09A8
//            UNKNOWN_DATA(076, 0x09AA, 0x09F9);
//  WORD     wN2RPM2;             // +09FA engine 1  N2
//  WORD     wN1RPM2;             // +09FC engine 1  N1
//            UNKNOWN_DATA(077, 0x09FE, 0x0A4D);
//  WORD     wN2RPM3;             // +0A4E engine 1  N2
//  WORD     wN1RPM3;             // +0A50 engine 1  N1
//            UNKNOWN_DATA(078, 0x0A52, 0x0AA1);
//  WORD     wN2RPM4;             // +0AA2 engine 1  N2
//  WORD     wN1RPM4;             // +0AA4 engine 1  N1
  // ---------------------------------------------------------------------

//            UNKNOWN_DATA(080, 0x0AA6, 0x0B67);

//  DWORD    dwFuelLeftPct;       // +0B68  left fuel tank quantity (%)
//  DWORD    dwFuelLeftCapGal;    // +0B6C  left fuel tank capacity (gallons)
//  DWORD    dwFuelLeftAuxPct;    // +0B70  left aux fuel tank quantity (%)
//  DWORD    dwFuelLeftAuxCapGal; // +0B74  left aux fuel tank capacity (gallons)
//            UNKNOWN_DATA(090, 0x0B78, 0x0B7F);
//  DWORD    dwFuelRightPct;      // +0B80  right fuel tank quantity (%)
//  DWORD    dwFuelRightCapGal;   // +0B84  right fuel tank capacity (gallons)
//  DWORD    dwFuelRightAuxPct;   // +0B88  right aux fuel tank quantity (%)
//  DWORD    dwFuelRightAuxCapGal;// +0B8C  right aux fuel tank capacity (gallons)
//            UNKNOWN_DATA(100, 0x0B90, 0x0BB9);
//  WORD     wFlaps;              // +0BBA  0: retracted; 7FFF: extended
//            UNKNOWN_DATA(110, 0x07DC, 0x0BE7);
  union {
  DWORD    dwGear;              // +0BE8  gear position (0=up, 3FFF=down
  WORD     wGear1;              // +0BE8  FS6State compatibility
  WORD     wGear2;              // +0BE8  FS6State compatibility
  WORD     wGear3;              // +0BE8  FS6State compatibility
  };
  union {
  DWORD    dwGear1Timer;        // +0BEC  gear 1 timer (0=up, 3FFF=down)
  WORD     wGear1Timer;         // +0BEC  FS6State compatibility
  };
  union {
  DWORD    dwGear2Timer;        // +0BF0  gear 2 timer
  WORD     wGear2Timer;         // +0BF0  FS6State compatibility
  };
  union {
  DWORD    dwGear3Timer;        // +0BF4  gear 3 timer
  WORD     wGear3Timer;         // +0BF4  FS6State compatibility
  };
//            UNKNOWN_DATA(120, 0x0BCC, 0x0BE5);
//  WORD     wTimeRate;           // +0BE6  time rate (1.0 == 0100h)

  // ---------------------------------------------------------------------
  // Various panel display variables
  // Courtesy of JF Vole <oavi@gulliver.fr>
//  char     szDisplayZoom[4];      // +0BE8 Display Zoom
//  char     szDisplayClock[9];     // +0BEC Display Time	
//  char     szDisplayDme1Dist[5];  // +0BF5 Display DME 1 Dist
//  char     szDisplayDme1Speed[5]; // +0BFA Display DME 1 Dist
//  char     szDisplayDme2Dist[5];  // +0BFF Display DME 2 Dist
//  char     szDisplayDme2Speed[5]; // +0C04 Display DME 2 Dist
  // ---------------------------------------------------------------------

            UNKNOWN_DATA(125, 0x0BF8, 0x0D47);
  DWORD    dwViewFlags;         // +0D48
            UNKNOWN_DATA(130, 0x0D4C, 0x0F1B);

  // ---------------------------------------------------------------------
  // Weather info 
  // Courtesy of Jason Grooms <Jason_Grooms@oxy.com>
  WORD     wCloud2Ceiling;      // +0F1C  (meters)
  WORD     wCloud2Floor;        // +0F1E  (meters)
  WORD     wCloud2Coverage;     // +0F20
  WORD     wCloud2Deviation;    // +0F22  (meters)
                                        
  WORD     wCloud1Ceiling;      // +0F24  (meters)
  WORD     wCloud1Floor;        // +0F26  (meters)
  WORD     wCloud1Coverage;     // +0F28
  WORD     wCloud1Deviation;    // +0F2A  (meters)
                                        
  WORD     wTStormCeiling;      // +0F2C  (meters)
  WORD     wTStormFloor;        // +0F2E  (meters)
  WORD     wTStormCoverage;     // +0F30
  WORD     wTStormDeviation;    // +0F32  (meters)
                                        
  WORD     wTemp3Ceiling;       // +0F34  (meters)
  WORD     wTemp3Temp;          // +0F36  (celsius * 256)
                                        
  WORD     wTemp2Ceiling;       // +0F38  (meters)
  WORD     wTemp2Temp;          // +0F3A  (celsius * 256)
                                        
  WORD     wTemp1Ceiling;       // +0F3C  (meters)
  WORD     wTemp1Temp;          // +0F3E  (celsius * 256)
                                        
  WORD     wTemp0Ceiling;       // +0F40  (meters)
  WORD     wTemp0Temp;          // +0F42  (celsius * 256)
                                        
  WORD     wDayNightDrift;      // +0F44
  WORD     wDayNightVariation;  // +0F46
                                        
  WORD     wBarometricPressure; // +0F48
  WORD     wBarometricDrift;    // +0F4A
                                        
  WORD     wWind3Ceiling;       // +0F4C  (meters)
  WORD     wWind3Floor;         // +0F4E  (meters)
  WORD     wWind3Speed;         // +0F50
  WORD     wWind3Direction;     // +0F52
  WORD     wWind3Turbulence;    // +0F54
  WORD     wWind3GustySteady;   // +0F56
                                        
  WORD     wWind2Ceiling;       // +0F58  (meters)
  WORD     wWind2Floor;         // +0F5A  (meters)
  WORD     wWind2Speed;         // +0F5C
  WORD     wWind2Direction;     // +0F5E
  WORD     wWind2Turbulence;    // +0F60
  WORD     wWind2GustySteady;   // +0F62
                                        
  WORD     wWind1Ceiling;       // +0F64  (meters)
  WORD     wWind1Floor;         // +0F66  (meters)
  WORD     wWind1Speed;         // +0F68
  WORD     wWind1Direction;     // +0F6A
  WORD     wWind1Turbulence;    // +0F6C
  WORD     wWind1GustySteady;   // +0F6E
                                        
  WORD     wWind0Ceiling;       // +0F70  (meters)
  union {                               
  WORD     wWind0Speed;         // +0F72
  WORD     wW0Speed;            // +0F72  (old name)
  };                                    
  union {                               
  WORD     wWind0Direction;     // +0F74
  WORD     wW0Dir;              // +0F74  (old name)
  };                                    
  union {                               
  WORD     wWind0Turbulence;    // +0F76
  WORD     wW0Trb;              // +0F76  (old name)
  };                                    
  WORD     wWind0GustySteady;   // +0F78
                                        
  WORD     wCloud2Type;         // +0F7A
  WORD     wCloud2Icing;        // +0F7C
  WORD     wCloud2Turbulence;   // +0F7E
  WORD     wCloud1Type;         // +0F80
  WORD     wCloud1Icing;        // +0F82
  WORD     wCloud1Turbulence;   // +0F84
  WORD     wTStormsType;        // +0F86
  WORD     wTStormsIcing;       // +0F88
  WORD     wTStormsTurbulence;  // +0F8A
                                        
  WORD     wVisibility;         // +0F8C
  // ---------------------------------------------------------------------

//            UNKNOWN_DATA(140, 0x0F5A, 0x113D);
//  WORD     wTimeOfDay;          // +113E
//            UNKNOWN_DATA(150, 0x1140, 0x114B);
//  WORD     wDynAirTraffic1;     // +114C  dynamic scenery (view window 1)
//  WORD     wDynGroundTraffic1;  // +114E
//  WORD     wDynServiceTraffic1; // +1150
//  WORD     wDynOutsideTraffic1; // +1152
//  WORD     wDynFrequency1;      // +1154
//  WORD     wDynAirTraffic2;     // +1156  dynamic scenery (view window 2)
//  WORD     wDynGroundTraffic2;  // +1158
//  WORD     wDynServiceTraffic2; // +115A
//  WORD     wDynOutsideTraffic2; // +115C
//  WORD     wDynFrequency2;      // +115E
//            UNKNOWN_DATA(160, 0x1160, 0x1621);
//  WORD     wTextureFlag;        // +1622
//  WORD     wEngineOn;           // +1624
//            UNKNOWN_DATA(170, 0x1626, 0x162B);
//  WORD     wAileron;            // +162C
//  WORD     wElevator;           // +162E
//  WORD     wRudder;             // +1630
//            UNKNOWN_DATA(180, 0x1632, 0x1687);
//  void*    pMdlState;           // +1688;
//            UNKNOWN_DATA(190, 0x168C, 0x16A9);
//  WORD     wWheelSmoke;         // +16AA
//            UNKNOWN_DATA(200, 0x16AC, 0x176B);
//  WORD     wFsVersion;          // +176C
//            UNKNOWN_DATA(210, 0x176D, 0x17BF);
//  char     szAdvWav[256];  // +17C0 Wav File Name (JF Vole)
            UNKNOWN_DATA(220, 0x0F8E, 0x1FFF);
} FS98StateData;

#undef UNKNOWN_DATA

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // _FS98State_h_
