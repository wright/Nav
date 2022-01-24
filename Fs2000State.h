// $Header: /zofware/Fsasm6/Modules/FS6IPC/Client/Fs98State.h 6     9/08/97 11:00p Zof $

// FS2000 (FS7) state data structure
// Unfortunately, the state structure for FS2000 is different from FS95 & FS98

// This file, except portions attributed to other authors, is:
// Copyright (C) 1996-1997 Adam Szofran. All Rights Reserved.
// Permission granted for non-commercial use only.
// Commercial users should contact the author for licensing details.

#ifndef _FS2000State_h_
#define _FS2000State_h_

// state structure requires 1-byte structure packing
#ifdef _MSC_VER
#pragma pack(push, 1)
#endif

// state table filler for unknown data
#define  UNKNOWN_DATA(id, first, last)  BYTE  _filler##id[last-first+1]

// I'm only including what I have tested here, see the FS98State.h for hints on other addresses - Ted Wright
typedef struct tagFS2000StateData
{
            UNKNOWN_DATA(000, 0x0000, 0x029F);
  WORD     wMagVar;             // +02A0
            UNKNOWN_DATA(040, 0x02A2, 0x02B7);
  DWORD    dwTAS;               // +02B8  true airspeed (m/s * 256)
            UNKNOWN_DATA(065, 0x02BC, 0x055F);
  DWORD    dwLatLo;             // +0560 latitude  (low dword)
  DWORD    dwLatHi;             // +0564 latitude  (high dword)
  DWORD    dwLonLo;             // +0568 longitude (low dword)
  DWORD    dwLonHi;             // +056C longitude (high dword)
  DWORD    dwAltLo;             // +0570 altitude  (low dword)
  DWORD    dwAltHi;             // +0574 altitude  (high dword)
  DWORD    dwPitch;             // +0578 pitch
  DWORD    dwBank;              // +057C bank
  DWORD    dwHeading;           // +0580 heading
            UNKNOWN_DATA(073, 0x0584, 0x07B7);
  // ---------------------------------------------------------------------
  // Autopilot info 
  // Courtesy of Ted Wright <wright@en.com>
  DWORD    dwAPMaster;          // +07B8 autopilot master switch (0 or 1)
  DWORD    dwAPWingLeveler;     // +07BC autopilot wing leveler enable (0 or 1)
  DWORD    dwAPNav1Hold;        // +07C0 autopilot Nav1 hold enable (0 or 1)
  DWORD    dwAPHeadingHold;     // +07C4 autopilot heading hold enable (0 or 1)
  DWORD    dwAPHeading;         // +07C8 autopilot heading to hold (actual heading * 0x10000/360.0)
  DWORD    dwAPAltitudeHold;    // +07CC autopilot altitude hold enable (0 or 1)
  DWORD    dwAPAltitude;        // +07D0 autopilot altitude to hold (actual altitude in meters * 0x10000)
  DWORD    dwAPAttitudeHold;    // +07D4 autopilot attitude (pitch & bank) hold enable (0 or 1)
  DWORD    dwAPAirSpeedHold;    // +07D8 autopilot air speed hold enable (0 or 1)
            UNKNOWN_DATA(075, 0x07DC, 0x07DD);
  WORD     wAPAirSpeed;         // +07DE autopilot air speed to hold (in knots)
  DWORD    dwAPMachNumberHold;  // +07E0 autopilot Mach number hold enable (0 or 1)
  DWORD    dwAPMachNumber;      // +07E4 autopilot Mach number to hold (percent Mach * 0xFFFF)
            UNKNOWN_DATA(076, 0x07E8, 0x07E9);
  WORD     wAPVerticalSpeed;    // +07EA autopilot vertical speed to hold (in feet/minute)
            UNKNOWN_DATA(077, 0x07EC, 0x07F3);
  DWORD    dwAPGlideSlopeHold;  // +07F4 autopilot glide slope hold enable (0 or 1) (enable with dwAPLocalizerHold for approach hold)
  DWORD    dwAPLocalizerHold;   // +07F8 autopilot localizer hold enable (0 or 1)
  DWORD    dwAPBackCourseHold;  // +07FC autopilot back course hold enable (0 or 1)
  DWORD    dwAPYawDamper;       // +0800 autopilot yaw damper enable (0 or 1)
  DWORD    dwAPToGa;			// +0804 autopilot take-off/go-around enable (0 or 1)
  DWORD    dwAPAutoThrottle;    // +0808 autopilot auto throttle enable (0 or 1)
  UNKNOWN_DATA(220, 0x080C, 0x1FFF);

} FS2000StateData;

#undef UNKNOWN_DATA

#ifdef _MSC_VER
#pragma pack(pop)
#endif

#endif // _FS2000State_h_
