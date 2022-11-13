// DLL.cpp

#include "stdafx.h"

#include <SubAuth.h>

#include "Windows/Defs.h"
#include "Windows/MemoryLock.h"

#include "../Common/7maxRegistry.h"
#include "../Common/InjectDll2.h"
#include "../Common/ProcessStatus.h"
#include "Common/IntToString.h"

using namespace NWindows;

static UInt32 g_AllocMin = (1 << 20);

HINSTANCE g_hInstance;

#pragma pack( push, 1)

static const Byte kNopInstr = 0x90;
static const Byte kJump32Instr = 0xE9;

struct CJump
{
  BYTE JumpInstr;
  CJump(): JumpInstr(kJump32Instr) {}
};

struct CNop
{
  BYTE NopInstr;
  CNop(): NopInstr(kNopInstr) {}
};

struct CJumpCode
{
  CJump JumpInstr;
  UINT32 JumpOffset;
};

/*
struct CStubFunc
{
  BYTE OldCode[kOldCodeSizeMax];
  BYTE JumpInstr;
  UINT32 JumpOffset;
};
*/

#pragma pack(pop)

const UInt32 kOldCodeSizeMax = 10;
const UInt32 kJumpInstSize = 5;
const UInt32 kStubSizeMax = kOldCodeSizeMax + kJumpInstSize;

struct CStubInfo
{
  Byte *Stub;
  UInt32 OldCodeSize;
  bool NeedRestore;
  FARPROC OldFunc;
  void Init()
  {
    Stub = 0;
    OldCodeSize = 0;
    OldFunc = 0;
    NeedRestore = false;
  }
};

/*
void PrintMem(Byte *p, int size)
{
  char sz[1000];
  sz[0] = 0;
  for (int i = 0; i < size; i++)
  {
    itoa((unsigned int)p[i], sz + strlen(sz), 16);
    strcat(sz , " ");
  }
  OutputDebugStringA(sz);
}
*/

static bool InterceptFunc(CStubInfo &stubInfo, FARPROC myFunc)
{
  stubInfo.OldCodeSize = kOldCodeSizeMax;
  Byte *stub = stubInfo.Stub;
  SIZE_T written;
  if (!::ReadProcessMemory(::GetCurrentProcess(), stubInfo.OldFunc, stub, 
      stubInfo.OldCodeSize, &written))
    return false;
  stubInfo.OldCodeSize = written;
  // PrintMem(stub, stubInfo.OldCodeSize);
  if (stubInfo.OldCodeSize >= 5 && stub[0] == 0xB8)
  {
    // B8 im32 : mov	EAX, im32 // XP64::CreateProcess, XP32, XP64_32, Vista64_32 ::VirtaulAlloc
    stubInfo.OldCodeSize = 5;
  }
  else if (stubInfo.OldCodeSize >= 5 && stub[0] == 0x68)
  {
    // 68 im32 : push Im32; // XP32, XP64_32::CreateProcess
    stubInfo.OldCodeSize = 5;
  }
  else if (stubInfo.OldCodeSize >= 5 && 
     (stub[0] == 0x8B &&  
      stub[1] == 0xFF &&  
      stub[2] == 0x53 &&  
      stub[3] == 0x8B &&  
      stub[4] == 0xDC))
  {
    // 8B FF : mov	EDI, EDI // Vista64_32 ::CreateProcess
    // 53    : push EBX
    // 8B DC : mov	EBX, ESP;
    stubInfo.OldCodeSize = 5;
  }
  else if (stubInfo.OldCodeSize >= 8 && 
     (stub[0] == 0x4C && 
      stub[1] == 0x8B && 
      stub[2] == 0xD1 && 
      stub[3] == 0xB8))
  {
    // Vista64, XP64::VirtaulAlloc
    // 4C 8B D1: mov R10, RCX
    // B8 im32 : move EAX, im32
    stubInfo.OldCodeSize = 8;
  }
  else if (stubInfo.OldCodeSize >= 8 && 
     (stub[0] == 0x4C && 
      stub[1] == 0x8B && 
      stub[2] == 0xDC && 
      stub[3] == 0x48 && 
      stub[4] == 0x81 && 
      stub[5] == 0xEC))
  {
    // Vista64::CreateProcess
    // 4C 8B DC      : mov R11, RSP
    // 48 81 EC im32 : sub RSP, im32
    stubInfo.OldCodeSize = 10;
  }
  else
    return false;
  UInt32 i = stubInfo.OldCodeSize;
  stub[i++] = kJump32Instr;
  *(UINT32 *)(stub + i) = (UINT32)(((BYTE *)stubInfo.OldFunc) - ((BYTE *)(stub) + kJumpInstSize));
  CJumpCode jumpCode;
  jumpCode.JumpOffset = (UINT32)((BYTE *)myFunc - ((BYTE *)stubInfo.OldFunc + sizeof(CJumpCode)));
  if (!::WriteProcessMemory(GetCurrentProcess(), 
      (void*)stubInfo.OldFunc, (void*)&jumpCode, sizeof(jumpCode), &written))
    return false;
  stubInfo.NeedRestore = true;
  FlushInstructionCache(::GetCurrentProcess(), 0, 0);
  return true;
}

static bool RestoreOneFunc(CStubInfo &stubInfo)
{
  if (!stubInfo.NeedRestore)
    return true;
  SIZE_T written;
  if (!::WriteProcessMemory(GetCurrentProcess(), 
      (LPVOID)stubInfo.OldFunc, (LPVOID)stubInfo.Stub, stubInfo.OldCodeSize, &written))
    return false;
  stubInfo.NeedRestore = false;
  FlushInstructionCache(::GetCurrentProcess(), 0, 0);
  return true;
}

typedef LONG (WINAPI * NtAllocateVirtualMemoryP)(
    HANDLE process, LPVOID *address, ULONG zeroBits, 
    SIZE_T *size, ULONG allocationType, ULONG protect);

typedef BOOL (WINAPI * CreateProcessInternalXPP)(
  PVOID unknown1,
  LPCTSTR lpApplicationName,
  LPTSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCTSTR lpCurrentDirectory,
  LPSTARTUPINFO lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation,
  PVOID unknown2
);

static CStubInfo g_AllocStubInfo;
static CStubInfo g_CreateProcessStubInfo;

SIZE_T g_LargePageSizeMask =
#ifdef _WIN64
(1 << 21)
#else
(1 << 22)
#endif
- 1;


LONG WINAPI MyNtAllocateVirtualMemory2(
    HANDLE process, LPVOID *address, ULONG zeroBits, 
    SIZE_T *size, ULONG allocationType, ULONG protect)
{
  if (process == HANDLE(-1) && address != 0 && *address == 0 && 
      zeroBits == 0 && size != 0 && *size != 0 &&
      (
        (allocationType == MEM_COMMIT) || 
        (allocationType == (MEM_COMMIT | MEM_RESERVE))
      )
      &&
      (
        (protect == PAGE_READWRITE) || 
        (protect == (PAGE_READWRITE | PAGE_EXECUTE_READWRITE))
      )
      && *size >= g_AllocMin
      // && *size <= (512 << 20)
    )
  {
    SIZE_T sizeRemember = *size;

    SIZE_T size2 = (*size + g_LargePageSizeMask) & ~g_LargePageSizeMask;

    LONG res = ((NtAllocateVirtualMemoryP)g_AllocStubInfo.Stub)(process, 
      address, zeroBits, &size2, allocationType 
      | MEM_LARGE_PAGES
      , protect);
    if (res == 0)
    {
      *size = size2;
      return res;
    }
    *size = sizeRemember;
  }
  return ((NtAllocateVirtualMemoryP)g_AllocStubInfo.Stub)(process, 
      address, zeroBits, size, allocationType, protect);
}

BOOL WINAPI MyCreateProcessInternalXP(
  PVOID unknown1,
  LPCTSTR lpApplicationName,
  LPTSTR lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles,
  DWORD dwCreationFlags,
  LPVOID lpEnvironment,
  LPCTSTR lpCurrentDirectory,
  LPSTARTUPINFO lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation,
  PVOID unknown2)
{
  // OutputDebugStringA("MyCreateProcessInternalXP");
  bool canInject = false;
  bool needResume = (dwCreationFlags & CREATE_SUSPENDED) == 0;
  if ((dwCreationFlags & DEBUG_PROCESS) == 0 &&
    (dwCreationFlags & DEBUG_ONLY_THIS_PROCESS) == 0)
  {
    canInject = true;
    dwCreationFlags |= CREATE_SUSPENDED;
  }
  BOOL res = ((CreateProcessInternalXPP)g_CreateProcessStubInfo.Stub)(
      unknown1,
      lpApplicationName,
      lpCommandLine,
      lpProcessAttributes,
      lpThreadAttributes,
      bInheritHandles,
      dwCreationFlags,
      lpEnvironment,
      lpCurrentDirectory,
      lpStartupInfo,
      lpProcessInformation,
      unknown2);
  if (res && canInject)
  {
    DWORD lastError = ::GetLastError();
    bool remoteProcessIs64 = Is64BitProcess(lpProcessInformation->hProcess);

    if (g_IsProcess64 != remoteProcessIs64)
    {
      STARTUPINFO si;
      si.cb = sizeof(si);
      si.lpReserved = si.lpDesktop = si.lpTitle = 0;
      si.dwFlags = 0;
      si.cbReserved2 = 0;
      si.lpReserved2 = 0;

      PROCESS_INFORMATION pi;

      const int idPos = 9;
      WCHAR id[idPos + 10 + 1];
      id[0] = L'M';
      id[1] = L' ';
      id[2] = L'-';
      id[3] = L's';
      id[4] = L' ';
      id[5] = L'-';
      id[6] = L'p';
      id[7] = L'i';
      id[8] = L'd';
      ConvertUInt64ToString(lpProcessInformation->dwProcessId, id + idPos);
      BOOL res2 = ((CreateProcessInternalXPP)g_CreateProcessStubInfo.Stub)(
        NULL, remoteProcessIs64 ? g_GuiExePath64: g_GuiExePath32, id, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi, NULL);
      ::CloseHandle(pi.hThread);
      ::WaitForSingleObject(pi.hProcess, INFINITE);
      ::CloseHandle(pi.hProcess);
    }
    else
    {
      InjectViaRemoteThread(lpProcessInformation->hProcess, g_DllPath, g_LoadLibAddress);
    }
    if (needResume)
      ::ResumeThread(lpProcessInformation->hThread);
    ::SetLastError(lastError);
  }
  return res;
}

class CFuncIntercepter
{
public:
  ~CFuncIntercepter() { RestoreIntercept(); }
  
  bool Intercept()
  {
    if (!g_AllocStubInfo.NeedRestore)
    {
      HMODULE module = GetModuleHandle(TEXT("ntdll.dll"));
      if (module != 0)
      {
        g_AllocStubInfo.OldFunc = ::GetProcAddress(module, "NtAllocateVirtualMemory");
        if (g_AllocStubInfo.OldFunc != 0)
        {
          InterceptFunc(g_AllocStubInfo, (FARPROC)MyNtAllocateVirtualMemory2);
        }
      }
    }
    if (!g_AllocStubInfo.NeedRestore)
    {
      // OutputDebugStringA("!_allocChanged\n");
      RestoreIntercept();
      return false;
    }
    // return true;
    if (!g_CreateProcessStubInfo.NeedRestore)
    {
      HMODULE module = GetModuleHandle(TEXT("kernel32.dll"));
      if (module != 0)
      {
        g_CreateProcessStubInfo.OldFunc = ::GetProcAddress(module, "CreateProcessInternalW");
        if (g_CreateProcessStubInfo.OldFunc != 0)
        {
          // OutputDebugStringA("InterceptFunc");
          InterceptFunc(g_CreateProcessStubInfo, (FARPROC)MyCreateProcessInternalXP);
        }
      }
    }
    return true;
  }

  void RestoreIntercept()
  {
    RestoreOneFunc(g_AllocStubInfo);
    RestoreOneFunc(g_CreateProcessStubInfo);
  }
} g_Intercepter;

static void *MyAllocateForStub(SIZE_T size)
  { return ::VirtualAlloc(0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);}

BOOL WINAPI Intercept()
{
  g_AllocStubInfo.Stub = (Byte *)MyAllocateForStub(kStubSizeMax);
  g_CreateProcessStubInfo.Stub = (Byte *)MyAllocateForStub(kStubSizeMax);
  if(g_AllocStubInfo.Stub == NULL || g_CreateProcessStubInfo.Stub == NULL)
    return FALSE;
  return BoolToBOOL(g_Intercepter.Intercept());
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    {
      g_hInstance = (HINSTANCE)hModule;
      
      OSVERSIONINFOA versionInfo;
      versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
      if (!::GetVersionExA(&versionInfo)) 
        return FALSE;
      if (versionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT ||
          versionInfo.dwMajorVersion < 5 || 
          versionInfo.dwMajorVersion == 5 && versionInfo.dwMinorVersion < 1)
        return FALSE;
      g_AllocStubInfo.Init();
      g_CreateProcessStubInfo.Init();

      if (!NSecurity::EnableLockMemoryPrivilege())
        return FALSE;
      g_AllocMin = ReadAllocMin();
      
      SIZE_T size = GetLargePageMinimum();
      if (size == 0 || (size & (size - 1)) != 0)
        return FALSE;
      g_LargePageSizeMask = size - 1;

      InitProcessStatus();
      // OutputDebugStringW(g_DllPath);

      if (!Intercept())
      {
        // OutputDebugStringA("!g_Intercepter.Intercept\n");
      }
      // OutputDebugStringA("Intercepter.Intercept OK\n");
      break;
    }
    case DLL_PROCESS_DETACH:
      g_Intercepter.RestoreIntercept();
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
  }
  return TRUE;
}
