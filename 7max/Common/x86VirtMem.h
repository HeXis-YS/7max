// x86VirtMem.h

#pragma once

#ifndef __X86VIRTMEM_H
#define __X86VIRTMEM_H

namespace NPE
{
  const int kPresent = 0;
  const int kReadWrite  = 1;
  const int kUserSupervisor = 2;
  const int kWriteThrough = 3;
  const int kCacheDisabled = 4;
  const int kAccessed = 5;
  const int kDirty = 6;
  const int kPageSize = 7;
  const int kGlobal = 8;
  const int kPATForLarge = 12;
  const int kPSE36 = 13;

  struct CPE
  {
    UINT32 Value;
    bool GetFlag(int bitPos) const { return (Value & (1 << bitPos)) != 0; }

    bool GetPresent() const { return GetFlag(kPresent); }
    bool GetCanWrite() const { return GetFlag(kReadWrite); }
    bool GetIsLargePage() const { return GetFlag(kPageSize); }
    bool GetWriteThrough() const { return GetFlag(kWriteThrough); }
    bool GetAvailableForUser() const { return GetFlag(kUserSupervisor); }
    bool GetCacheDisabled() const { return GetFlag(kCacheDisabled); }
    bool GetAccessed() const { return GetFlag(kAccessed); }
    bool GetDirty() const { return GetFlag(kDirty); }
    bool GetGlobal() const { return GetFlag(kGlobal); }
    
    int GetSysField() const { return (Value >> 9) & 0x7; }

    void SetFlag(int bitPos, bool flag)
    { 
      Value &= ~(1 << bitPos);
      Value |= ((UINT32)(flag ? 1 : 0)) << bitPos;
    }
    void SetPresent(bool flag) { SetFlag(kPresent, flag); }
    void SetCanWrite(bool flag) { SetFlag(kReadWrite, flag); }
    void SetIsLargePage(bool flag) { SetFlag(kPageSize, flag); }
    void SetWriteThrough(bool flag) { SetFlag(kWriteThrough, flag); }
    void SetAvailableForUser(bool flag) { SetFlag(kUserSupervisor, flag); }
    void SetCacheDisabled(bool flag) { SetFlag(kCacheDisabled, flag); }
    void SetAccessed(bool flag) { SetFlag(kAccessed, flag); }
    void SetDirty(bool flag) { SetFlag(kDirty, flag); }
    void SetGlobal(bool flag) { SetFlag(kGlobal, flag); }
    
    UINT32 GetBaseAddress() const { return (Value & 0xFFFFF000); }
    UINT32 GetBaseAddress4MB() const { return (Value & 0xFFC00000); }
    
    void SetBaseAddress(UINT32 address)
    { 
      Value   &= 0x00000FFF;
      address &= 0xFFFFF000;
      Value |= address;
    }
    
    UINT32 GetPfn4MB() const { return (Value >> 22); }
    void SetPfn4M(UINT32 pfn)
    { 
      Value &= 0x003E1FFF; // 0x003FFFFF; // for normar
      Value |= (pfn << 22);
      Value |= ((pfn >> 10) & 0xF) << kPSE36;
    }

    /*
    UINT32 GetBaseAddress4MB() const { return (Value & 0xFFC00000); }
    void SetBaseAddress4M(UINT32 address)
    { 
      Value   &= 0x003FFFFF;
      address &= 0xFFC00000;
      Value |= address;
    }
    */
    bool GetPATForLarge() const { return GetFlag(kPATForLarge); }
  };
};

// const int kTableSize = (1 << 10);

#endif
