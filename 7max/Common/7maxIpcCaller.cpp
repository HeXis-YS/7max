// 7maxIpcCaller.cpp

#include "StdAfx.h"

#include "7maxIpcCaller.h"
#include "Windows/FileMapping.h"

using namespace NWindows;

NSynchronization::CMutex g_StartMutex;
NSynchronization::CBaseEvent g_StartEvent;
NSynchronization::CAutoResetEvent g_FinishEvent;
NSynchronization::CCriticalSection g_ProcessCS;

static CFileMapping g_fileMapping;
CIpcInfo *g_IpcInfo;
C7maxIpc g_7maxIpc;

static LPCTSTR kIpcFileMappingName = TEXT("7maxIPCFileMapping");
static LPCTSTR kIpcMutexName = TEXT("7maxIPCMutex");
static LPCTSTR kIpcStartEventName = TEXT("7maxIPCStartEvent");

bool Init7maxIpc()
{
  const UINT32 kTotalSize = sizeof(CIpcInfo);
  if (!g_fileMapping.Open(PAGE_READWRITE, kTotalSize, kIpcFileMappingName))
    return false;
  g_IpcInfo = (CIpcInfo *)g_fileMapping.MapViewOfFile(FILE_MAP_WRITE, 0, kTotalSize);
  if (g_IpcInfo != NULL)
  {
    if (g_StartMutex.Open(MUTEX_MODIFY_STATE, FALSE, kIpcMutexName))
    {
      if (g_StartEvent.Open(EVENT_MODIFY_STATE, FALSE, kIpcStartEventName))
        return true;
      g_StartMutex.Close();
    }
    UnmapViewOfFile(g_IpcInfo);
  }
  g_fileMapping.Close();
  return false;
}
