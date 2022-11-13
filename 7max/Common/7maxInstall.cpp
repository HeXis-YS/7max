// 7maxInstall.cpp

#include "StdAfx.h"

#include "Windows/Service.h"
#include "Windows/Registry.h"

#include "ProgramLocation.h"
#include "7maxInstall.h"
#include "x86CPUID.h"

using namespace NWindows;

static LPCTSTR kServiceName = TEXT("7max");
static LPCTSTR kDriverFileName = TEXT("7max.sys");

bool Start7maxService(CSysString &errorMessage)
{
  CSCManager manager;
  if (!manager.Open(SC_MANAGER_CREATE_SERVICE))
  {
    errorMessage = TEXT("Can't open Service Control Manager"); 
    return false;
  }

  CService service;
  for (int i = 0; i < 2; i++)
  {
    // printf("\nnBefore MyOpenService");
    service.Attach(manager.MyOpenService(kServiceName, SERVICE_START));
    if (service != 0)
      break;
    if (::GetLastError() != ERROR_SERVICE_DOES_NOT_EXIST)
    {
      errorMessage = TEXT("Can't open service");
      return false;
    }
    CSysString fullPathForDriver;
    if (!GetSevenMaxFilePath(kDriverFileName, fullPathForDriver))
    {
      errorMessage = CSysString(TEXT("Can't find ")) + kDriverFileName;
      return false;
    }
    // printf("\nBefore MyCreateService");
    service.Attach(manager.MyCreateService(
        kServiceName, // Service Name
        kServiceName, // Display Name
        SERVICE_ALL_ACCESS, //dwDesiredAccess,
        SERVICE_KERNEL_DRIVER, // dwServiceType,
        SERVICE_DEMAND_START, // SERVICE_AUTO_START, // dwStartType,
        SERVICE_ERROR_IGNORE, // dwErrorControl
        fullPathForDriver, // lpBinaryPathName
        NULL, // lpLoadOrderGroup
        NULL, // lpdwTagId
        NULL, // lpDependencies
        NULL, // lpServiceStartName, Check It !!!!!
        TEXT("") // lpPassword
        ));
    if (service != 0)
      break;
    if (::GetLastError() == ERROR_SERVICE_EXISTS)
      continue;
    errorMessage = TEXT("Can't create service");
    return false;
  }
  // printf("\nBefore service.Start");
  if (!service.Start())
  {
    if (::GetLastError() != ERROR_SERVICE_ALREADY_RUNNING)
    {
      errorMessage = TEXT("Can't start service");
      return false;
    }
  }
  // printf("\nBefore return OK");
  return true;
}

static LPCTSTR kPAEKeyPath = 
  TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management");
static LPCTSTR kPAEValueName = TEXT("PhysicalAddressExtension");

bool CheckPAE()
{
  NRegistry::CKey key;
  UINT32 value;
  if(key.Open(HKEY_LOCAL_MACHINE, kPAEKeyPath, KEY_READ) == ERROR_SUCCESS)
    if (key.QueryValue(kPAEValueName, value) == ERROR_SUCCESS)
      return value != 0;
  return false;
}

void Check7maxConditions()
{
  OSVERSIONINFOA versionInfo;
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (!::GetVersionExA(&versionInfo)) 
    throw "Error #031022";
  if (versionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT ||
      versionInfo.dwMajorVersion != 5 || 
      versionInfo.dwMinorVersion > 1)
    throw "7-max works only in Windows 2000/XP";
  
  if (!NCPUID::IsPSESupported())
    throw "7-max works on Pentium or higher processor";

  CSysString errorMessage;
  if(!Start7maxService(errorMessage))
    throw errorMessage;
  /*
  if (CheckPAE())
    throw "7-max doesn't work in PAE mode";
  */
}

bool Check3GBMode()
{
  SYSTEM_INFO sysInfo;
  ::GetSystemInfo(&sysInfo);
  return ((UINT32)sysInfo.lpMaximumApplicationAddress > 0x80000000);
}
