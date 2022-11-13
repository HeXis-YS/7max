#include "StdAfx.h"

#include "InjectDLL2.h"

bool MyCreateThread(HANDLE process, LPTHREAD_START_ROUTINE threadStartAddress, 
    const void *paramData, size_t dataSize)
{
  void *p = ::VirtualAllocEx(process, 0, dataSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
  if (p == 0)
    return false;
  SIZE_T wr;
  bool res = false;
  if (::WriteProcessMemory(process, p, paramData, dataSize, &wr))
  {
    if (wr == dataSize)
    {
      DWORD id;
      HANDLE thread = ::CreateRemoteThread(process, NULL, 0, (unsigned long (__stdcall *)(void *))
          threadStartAddress, p, 0, &id);
      if (thread != 0)
      {
        if (::WaitForSingleObject(thread, INFINITE) == WAIT_OBJECT_0)
        { 
          DWORD exitCode;
          if (GetExitCodeThread(thread, &exitCode))
            res = (exitCode != 0);
        }
        ::CloseHandle(thread);
      }
    }
  }
  ::VirtualFreeEx(process, p, 0, MEM_RELEASE);
  return res;
}

static int MyStringLen2(const wchar_t *s)
{ 
  int i;
  for (i = 0; s[i] != '\0'; i++);
  return i;
}

bool InjectViaRemoteThread(HANDLE process, const wchar_t *dllPath, FARPROC loadLibraryWAddress)
{
  if (loadLibraryWAddress == 0)
    loadLibraryWAddress = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
  return MyCreateThread(process, (PTHREAD_START_ROUTINE)loadLibraryWAddress, (const wchar_t *)dllPath, (MyStringLen2(dllPath) + 1) * sizeof(dllPath[0]));
}
