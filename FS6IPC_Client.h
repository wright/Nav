//************************************************************************
// $Header: /zofware/Fsasm6/Modules/FS6IPC/Client/FS6IPC_Client.h 4     7/13/97 2:24p Zof $
// 
// Interprocess Communication Client for FS6
// 
// Copyright (C) 1996-1997 Adam Szofran. All Rights Reserved.
// Permission granted for non-commercial use only.
// Commercial users should contact the author for licensing details.
//************************************************************************

#ifndef _FS6IPC_Client_h_
#define _FS6IPC_Client_h_

class FS6IPC_Client
{
  public:
    FS6IPC_Client();
    virtual ~FS6IPC_Client();

  // client start/stop
  public:
    BOOL Start(HWND hWndFs6, DWORD dwBufSize = 0x10000, DWORD dwTimeout = 2000);
    void Stop();

  // request structure setup
  public:
    BOOL BeginRequest();
    BOOL ReadStateData(DWORD dwStateTableOffset, DWORD nBytes, void* pDest);
    BOOL WriteStateData(DWORD dwStateTableOffset, DWORD nBytes, void* pSrc);
    BOOL SendRequest();

  // data
  protected:
    DWORD   m_id;         // instance identifier
    HWND    m_hWnd;       // FS6 window handle
    DWORD   m_dwTimeout;  // message timeout in milliseconds
    UINT    m_msg;        // id of registered window message
    ATOM    m_atom;       // global atom containing name of file-mapping object
    HANDLE  m_hMap;       // handle of file-mapping object
    BYTE*   m_pView;      // pointer to view of file-mapping object
    BYTE*   m_pNext;      // pointer to next available byte in view

    static DWORD  m_idNext; // next instance identifier
};

#endif // _FS6IPC_Client_h_
