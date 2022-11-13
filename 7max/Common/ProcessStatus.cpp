// ProcessStatus.cpp

#include "StdAfx.h"
#include "ProcessStatus.h"

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

static LPFN_ISWOW64PROCESS g_fnIsWow64Process = NULL;
bool g_IsSystem64 = true;
bool g_IsProcess64 = true;

static bool MyIsWow64Process(HANDLE process)
{
  if (g_fnIsWow64Process == NULL)
    return false;
  BOOL isWow;
  if (!g_fnIsWow64Process(process, &isWow))
    return false;
  return isWow != FALSE;
}

bool Is64BitProcess(HANDLE process)
{
  if (g_IsSystem64)
    return !MyIsWow64Process(process);
  return false;
}

TCHAR g_DllPath32[kPathNames];
TCHAR g_DllPath64[kPathNames];
TCHAR g_DllPath[kPathNames];

TCHAR g_GuiExePath32[kPathNames];
TCHAR g_GuiExePath64[kPathNames];

TCHAR g_ConExePath32[kPathNames];
TCHAR g_ConExePath64[kPathNames];

extern HINSTANCE g_hInstance;

// FARPROC g_LoadLibAddress32 = 0;
// FARPROC g_LoadLibAddress64 = 0;
FARPROC g_LoadLibAddress = 0;

/*
static void GetDllAddress()
{
  TCHAR commandLine[kPathNames + 20];
  lstrcpy(commandLine, g_IsProcess64 ? g_ExePath32 : g_ExePath64);
  lstrcat(commandLine, L" -lla");
  STARTUPINFO si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi;
  BOOL result = ::CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
  if (!result)
    return;
  ::CloseHandle(pi.hThread);
  ::WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD exitCode;
  if (GetExitCodeProcess(pi.hProcess, &exitCode))
  {
    if (g_IsProcess64)
      g_LoadLibAddress32 = (FARPROC)exitCode;
    else
      g_LoadLibAddress64 = (FARPROC)exitCode;
  }
  ::CloseHandle(pi.hProcess);
}
*/

void InitProcessStatus()
{
  HMODULE kernelHandle = GetModuleHandle(L"kernel32");
  g_fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(kernelHandle, "IsWow64Process");
  #ifndef _WIN64
  g_IsSystem64 = MyIsWow64Process(GetCurrentProcess());
  g_IsProcess64 = Is64BitProcess(GetCurrentProcess());
  #endif

  ::GetModuleFileName(g_hInstance, g_DllPath32, MAX_PATH);
  for (int i = lstrlen(g_DllPath32) - 1; i >= 0; i--)
    if (g_DllPath32[i] == '\\')
    {
      lstrcpy(g_DllPath32 + i + 1, L"7max");
      lstrcpy(g_DllPath64, g_DllPath32);
      lstrcpy(g_GuiExePath32, g_DllPath32);
      lstrcpy(g_GuiExePath64, g_DllPath32);
      lstrcpy(g_ConExePath32, g_DllPath32);
      lstrcpy(g_ConExePath64, g_DllPath32);
      lstrcat(g_GuiExePath32, L"32.exe");
      lstrcat(g_GuiExePath64, L"64.exe");
      lstrcat(g_ConExePath32, L"c32.exe");
      lstrcat(g_ConExePath64, L"c64.exe");
      lstrcat(g_DllPath32, L"32.dll");
      lstrcat(g_DllPath64, L"64.dll");
      lstrcat(g_DllPath, 
      #ifdef _WIN64
      g_DllPath64
      #else
      g_DllPath32
      #endif
      );
      break;
    }
  // GetDllAddress();
  FARPROC proc = GetProcAddress(kernelHandle, "LoadLibraryW");
  /*
  if (g_IsProcess64)
    g_LoadLibAddress64 = proc;
  else
    g_LoadLibAddress32 = proc;
  */
  g_LoadLibAddress = proc;
}

UInt64 GetPhysicalRamSize()
{
  MEMORYSTATUSEX g_statex;
  g_statex.dwLength = sizeof(g_statex);
  if (!::GlobalMemoryStatusEx(&g_statex))
    return 0;
  return g_statex.ullTotalPhys;
}
