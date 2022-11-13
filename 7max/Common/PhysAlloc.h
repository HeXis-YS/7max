// PhysAlloc.h

#ifndef __PHYSALLOC_H
#define __PHYSALLOC_H

#include "Windows/Synchronization.h"

const int kNumBitsInLargePage = 22;
const UINT32 kNumPhysicalLargePagesMax = (1 << 14);
const UINT32 kNumProcessIDs = (1 << 10);

extern NWindows::NSynchronization::CMutex g_PhysicalAllocatorMutex; 
extern UINT32 g_PhysicalAllocatorProcessID;

bool GetPhysicalSize(UINT64 startPos, UINT64 limitPos, UINT64 &result);
// bool GetTotalPhysicalSize(UINT64 &size);

struct CPhysicalPage
{
  enum 
  {
    kSevenMax = 1,
    kAllocated = 2,
    kZero = 4,
    kSystem = 2
  };
  bool CheckFlag(UINT32 flag) const { return (Status & flag) != 0; }
public:
  BYTE Status;
  BYTE Reserved[3];
  UINT32 ProcessID;
  
  void SetAsUnknown() 
  { 
    Status = 0 | 0; 
    Reserved[0] = Reserved[1] = Reserved[2] = 0;
  }
  void SetAsSystem() 
  { 
    Status = 0 | kSystem; 
    Reserved[0] = Reserved[1] = Reserved[2] = 0;
  }
  void SetAsFree() { Status = kSevenMax | 0; }
  void SetAsZero() { Status = kSevenMax | 0 | kZero; }
  void SetAsAllocated() { Status = kSevenMax | kAllocated; }

  bool IsSevenMax() const { return CheckFlag(kSevenMax); }
  bool IsFree() const { return IsSevenMax() && !CheckFlag(kAllocated); }
  bool IsZero() const { return IsFree() && CheckFlag(kZero); }
  bool IsAllocated() const { return IsSevenMax() && CheckFlag(kAllocated); }
  bool IsSystem() const { return !IsSevenMax() && CheckFlag(kSystem); }
};

class CPhysicalAllocator
{
  UINT32 StructureSize;
  UINT32 Version;
  UINT32 _canAllocate;
  UINT32 Flags;
  UINT32 NumTotalPages;
  UINT32 NumProcessesIDs;
  UINT32 StartPage;
  UINT32 LimitPage;
  UINT32 Reserved1[64 - 8];
  // UINT32 g_PhysicalStartFrame;
  CPhysicalPage g_Pages[kNumPhysicalLargePagesMax];
  DWORD ProcessIDs[kNumProcessIDs];
  UINT32 Reserved2[1024];
public:
  bool FillPagesStatus();
  bool Init();
  bool CanAllocate() const { return _canAllocate != 0; }


  UINT64 GetPhysicalSize() const
  {
    NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return 0;
    UINT64 size = 0;
    for (UINT32 i = 0; i < kNumPhysicalLargePagesMax; i++)
    {
      const CPhysicalPage &page = g_Pages[i];
      if (page.IsSevenMax() || page.IsSystem())
        size++;
    }
    return size << kNumBitsInLargePage;
  }

  bool Allocate(UINT32 numPages, UINT32 *pfnArray)
  {
    // MessageBeep(-1);
    // Sleep(300);
    NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return false;
    if (numPages == 0)
      return true;
    // NWindows::NSynchronization::CCriticalSectionLock lock(g_PhysicalCriticalSection); 
    UINT32 numAllocated = 0;
    UINT32 i;
    for (i = StartPage; i < LimitPage; i++)
    {
      CPhysicalPage &page = g_Pages[i];
      if (page.IsFree())
      {
        page.SetAsAllocated();
        page.ProcessID = g_PhysicalAllocatorProcessID;
        /*
        if (zeroArray != 0)
          zeroArray[numAllocated] = (page.IsZero() ? 1: 0);
        */
        pfnArray[numAllocated++] = i;
        if (numAllocated == numPages)
          return true;
      }
    }
    for (i = 0; i < numAllocated; i++)
    {
      UINT32 pageIndex = pfnArray[i];
      CPhysicalPage &page = g_Pages[pageIndex];
      page.SetAsFree();
      /*
      if (zeroArray != 0)
        if (zeroArray[pageIndex] != 0)
          page.SetAsZero();
      */
    }
    return false;
  }

  bool Free(UINT32 numPages, const UINT32 *pfnArray)
  {
    NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return false;
    // NWindows::NSynchronization::CCriticalSectionLock lock(g_PhysicalCriticalSection); 
    for (UINT32 i = 0; i < numPages; i++)
    {
      UINT32 pfn = pfnArray[i];
      if (pfn < LimitPage)
      {
        CPhysicalPage &page = g_Pages[pfn];
        if (page.IsAllocated())
        {
          page.SetAsFree();
          /*
          if (zeroArray != 0)
            if (zeroArray[i] != 0)
              page.SetAsZero();
          */
        }
      }
    }
    // MessageBeep(-1);
    // Sleep(300);
    return true;
  }
  
  bool FreeForProcess(UINT32 processID)
  {
    NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return false;
    UINT32 i;
    for (i = StartPage; i < LimitPage; i++)
    {
      CPhysicalPage &page = g_Pages[i];
      if (page.IsAllocated() && page.ProcessID == processID)
        page.SetAsFree();
    }
    for (i = 0; i < kNumProcessIDs; i++)
      if (ProcessIDs[i] == processID)
      {
        ProcessIDs[i] = 0;
        break;
      }
    return true;
  }

  void AddProcess(DWORD processID)
  {
    NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return;
    for (UINT32 i = 0; i < kNumProcessIDs; i++)
      if (ProcessIDs[i] == 0)
      {
        ProcessIDs[i] = processID;
        break;
      }
  }

  bool FindProcess(DWORD processID) const
  {
    NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return false;
    for (UINT32 i = 0; i < kNumProcessIDs; i++)
      if (ProcessIDs[i] == processID)
        return true;
    return false;
  }

  void GetSnapshot(CPhysicalAllocator &res) const
  {
    res = *this;
  }

  void GetSizes(UINT64 &totalSize, UINT64 &freeSize) const
  {
    totalSize = 0;
    freeSize = 0;
    // NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return;
    for (UINT32 i = StartPage; i < LimitPage; i++)
    {
      const CPhysicalPage &page = g_Pages[i];
      if (page.IsFree())
        freeSize += ((UINT64)1)<< kNumBitsInLargePage;
      if (page.IsSevenMax())
        totalSize += ((UINT64)1)<< kNumBitsInLargePage;
    }
  }

  UINT64 GetProcessMemorySize(UINT32 processID) const
  {
    // NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
    if (_canAllocate == 0)
      return 0;
    UINT64 totalSize = 0;
    for (UINT32 i = StartPage; i < LimitPage; i++)
    {
      const CPhysicalPage &page = g_Pages[i];
      if (page.IsAllocated() && page.ProcessID == processID)
        totalSize += ((UINT64)1)<< kNumBitsInLargePage;
    }
    return totalSize;
  }

  bool Is7maxProcess(DWORD processID) const
  {
    if (FindProcess(processID))
      return true;
    return (GetProcessMemorySize(processID) > 0);
  }


  static bool Create();
  static void Destroy();
};

extern CPhysicalAllocator *g_PhysicalAllocator;

class CPhysicalAllocatorWrap
{
  bool _needDestroy;
public:
  CPhysicalAllocatorWrap(): _needDestroy (false) {};
  bool Create()
  {
    _needDestroy = CPhysicalAllocator::Create();
    return _needDestroy;
  };
  ~CPhysicalAllocatorWrap() 
  {
    if (_needDestroy)
    {
      CPhysicalAllocator::Destroy();
      _needDestroy = false;
    }
  };
};

#endif
