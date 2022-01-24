//************************************************************************
// $Header: /zofware/Fsasm6/Modules/FS6IPC/Client/fs6ipc.h 5     7/13/97 2:24p Zof $
// 
// Interprocess Communication Definitions for FS6
// 
// Copyright (C) 1996-1997 Adam Szofran. All Rights Reserved.
// Permission granted for non-commercial use only.
// Commercial users should contact the author for licensing details.
//************************************************************************

#ifndef _FS6IPC_H_
#define _FS6IPC_H_

#define FS6IPC_MESSAGENAME      "FsasmLib:IPC" 

// IPC message return values
// Only valid for FsasmLib.DLL version 6.0.0.334 and later
// Note: I would prefer to make SUCCESS==0 but that would break
// older client implementations.
#define FS6IPC_MESSAGE_SUCCESS 1
#define FS6IPC_MESSAGE_FAILURE 0

// IPC message types
#define FS6IPC_READSTATEDATA_ID    1
#define FS6IPC_WRITESTATEDATA_ID   2

// read request structure
typedef struct tagFS6IPC_READSTATEDATA_HDR
{
  DWORD dwId;       // FS6IPC_READSTATEDATA_ID
  DWORD dwOffset;   // state table offset
  DWORD nBytes;     // number of bytes of state data to read
  void* pDest;      // destination buffer for data (client use only)
} FS6IPC_READSTATEDATA_HDR;

// write request structure
typedef struct tagFS6IPC_WRITESTATEDATA_HDR
{
  DWORD dwId;       // FS6IPC_WRITESTATEDATA_ID
  DWORD dwOffset;   // state table offset
  DWORD nBytes;     // number of bytes of state data to write
} FS6IPC_WRITESTATEDATA_HDR;

#endif // _FS6IPC_H_
