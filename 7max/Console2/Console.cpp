// 7max/Console.cpp

#include "StdAfx.h"

#include "Common/StringConvert.h"

#include "../Common/ProcessStatus.h"
#include "../Common/InjectDLL3.h"
#include "../Common/7maxCommandLine.h"

#include "Windows/Security.h"
#include "Windows/MemoryLock.h"
#include "Windows/Error.h"

#include "ConsoleClose.h"
#include "../MyVersion.h"

using namespace NWindows;

HINSTANCE g_hInstance = 0;

static const char *kCopyrightString = "\n7-max"

      " "
      #ifdef _WIN64
      "64"
      #else
      "32"
      #endif
      "-bit"
" " MY_VERSION_COPYRIGHT_DATE "\n";

static const char *kHelpString = 
    "\nUsage: 7maxc64"
    " [<switches>...] [--] [commands]\n"
    "\n"
    "<Switches>\n"
    "  --: Stop switches parsing\n"
    "  -pid{ProcessId}: Inject to process with specified ProcessId\n"
    "  -vms{Size}: Check Virtual Memory limits for Small pages\n"
    "  -vml{Size}: Check Virtual Memory limits for Large pages\n"
    "  -defrag: Make defragmentation of physical memory\n";


void PrintMainString()
{
  printf(kCopyrightString);
}

const int kNumPagesMax = 1 << 16;
void *blocks[kNumPagesMax];

static LPVOID MyVirtualAlloc(SIZE_T size, bool largePages = false)
{
  return ::VirtualAlloc(0, size, MEM_COMMIT | (largePages ? MEM_LARGE_PAGES: 0), PAGE_READWRITE);
}

void PrintError(DWORD error)
{
  printf(" error = %X", error);
  UString message;
  if (NError::MyFormatMessage(error, message))
    printf(": %S", message);
  printf("\n");
  
}

static void VmTest(bool largePages, UInt64 maxVal = ((UInt64)1 << 60))
{
  #ifndef _WIN64
  if (maxVal > ((UInt64)1 << 32))
    maxVal = ((UInt64)1 << 32) - 1;
  #endif

  SIZE_T kStep;
  if (largePages)
  {
    kStep = GetLargePageMinimum();
    printf("Large Page Min Size = %d bytes\n\n", (int)kStep);
    printf("1-Page blocks:\n");
  }
  else
  {
    printf("Normal pages test\n");
    kStep = 1 << 20;
    printf("1 MB blocks:\n");
  }
  unsigned __int64 i;
  int index = 0;
  i = kStep;
  index = 0;
  for (; index < kNumPagesMax && i <= maxVal; i += kStep)
  {
    printf("%6d MB ", (int)(i >> 20));
    DWORD start = GetTickCount();
    void *res = MyVirtualAlloc((size_t)kStep, largePages);
    DWORD error = GetLastError();
    DWORD end = GetTickCount() - start;
    printf("%7d ms", end);
    if (res == 0)
    {
      PrintError(error);
      if (index == 0)
        return;
      break;
    }
    blocks[index++] = res;
    Sleep(0 + end / 8);
    printf("\n", end);
  }
  while (index != 0)
    ::VirtualFree(blocks[index--], 0, MEM_RELEASE);

  SIZE_T kStep2 = (SIZE_T)(i / 32);
  kStep2 = (kStep2 + kStep) & (~(kStep - 1));

  printf("\nLarge blocks:\n");
  for (i = kStep2; i <= maxVal; i += kStep2)
  {
    printf("%6d MB ", (int)(i >> 20));
    DWORD start = GetTickCount();
    void *res = MyVirtualAlloc((size_t)i, largePages);
    DWORD error = GetLastError();
    DWORD end = GetTickCount() - start;
    printf("%7d ms", end);
    if (res == 0)
    {
      PrintError(error);
      break;
    }
    ::VirtualFree(res, 0, MEM_RELEASE);
    Sleep(100 + end / 4);
    printf("\n", end);
  }
}

void Defrag(bool useMaxVal, UInt64 maxVal)
{
  SIZE_T kStep = 1 << 20;
  printf("RAM Defragmentation:\n");
  UInt64 totalSize = GetPhysicalRamSize();
  printf("Total RAM size = %d MB\n, \n", (unsigned int)(totalSize >> 20));
  if (!useMaxVal)
    maxVal = totalSize;
  #ifndef _WIN64
  if (maxVal > ((UInt64)1 << 32))
    maxVal = ((UInt64)1 << 32) - 1;
  #endif

  UInt64  i;
  int index = 0;
  i = kStep;
  index = 0;
  for (; index < kNumPagesMax && i <= maxVal; i += kStep)
  {
    printf("%6d MB ", (int)(i >> 20));
    void *res = ::MyVirtualAlloc(kStep, false);
    DWORD error = GetLastError();
    if (res == 0)
    {
      PrintError(error);
      if (index == 0)
        return;
      break;
    }
    Sleep(0);
    DWORD start = GetTickCount();
    for (SIZE_T i = 0; i < kStep; i += (1 << 10))
      ((BYTE *)res)[i] = 0x5A;
    DWORD end = GetTickCount() - start;
    printf("%7d ms", end);
    blocks[index++] = res;
    printf("\n");
    /*
    if (end > 200)
      break;
    */
  }
  while (index != 0)
    ::VirtualFree(blocks[index--], 0, MEM_RELEASE);
	return;
}

int __cdecl main2()
{
  PrintMainString();

  NSecurity::AddLockMemoryPrivilege();
  NSecurity::EnableLockMemoryPrivilege();

  CCtrlHandlerSetter ctrlHandler;
  ctrlHandler.EnableBreak(true);

  CCommandLineOptions options;
  {
    CCommandLineParser parser;
    parser.Parse(options);
  }

  if (options.IsEmpty() || options.HelpMode)
  {
    printf("%s", kHelpString);
    return 0;
  }

  /*
  if (options.LoadLib)
  {
    char s[32];
    FARPROC f = ::GetProcAddress(::GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
    ConvertUInt64ToString((UInt64)f, s, 16);
    printf("LoadLibraryW = %s", s);
    if ((UInt64)f <= 0xFFFFFFFF)
      return (int)f;
    return 0;
  }
  */
  
  // Check7maxConditions();
  InitProcessStatus();

  if (options.IsProcessIdDefined)
  {
    printf("\nInject process %d\n", options.ProcessId);
    if (!InjectToProcess(options.ProcessId))
      throw "can't inject";
    return 0;
  }

  if (options.VmSmall.Enabled)
    VmTest(false, (options.VmSmall.SizeIsDefined ? options.VmSmall.Size: ((UInt64)1 << 60)));
  if (options.VmLarge.Enabled)
    VmTest(true, (options.VmLarge.SizeIsDefined ? options.VmLarge.Size: ((UInt64)1 << 60)));
  if (options.Defrag.Enabled)
    Defrag(options.Defrag.SizeIsDefined, options.Defrag.Size);

  ctrlHandler.EnableBreak(false);

  StartProcessVia7max(options.Arguments, true);
  return 0;
}

int __cdecl main()
{
  try
  {
    return main2();
  }
  catch (const CSysString &s)
  {
    printf("\nError: %s\n", UnicodeStringToMultiByte(s, CP_OEMCP));
    return 1;
  }
  catch(const wchar_t *s)
  {
    printf("\nError: %s\n", UnicodeStringToMultiByte(s, CP_OEMCP));
    return 1;
  }
  catch (const char *s)
  {
    printf("\nError: %s\n", s);
    return 1;
  }
  catch (...)
  {
    printf("\nUnspecified error\n");
    return 1;
  }
  return 0;
}
