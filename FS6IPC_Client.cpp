//************************************************************************
// $Header: /zofware/Fsasm6/Modules/FS6IPC/Client/FS6IPC_Client.cpp 6     7/13/97 2:24p Zof $
// 
// Interprocess Communication Client for FS6
// 
// Copyright (C) 1996-1997 Adam Szofran. All Rights Reserved.
// Permission granted for non-commercial use only.
// Commercial users should contact the author for licensing details.
//************************************************************************

#define WIN32_LEAN_AND_MEAN
#include "stdafx.h"

#include "FS6IPC.h"
#include "FS6IPC_Client.h"

DWORD FS6IPC_Client::m_idNext = 0;

FS6IPC_Client::FS6IPC_Client()
{
  m_id = 0;
  m_dwTimeout = 2000;
  m_hWnd = 0;
  m_msg = 0;
  m_atom = 0;
  m_hMap = 0;
  m_pView = 0;
  m_pNext = 0;
}

FS6IPC_Client::~FS6IPC_Client()
{
  Stop();
}

//************************************************************************
// Start the client
// hWndFs6: window handle of FS6 instance to communicate with
// dwBufSize: size of the IPC buffer
// dwTimeout: timeout in milliseconds for calls to SendRequest
// return: TRUE if successful, FALSE otherwise
//************************************************************************
BOOL FS6IPC_Client::Start(HWND hWndFs6, DWORD dwBufSize, DWORD dwTimeout)
{
  // abort if already started
  if (m_pView)
    return FALSE;

  m_hWnd = hWndFs6;
  m_dwTimeout = dwTimeout;
  m_id = m_idNext++;

  // register the window message
  m_msg = RegisterWindowMessage(FS6IPC_MESSAGENAME);
  if (m_msg == 0)
    return FALSE;

  // create the name of our file-mapping object
  char szName[MAX_PATH];
  wsprintf(szName, FS6IPC_MESSAGENAME ":%X:%X", GetCurrentProcessId(), m_id);

  // stuff the name into a global atom
  m_atom = GlobalAddAtom(szName);
  if (m_atom == 0)
    {
      Stop();
      return FALSE;
    }

  // create the file-mapping object
  m_hMap = CreateFileMapping(
    (HANDLE)0xFFFFFFFF, // use system paging file
    NULL,               // security
    PAGE_READWRITE,     // protection
    0, dwBufSize,       // size
    szName);            // name 

  if ((m_hMap == 0) || (GetLastError() == ERROR_ALREADY_EXISTS))
    {
      Stop();
      return FALSE;    
    }

  // get a view of the file-mapping object
  m_pView = (BYTE*)MapViewOfFile(m_hMap, FILE_MAP_WRITE, 0, 0, 0);
  if (m_pView == NULL)
    {
      Stop();
      return FALSE;
    }

  m_pNext = m_pView;

  return TRUE;
}

//************************************************************************
// Stop the client
// return: TRUE if successful, FALSE otherwise
//************************************************************************
void FS6IPC_Client::Stop()
{
  m_hWnd = 0;
  m_msg = 0;
  m_pNext = 0;
  
  if (m_atom)
    {
      GlobalDeleteAtom(m_atom);
      m_atom = 0;
    }

  if (m_pView)
    {
      UnmapViewOfFile((LPVOID)m_pView);
      m_pView = 0;
    }

  if (m_hMap)
    {
      CloseHandle(m_hMap);
      m_hMap = 0;
    }

}

//************************************************************************
// Begin constructing a request to the server
// After this, call the Read/Write functions as desired, then call
// SendRequest to send the request to the server.
// return: TRUE if successful, FALSE otherwise
//************************************************************************
BOOL FS6IPC_Client::BeginRequest()
{
  if (!m_pView)
    return FALSE;

  m_pNext = m_pView;
  return TRUE;
}

//************************************************************************
// Send a request to the server
// return: TRUE if successful, FALSE otherwise
//************************************************************************
BOOL FS6IPC_Client::SendRequest()
{
  if (!m_pView)
    return FALSE;

  // terminate the request buffer
  *(DWORD*)m_pNext = 0;

  // send the request
  DWORD dwResult;
  if (!SendMessageTimeout(
    m_hWnd,       // FS6 window handle
    m_msg,        // our registered message id
    m_atom,       // wParam: name of file-mapping object
    0,            // lParam: offset of request into file-mapping obj
    SMTO_BLOCK,   // halt this thread until we get a response
    m_dwTimeout,  // time out interval
    &dwResult))   // return value
    {
      return FALSE;
    }

  // process the completed request
  m_pNext = m_pView;
  DWORD reqid;
  while (reqid = *(DWORD*)m_pNext)
    {
      switch (reqid)
        {
          case FS6IPC_READSTATEDATA_ID:
            {
              // copy the data we read into the user's buffer
              FS6IPC_READSTATEDATA_HDR* pHdr = (FS6IPC_READSTATEDATA_HDR*)m_pNext;
              m_pNext += sizeof(FS6IPC_READSTATEDATA_HDR);
              CopyMemory(pHdr->pDest, m_pNext, pHdr->nBytes);
              m_pNext += pHdr->nBytes;
              break;
            }
          
          case FS6IPC_WRITESTATEDATA_ID:
            {
              // don't need to do anything but skip over the request
              FS6IPC_WRITESTATEDATA_HDR* pHdr = (FS6IPC_WRITESTATEDATA_HDR*)m_pNext;
              m_pNext += sizeof(FS6IPC_WRITESTATEDATA_HDR) + pHdr->nBytes;
              break;
            }

          default:
            {
              // error??
              m_pNext = m_pView;
              return FALSE;
            }
        }
    }

  m_pNext = m_pView;
  return (dwResult == FS6IPC_MESSAGE_SUCCESS) ? TRUE : FALSE;
}

//************************************************************************
// Buffer a request to read state data from the server
// return: TRUE if successful, FALSE otherwise
//************************************************************************
BOOL FS6IPC_Client::ReadStateData(
  DWORD dwStateTableOffset,   // byte offset into state table
  DWORD nBytes,               // number of bytes to fetch
  void* pDest)                // buffer where fetched data will be written
{
  if (!m_pView)
    return FALSE;

  // fill the header
  FS6IPC_READSTATEDATA_HDR* pHdr = (FS6IPC_READSTATEDATA_HDR*)m_pNext;
  pHdr->dwId = FS6IPC_READSTATEDATA_ID;
  pHdr->dwOffset = dwStateTableOffset;
  pHdr->nBytes = nBytes;
  pHdr->pDest = pDest;

  // advance the pointer and leave room for the returned data
  m_pNext += sizeof(FS6IPC_READSTATEDATA_HDR) + nBytes;

  return TRUE;
}


//************************************************************************
// Buffer a request to write state data to the server
// return: TRUE if successful, FALSE otherwise
//************************************************************************
BOOL FS6IPC_Client::WriteStateData(
  DWORD dwStateTableOffset,   // byte offset into state table
  DWORD nBytes,               // number of byte to write
  void* pSrc)                 // buffer containing data to write
{
  if (!m_pView)
    return FALSE;

  // fill the header
  FS6IPC_WRITESTATEDATA_HDR* pHdr = (FS6IPC_WRITESTATEDATA_HDR*)m_pNext;
  pHdr->dwId = FS6IPC_WRITESTATEDATA_ID;
  pHdr->dwOffset = dwStateTableOffset;
  pHdr->nBytes = nBytes;

  // advance the pointer
  m_pNext += sizeof(FS6IPC_WRITESTATEDATA_HDR);

  // copy the data
  CopyMemory(m_pNext, pSrc, nBytes);

  // advance the pointer
  m_pNext += nBytes;

  return TRUE;
}


