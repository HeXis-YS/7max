// MTRR.h

#ifndef __MTRR_H
#define __MTRR_H

namespace NMTRR {

const UINT32 kPhysBase0 = 0x200;
const UINT32 kPhysMask0 = 0x201;
const UINT32 kNumVarRegsMax = 8; 

const UINT32 kFix64K_0000 = 0x250;
const UINT32 kNumFix64KItems = 1 * 8;

const UINT32 kFix16K_8000 = 0x258;
// const UINT32 kFix16K_A000 = 0x259;
const UINT32 kNumFix16KRegs = 2;
const UINT32 kNumFix16KItems = kNumFix16KRegs * 8;

const UINT32 kFix4K_C000 = 0x268;
const UINT32 kNumFix4KRegs = 8;
const UINT32 kNumFix4KItems = kNumFix4KRegs * 8;

const UINT32 kNumFixItems = kNumFix64KItems + kNumFix16KItems + kNumFix4KItems;

const UINT32 kDefCapReg  = 0xFE;
const UINT32 kDefTypeReg = 0x2FF;

enum EType
{
  kUnCacheable = 0,
  kWriteCombining = 1,
  
  kWriteThrough = 4,
  kWriteProtected = 5,
  kWriteBack = 6
};

struct IReadMSR
{
  virtual bool ReadMSR(UINT32 address, UINT64 &result) = 0;
};

struct CVarReg
{
  UINT64 Base;
  UINT64 Mask;
  BYTE GetType() const { return BYTE(Base & 0xFF); }
  UINT64 GetBase() const { return Base & 0xFFFFFFFFFFFFF000; }
  UINT64 GetMask() const { return Mask & 0xFFFFFFFFFFFFF000; }
  bool IsValid() const { return (Mask & (1 << 11)) != 0; }
  UINT64 GetStart() const { return GetBase() & GetMask(); }
  UINT64 GetSize() const
  { 
    int startBit = 12;
    for (;startBit < 64; startBit++)
      if (((Mask >> startBit) & 1) != 0)
        break;
    return UINT64(1) << startBit;
    /*
    int endBit = 63;
    while(true)
    {
      if ((mask >> startBit) & 1) != 0)
        break;
    }
    endBit++;
    for (int i = startBit; i < endBit; i++)
      if ((mask >> startBit) & 1) == 0)
        return false;
    endPos = base & mask;
    return (Mask & (1 << 11)) != 0; 
    return  true;
    */
  }
  UINT64 GetLimit() const { return GetStart() + GetSize(); }
  bool DoesIntersect(UINT64 start, UINT64 size) const
  { 
    UINT64 start2 = GetStart();
    UINT64 limit2 = GetLimit();
    UINT64 limit = start + size;
    if (start <= start2)
      return limit > start2;
    return start < limit2;
  }
  bool DoesContain(UINT64 start, UINT64 size) const
    { return start >= GetStart() && start + size <= GetLimit(); }
};

struct CInfo
{
  UINT64 Cap;
  UINT64 Type;
  BYTE Fix64K[kNumFix64KItems];
  BYTE Fix16K[kNumFix16KItems];
  BYTE Fix4K[kNumFix4KItems];

  CVarReg VarRegs[kNumVarRegsMax];

  bool ReadInfo(IReadMSR * readMSR);
  
  BYTE GetFixedRegType(int index) const
    { return Fix64K[index]; }
  UINT32 GetItemSize(int index) const
  { 
    if (index < kNumFix64KItems)
      return (64 << 10);
    index -= kNumFix64KItems;
    if (index < kNumFix16KItems)
      return (16 << 10);
    index -= kNumFix16KItems;
    return (4 << 10);
  }
  BYTE GetNumberOfVarRegs() const
    { return BYTE(Cap & 0xFF); }
  bool AreFixedSupported() const
    { return (Cap & (1 << 8)) != 0; }
  bool IsWriteCombiningSupported() const
    { return (Cap & (1 << 10)) != 0; }
  bool IsFixedEnabled() const
    { return (Type & (1 << 10)) != 0; }
  bool IsEnabled() const
    { return (Type & (1 << 11)) != 0; }
  BYTE GetDefaultType() const
    { return BYTE(Type & 0xFF); }

  /*
  bool GetVarBlockType(UINT64 address, BYTE &type) const
  {
    for (int i = (int)GetNumberOfVarRegs() - 1; i >= 0; i--)
    {
      const CVarReg &varReg = VarRegs[i];
      if (varReg.IsValid())
        if ((address & varReg.GetMask()) == varReg.GetBase())
        {
          type = varReg.GetType();
          return true;
        }
    }
    return false;
  }
  */
  bool GetVarBlockType(UINT64 start, UINT64 size, BYTE &type) const
  {
    bool intersect = false;
    bool contain = false;
    for (int i = (int)GetNumberOfVarRegs() - 1; i >= 0; i--)
    {
      const CVarReg &varReg = VarRegs[i];
      if (varReg.IsValid())
        if (varReg.DoesIntersect(start, size))
        {
          if (intersect && type != varReg.GetType())
            return false;
          intersect = true;
          if (!contain)
            contain = varReg.DoesContain(start, size);
          type = varReg.GetType();
        }
    }
    return contain;
  }

  UINT64 GetWriteBackLimit() const 
  {
    UINT64 limit = 0;
    for (int i = 0; i < GetNumberOfVarRegs(); i++)
    {
      const CVarReg &varReg = VarRegs[i];
      if (varReg.IsValid())
        if (varReg.GetType() == kWriteBack)
        {
          UINT64 localLimit = varReg.GetLimit();
          if (localLimit > limit)
            limit = localLimit;
        }
    }
    return limit;
  }

};

/*
struct CInfoItem
{
  UINT64 pos;
  UINT64 size;
  EType type;
};

struct CInfoItemItems
{
  CInfoItem[1 + kNumFix16KRegs + kNumFix4KRegs];
  void ConvertFrom(

  UINT64 pos;
  UINT64 size;
  EType type;
};
*/

};

#endif