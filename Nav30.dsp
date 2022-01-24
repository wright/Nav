# Microsoft Developer Studio Project File - Name="Nav30" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Nav30 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Nav30.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Nav30.mak" CFG="Nav30 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Nav30 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Nav30 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Nav30 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 xmlparse.lib xmltok.lib wsock32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Nav30 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o NUL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Nav30 - Win32 Release"
# Name "Nav30 - Win32 Debug"
# Begin Source File

SOURCE=.\Aircraft.cpp
# End Source File
# Begin Source File

SOURCE=.\Aircraft.h
# End Source File
# Begin Source File

SOURCE=.\autoopt.cpp
# End Source File
# Begin Source File

SOURCE=.\autoopt.h
# End Source File
# Begin Source File

SOURCE=.\bgl.cpp
# End Source File
# Begin Source File

SOURCE=.\CenterAt.cpp
# End Source File
# Begin Source File

SOURCE=.\CenterAt.h
# End Source File
# Begin Source File

SOURCE=.\CreateDpStar.cpp
# End Source File
# Begin Source File

SOURCE=.\CreateDpStar.h
# End Source File
# Begin Source File

SOURCE=.\datadial.cpp
# End Source File
# Begin Source File

SOURCE=.\datadial.h
# End Source File
# Begin Source File

SOURCE=.\details.cpp
# End Source File
# Begin Source File

SOURCE=.\details.h
# End Source File
# Begin Source File

SOURCE=.\Dpstar.cpp
# End Source File
# Begin Source File

SOURCE=.\Dpstar.h
# End Source File
# Begin Source File

SOURCE=.\Export.cpp
# End Source File
# Begin Source File

SOURCE=.\Export.h
# End Source File
# Begin Source File

SOURCE=.\ExportProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\ExportProgress.h
# End Source File
# Begin Source File

SOURCE=.\FancyMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\FancyMapper.h
# End Source File
# Begin Source File

SOURCE=.\flightpl.cpp
# End Source File
# Begin Source File

SOURCE=.\flightpl.h
# End Source File
# Begin Source File

SOURCE=.\folder_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.\folder_dialog.h
# End Source File
# Begin Source File

SOURCE=.\Fs2000State.h
# End Source File
# Begin Source File

SOURCE=.\fs6ipc.h
# End Source File
# Begin Source File

SOURCE=.\FS6IPC_Client.cpp
# End Source File
# Begin Source File

SOURCE=.\FS6IPC_Client.h
# End Source File
# Begin Source File

SOURCE=.\Fs6State.h
# End Source File
# Begin Source File

SOURCE=.\Fs98State.h
# End Source File
# Begin Source File

SOURCE=.\FTDensity.cpp
# End Source File
# Begin Source File

SOURCE=.\FTDensity.h
# End Source File
# Begin Source File

SOURCE=.\insertgpsdialog.cpp
# End Source File
# Begin Source File

SOURCE=.\insertgpsdialog.h
# End Source File
# Begin Source File

SOURCE=.\InsertGpsFixDial.cpp
# End Source File
# Begin Source File

SOURCE=.\InsertGpsFixDial.h
# End Source File
# Begin Source File

SOURCE=.\LinkOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkOptions.h
# End Source File
# Begin Source File

SOURCE=.\LoadAircraft.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadAircraft.h
# End Source File
# Begin Source File

SOURCE=.\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\MapOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\MapOptions.h
# End Source File
# Begin Source File

SOURCE=.\Nav.cpp
# End Source File
# Begin Source File

SOURCE=.\Nav.h
# End Source File
# Begin Source File

SOURCE=.\Nav.rc

!IF  "$(CFG)" == "Nav30 - Win32 Release"

!ELSEIF  "$(CFG)" == "Nav30 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Navdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Navdoc.h
# End Source File
# Begin Source File

SOURCE=.\Navview.cpp
# End Source File
# Begin Source File

SOURCE=.\Navview.h
# End Source File
# Begin Source File

SOURCE=.\NewList.cpp
# End Source File
# Begin Source File

SOURCE=.\NewList.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\search.cpp
# End Source File
# Begin Source File

SOURCE=.\search.h
# End Source File
# Begin Source File

SOURCE=.\SelectDpStar.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectDpStar.h
# End Source File
# Begin Source File

SOURCE=.\serbgl.cpp
# End Source File
# Begin Source File

SOURCE=.\serbgl.h
# End Source File
# Begin Source File

SOURCE=.\SetMapOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\SetMapOptions.h
# End Source File
# Begin Source File

SOURCE=.\settitle.cpp
# End Source File
# Begin Source File

SOURCE=.\settitle.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Transition.cpp
# End Source File
# Begin Source File

SOURCE=.\Transition.h
# End Source File
# Begin Source File

SOURCE=.\utilcalcul.cpp
# End Source File
# Begin Source File

SOURCE=.\utilcalcul.h
# End Source File
# Begin Source File

SOURCE=.\Wind.cpp
# End Source File
# Begin Source File

SOURCE=.\Wind.h
# End Source File
# Begin Source File

SOURCE=.\xmlparse.h
# End Source File
# End Target
# End Project
