// InjectDLL3.cpp

#include "StdAfx.h"

#include "InjectDLL3.h"

#include "Common/IntToString.h"
#include "Windows/Process.h"

#include "ProcessStatus.h"
#include "InjectDLL2.h"
#include "ProgramLocation.h"

using namespace NWindows;

static bool MyCreateProcess(TCHAR *cmdLine, bool suspend, PROCESS_INFORMATION *pi)
{
  STARTUPINFO si;
  ::ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  return ::CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, suspend ? CREATE_SUSPENDED : 0, NULL, NULL, &si, pi) != FALSE;
}
  
static bool InjectToProcess(HANDLE hProcess, DWORD processId)
{
  bool remoteProcessIs64 = Is64BitProcess(hProcess);
  if (g_IsProcess64 == remoteProcessIs64)
    return InjectViaRemoteThread(hProcess, g_DllPath, g_LoadLibAddress);
  if (g_IsProcess64 == remoteProcessIs64)
    return false;
  TCHAR commandLine[kPathNames + 100];
  lstrcpy(commandLine, remoteProcessIs64 ? g_GuiExePath64: g_GuiExePath32);
  lstrcat(commandLine, L" -pid");
  wchar_t id[32];
  ConvertUInt64ToString(processId, id);
  lstrcat(commandLine, id);
  
  PROCESS_INFORMATION pi;
  bool result = ::MyCreateProcess(commandLine, false, &pi);
  ::CloseHandle(pi.hThread);
  ::WaitForSingleObject(pi.hProcess, INFINITE);
  ::CloseHandle(pi.hProcess);
  return result;
}

bool StartProcessVia7max(const UString &command, bool wait)
{
  TCHAR *commandLine = new TCHAR[command.Length() + 300];
  lstrcpy(commandLine, command);
  
  PROCESS_INFORMATION pi;
  BOOL result = ::MyCreateProcess(commandLine, true, &pi);
  delete []commandLine;
  if (!result)
    return false;

  try
  {
    InjectToProcess(pi.hProcess, pi.dwProcessId);
  }
  catch(...)
  {
    ::CloseHandle(pi.hThread);
    ::CloseHandle(pi.hProcess);
    throw;
  }
  ::ResumeThread(pi.hThread);
  ::CloseHandle(pi.hThread);
  if (wait)
    ::WaitForSingleObject(pi.hProcess, INFINITE);
  ::CloseHandle(pi.hProcess);
  return true;
}

bool InjectToProcess(DWORD processID)
{
  CProcess process;
  if (!process.Open(PROCESS_ALL_ACCESS, FALSE, processID))
    return false;
  return InjectToProcess(process, processID);
}
