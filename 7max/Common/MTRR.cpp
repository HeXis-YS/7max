// MTRR.cpp

#include "StdAfx.h"

#include "MTRR.h"

namespace NMTRR {

bool CInfo::ReadInfo(IReadMSR * readMSR)
{
  if (!readMSR->ReadMSR(kDefCapReg , Cap))
    return false;
  if (!readMSR->ReadMSR(kDefTypeReg , Type))
    return false;
  if (!readMSR->ReadMSR(kFix64K_0000, *(UINT64*)Fix64K))
    return false;
  UINT32 i;
  for (i = 0; i < kNumFix16KRegs; i++)
    if (!readMSR->ReadMSR(kFix16K_8000 + i, *(((UINT64*)Fix16K) + i)))
      return false;
  for (i = 0; i < kNumFix4KRegs; i++)
    if (!readMSR->ReadMSR(kFix4K_C000 + i, *(((UINT64*)Fix4K) + i)))
      return false;
  for (i = 0; i < GetNumberOfVarRegs() && i < kNumVarRegsMax; i++)
  {
    if (!readMSR->ReadMSR(kPhysBase0 + i * 2, VarRegs[i].Base))
      return false;
    if (!readMSR->ReadMSR(kPhysMask0 + i * 2, VarRegs[i].Mask))
      return false;
  }
  return true;
}

}





