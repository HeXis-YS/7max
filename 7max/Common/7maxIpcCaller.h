// 7maxIpcCaller.h

#pragma once

#ifndef __7MAXIPCCALLER_H
#define __7MAXIPCCALLER_H

#include "7maxIpc.h"

extern NWindows::NSynchronization::CMutex g_StartMutex; 
extern NWindows::NSynchronization::CBaseEvent g_StartEvent;
extern NWindows::NSynchronization::CAutoResetEvent g_FinishEvent;
extern NWindows::NSynchronization::CCriticalSection g_ProcessCS;

extern CIpcInfo *g_IpcInfo;

class C7maxIpc
{
public:
  bool Allocate(UINT32 numPages, UINT32 *pfnArray)
  {
    g_StartMutex.Lock();
    g_ProcessCS.Enter();
    g_IpcInfo->CallerProcessID = ::GetCurrentProcessId();
    g_IpcInfo->FinishEvent = g_FinishEvent;
    g_IpcInfo->CommandID = kAllocate;
    g_StartEvent.Set();
    g_FinishEvent.Lock();
    for (UINT32 i = 0; i < numPages; i++)
      pfnArray[i] = g_IpcInfo->Data[i];
    bool res = (g_IpcInfo->ExitCode == 0);
    g_ProcessCS.Leave();
    g_StartMutex.Release();
    return res;
  }
  bool Free(UINT32 numPages, const UINT32 *pfnArray)
  {
    g_StartMutex.Lock();
    g_ProcessCS.Enter();
    g_IpcInfo->CallerProcessID = ::GetCurrentProcessId();
    g_IpcInfo->FinishEvent = g_FinishEvent;
    g_IpcInfo->CommandID = kFree;
    for (UINT32 i = 0; i < numPages; i++)
      g_IpcInfo->Data[i] = pfnArray[i];
    g_StartEvent.Set();
    g_FinishEvent.Lock();
    bool res = (g_IpcInfo->ExitCode == 0);
    g_ProcessCS.Leave();
    g_StartMutex.Release();
    return res;
  }
};

extern C7maxIpc g_7maxIpc;
bool Init7maxIpc();


#endif
