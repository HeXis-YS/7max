// 7maxInstall.cpp

#include "StdAfx.h"

#include "7maxInstall.h"

void Check7maxConditions()
{
  OSVERSIONINFOA versionInfo;
  versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
  if (!::GetVersionExA(&versionInfo)) 
    throw "Error #031022";
  if (versionInfo.dwPlatformId != VER_PLATFORM_WIN32_NT ||
      versionInfo.dwMajorVersion < 5 || 
      versionInfo.dwMajorVersion == 5 &&
      versionInfo.dwMinorVersion < 2)
    throw "7-max works only in Windows 2003, Windows XP x64 and Windows Vista";
}
