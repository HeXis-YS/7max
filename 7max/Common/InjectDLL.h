// InjectDLL.h

#ifndef __INJECTDLL_H
#define __INJECTDLL_H

#include "Common/String.h"
#include "Windows/Process.h"
#include "../Common/7maxDevice.h"

class CDebugInjectorProcess
{
  C7maxDevice *_device;
  bool _injected; // have tried to inject
  bool _firstBP;
  CREATE_PROCESS_DEBUG_INFO _createProcessDebugInfo;
  // NWindows::CProcess _process;
  // NWindows::CHandle _thread;
  // DWORD _threadID;
  // DWORD _processID;
  CONTEXT _originalThreadContext;
  void *_stubInTarget;
  void *_stubInTargetBP;

  HANDLE _process;
  HANDLE _thread;
  DWORD _threadID;

  UString _dllPath;

  void *_exeEntryPoint;
  BYTE  _originalExeEntryPointOpcode;

  void ReadTargetMemory(const void *base, void *buf, DWORD size);
  void WriteTargetMemory(void *base, const void *buf, DWORD size);
  bool SaveEntryPointContext(DEBUG_EVENT & dbgEvent);

public:
  bool _dllWasInjected; // real injected (3 GB and other checks were OK).
  virtual DWORD ProcessEvent(DEBUG_EVENT &debugEv);
  CDebugInjectorProcess();
  void Init(C7maxDevice *device, const CSysString &dllPath);
};

struct CDebugPair
{
  DWORD ProcessId;
  CDebugInjectorProcess ProcessInfo;
};

class CDebugger
{
  CObjectVector<CDebugPair> _map;
public:
  bool Debug(const UString &command, DWORD processID = 0);
};

#endif
