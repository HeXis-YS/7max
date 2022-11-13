// 7maxIpc.h

#pragma once

#ifndef __7MAXIPC_H
#define __7MAXIPC_H

#include "Windows/Synchronization.h"

enum CIpcCommandID
{
  kAllocate,
  kFree
};

struct CIpcInfo
{
  // DWORD ServerProcessID;
  DWORD CallerProcessID;
  // HPROCESS ManagerProcessID;
  HANDLE FinishEvent; // in Server;
  union
  {
    UINT32 CommandID;
    UINT32 ExitCode;
  };
  UINT32 Data[512];
};


#endif
