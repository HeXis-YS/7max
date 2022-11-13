// InjectDLL.cpp

#include "StdAfx.h"

#include "Windows/Synchronization.h"
#include "InjectDLL1.h"

using namespace NWindows;

static bool InjectDllRemote32(HANDLE hProcess, const wchar_t *dllPath)
{
  HANDLE duplicateHandle;
  NSynchronization::CManualResetEvent loadFinishEvent;
  if (!::DuplicateHandle(GetCurrentProcess(), loadFinishEvent,
      hProcess, &duplicateHandle,      
      0, // dwDesiredAccess,
      FALSE, // bInheritHandle,
      DUPLICATE_SAME_ACCESS // dwOptions
      ))
    return false;
  CInjectBlock32 injectBlock;
  injectBlock.Init();
  injectBlock.SetEventCall.Param = (UINT32)duplicateHandle;
  DWORD nameLen = (lstrlen(dllPath) + 1) * 2;
  DWORD blockSize = sizeof(injectBlock) + nameLen;
  BYTE *p = (BYTE*)::VirtualAllocEx(hProcess, 
      0, blockSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if(p == NULL)
    return false;
  void *remotePath = p + sizeof(injectBlock);
  injectBlock.LoadLibraryWCall.Param = (UINT32)remotePath;
  SIZE_T wr;
  if (!::WriteProcessMemory(hProcess, p, &injectBlock, sizeof(injectBlock), &wr))
    return false;
  if (!::WriteProcessMemory(hProcess, remotePath, dllPath, nameLen, &wr))
    return false;
  ::FlushInstructionCache(hProcess, 0, 0);
  DWORD id;
  HANDLE thread = ::CreateRemoteThread(hProcess, NULL, 0, 
      (unsigned long (__stdcall *)(void *))p, 0, 0, &id);
  if (thread == 0)
    return false;
  ::CloseHandle(thread);
  loadFinishEvent.Lock();
  return true;
}

static bool InjectDllRemote64(HANDLE hProcess, const wchar_t *dllPath)
{
  HANDLE duplicateHandle;
  NSynchronization::CManualResetEvent loadFinishEvent;
  if (!::DuplicateHandle(GetCurrentProcess(), loadFinishEvent,
      hProcess, &duplicateHandle,      
      0, // dwDesiredAccess,
      FALSE, // bInheritHandle,
      DUPLICATE_SAME_ACCESS // dwOptions
      ))
    return false;
  CInjectBlock64 injectBlock;
  injectBlock.Init();
  injectBlock.SetEventCall.Param = (UINT64)duplicateHandle;
  DWORD nameLen = (lstrlen(dllPath) + 1) * 2;
  DWORD blockSize = sizeof(injectBlock) + nameLen;
  BYTE *p = (BYTE*)::VirtualAllocEx(hProcess, 
      0, blockSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if(p == NULL)
    return false;
  void *remotePath = p + sizeof(injectBlock);
  injectBlock.LoadLibraryWCall.Param = (UINT64)remotePath;
  SIZE_T wr;
  if (!::WriteProcessMemory(hProcess, p, &injectBlock, sizeof(injectBlock), &wr))
    return false;
  if (!::WriteProcessMemory(hProcess, remotePath, dllPath, nameLen, &wr))
    return false;
  ::FlushInstructionCache(hProcess, 0, 0);
  DWORD id;
  HANDLE thread = ::CreateRemoteThread(hProcess, NULL, 0, 
      (unsigned long (__stdcall *)(void *))p, 0, 0, &id);
  if (thread == 0)
    return false;
  ::CloseHandle(thread);
  loadFinishEvent.Lock();
  return true;
}

bool InjectDllRemote(HANDLE hProcess, const wchar_t *dllPath)
{
  if (g_IsProcess64 != Is64BitProcess(hProcess))
    return false;
  if (Is64BitProcess(hProcess))
    return InjectDllRemote64(hProcess, dllPath);
  return InjectDllRemote32(hProcess, dllPath);
}
