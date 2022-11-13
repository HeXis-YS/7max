// x86CPUID.h

#include "Stdafx.h"
#include "x86CPUID.h"

namespace NCPUID {

inline bool IsCPUIDSupported()
{
  UINT32 flag;
  __asm pushfd;
  __asm pop eax;
  __asm mov ebx, eax;
  __asm xor eax, 00200000h;
  __asm push eax; 
  __asm popfd; 
  __asm pushfd;
  __asm pop eax; 
  __asm xor eax, ebx;
  __asm and eax, 00200000h;
  __asm mov flag, eax;
  return (flag != 0);
}

static const UINT32 kFlagPSE = 1 << 3;
static const UINT32 kFlagMTRR = 1 << 12;
static const UINT32 kFlagPSE36 = 1 << 17;
struct CPUInfo
{
  UINT32 HighestFunctionCode;
  UINT32 VendorID[3];
  UINT32 Version;
  UINT32 Func1BX;
  UINT32 Features;
  
  void ExecuteCPUID(UINT32 function, UINT32 &a, UINT32 &b, UINT32 &c, UINT32 &d)
  {
    UINT32 a2, b2, c2, d2;
    __asm push EAX;
    __asm push EBX;
    __asm push ECX;
    __asm push EDX;
    __asm mov EAX, function;
    __asm cpuid;
    __asm mov a2, EAX;
    __asm mov b2, EBX;
    __asm mov c2, ECX;
    __asm mov d2, EDX;
    __asm pop EDX;
    __asm pop ECX;
    __asm pop EBX;
    __asm pop EAX;
    a = a2;
    b = b2;
    c = c2;
    d = d2;
  }
  void ReadInfo()
  {
    UINT32 c;
    ExecuteCPUID(0, HighestFunctionCode, VendorID[0], VendorID[2], VendorID[1]);
    ExecuteCPUID(1, Version, Func1BX, c, Features);
  }

  bool CheckFlag(UINT32 flag) const { return (Features & flag) != 0; }
  bool IsMTRRSupported() const { return CheckFlag(kFlagMTRR); }
  bool IsPSESupported() const { return CheckFlag(kFlagPSE); }
  bool IsPSE36Supported() const { return CheckFlag(kFlagPSE36); }
};

bool IsMTRRSupported()
{
  if (!IsCPUIDSupported())
    return false;
  CPUInfo cpuInfo;
  cpuInfo.ReadInfo();
  return cpuInfo.IsMTRRSupported();
}

bool IsPSESupported()
{
  if (!IsCPUIDSupported())
    return false;
  CPUInfo cpuInfo;
  cpuInfo.ReadInfo();
  return cpuInfo.IsPSESupported();
}

bool IsPSE36Supported()
{
  if (!IsCPUIDSupported())
    return false;
  CPUInfo cpuInfo;
  cpuInfo.ReadInfo();
  return cpuInfo.IsPSE36Supported();
}

}