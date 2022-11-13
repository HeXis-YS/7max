// PhysAlloc.cpp

#include "StdAfx.h"

#include "Windows/FileMapping.h"
#include "PhysAlloc.h"
#include "7maxDevice.h"
#include "x86VirtMem.h"
#include "7maxRegistry.h"
#include "x86CPUID.h"

using namespace NWindows;

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!
// this function must not be inlined, since 
// it can be optimized, and will always give true

static void WriteMem(UINT32 *p, UINT32 val) { *p = val; }
static bool CheckMem(UINT32 *p, UINT32 val) 
{ 
  WriteMem(p, val);
  return *p == val;
}

static const UINT32 kNumVirtualLargePages = 0x100;
static const UINT32 kPDBase = (0xC0300000);
static const UINT32 kLargePDBase = kPDBase + (1 << 11);
static const UINT32 kCheckPageIndex = kNumVirtualLargePages - 1;
static const UINT32 kCheckPDAddress = kLargePDBase + (kCheckPageIndex << 2);

static const UINT32 kLargeVirtualStartPos = 0x80000000;

static LPCTSTR kFileMappingName = TEXT("7maxFileMapping");
static LPCTSTR kMutexName = TEXT("7maxMutex");

/*
UINT32 VirtToPhys(UINT32 virt)
{
  C7maxDevice device;
  if (!device.Create())
    return 0;
  NPE::CPE pe;
  UINT32 pdIndex = (virt >> 22);
  device.CopyMem(&pe, (const void *)kPDBase + 
      (pdIndex) * sizeof(pe), sizeof(pe)); 
  if (!pe.GetPresent())
    return 0;
  if (pe.GetIsLargePage())
    return pe.GetBaseAddress4MB() + (virt & 0x3FFFFF);
  return 0;
}
*/

static void GetPhysicalSize(C7maxDevice &device, UINT64 startPos, UINT64 limitPos, UINT64 &result)
{
  result = 0;
  NPE::CPE storedPE;

  device.CopyMem(&storedPE, (const void *)kCheckPDAddress, sizeof(storedPE)); 

  UINT64 i = (startPos >> kNumBitsInLargePage << kNumBitsInLargePage);
  for (; i +  (1 << kNumBitsInLargePage) - sizeof(UINT32) < limitPos; 
      i += (1 << kNumBitsInLargePage))
  {
    UINT32 pageIndex = UINT32(i >> kNumBitsInLargePage);
    NPE::CPE pe;
    pe.Value = 0;
    pe.SetIsLargePage(true);
    pe.SetPresent(true);
    pe.SetCanWrite(true);
    pe.SetWriteThrough(false);
    pe.SetAvailableForUser(true);
    pe.SetCacheDisabled(false);
    pe.SetAccessed(true);
    pe.SetDirty(true);
    
    // Global must be off for tlb flushing on CR3 changing.
    pe.SetGlobal(false); 
    // pe.SetGlobal(true);
    
    pe.SetBaseAddress(0);
    pe.SetPfn4M(pageIndex);
    device.CopyMem((void *)((UINT32)kCheckPDAddress), &pe, sizeof(pe));

    UINT32 *pointer = 
      (UINT32 *)(kLargeVirtualStartPos + ((kCheckPageIndex + 1) << kNumBitsInLargePage) - sizeof(UINT32));
    __try 
    {
      UINT32 v = *pointer;
      if (!CheckMem(pointer, 0))
        break;
      if (!CheckMem(pointer, 0x1234FEDC))
        break;
      *pointer = v;
    } 
    __except (EXCEPTION_EXECUTE_HANDLER) 
    { 
        break;
    }
  }
  device.CopyMem((void *)kCheckPDAddress, &storedPE, sizeof(storedPE));
  result = i;
}

static bool CheckPage(C7maxDevice &device, UINT32 pageIndex)
{
  UINT64 startPos = UINT64(pageIndex) << kNumBitsInLargePage;
  UINT64 limitPos = startPos + (UINT64(1) << kNumBitsInLargePage);
  UINT64 result;
  GetPhysicalSize(device, startPos, limitPos, result);
  return (result == limitPos);
}

bool GetPhysicalSize(UINT64 startPos, UINT64 limitPos, UINT64 &result)
{
  result = 0;
  C7maxDevice device;
  if (!device.Create())
    return false;
  GetPhysicalSize(device, startPos, limitPos, result);
  return true;
}

/*
bool GetPhysicalSizeViaMTRR(UINT64 &result)
{
  C7maxDevice device;
  if (!device.Create())
    return false;
  NMTRR::CInfo mtrr;
  if (!mtrr.ReadInfo(&device))
    return false;
  result = mtrr.GetWriteBackLimit();
  return true;
}
*/

/*
bool GetTotalPhysicalSize(UINT64 &size)
{
  CPhysicalAllocator allocator;
  size = 0;
  if (!allocator.Init())
    return false;
  size = allocator.GetPhysicalSize();
  return true;
}
*/

bool CPhysicalAllocator::FillPagesStatus()
{
  MEMORYSTATUSEX g_statex;
  g_statex.dwLength = sizeof(g_statex);
  if (!::GlobalMemoryStatusEx(&g_statex))
    return false;
  /*
  if (g_statex.ullTotalPhys > (UINT32(0x3) << 30))
    return false;
  */
  
  StartPage = UINT32((g_statex.ullTotalPhys + 
    (1 << kNumBitsInLargePage) - 1) >> kNumBitsInLargePage);
  
  // StartPage = ((64 * 57) << 20) >> kNumBitsInLargePage;    
  // StartPage = (0xE0000000) >> kNumBitsInLargePage;
  // StartPage =  (0xE7000000) >> kNumBitsInLargePage;    

  UINT32 i;
  for (i = 0; i < StartPage; i++)
    g_Pages[i].SetAsSystem();
  for (; i < kNumPhysicalLargePagesMax; i++)
    g_Pages[i].SetAsUnknown();
  C7maxDevice device;
  if (!device.Create())
    return false;
  bool useMTRR = ReadUseMTRR();

  if (!NCPUID::IsMTRRSupported())
    useMTRR = false;

  NMTRR::CInfo mtrr;
  if (useMTRR)
  {
    if (!mtrr.ReadInfo(&device))
      useMTRR = false;
    else
      if (mtrr.GetNumberOfVarRegs() == 0)
        useMTRR = false; // It's for VM Ware
  }  

  UINT32 currentPageLimit = NumTotalPages;
  if (useMTRR)
  {
    UINT64 mtrrWBLimitPage = mtrr.GetWriteBackLimit() >> kNumBitsInLargePage;
    if (mtrrWBLimitPage < currentPageLimit)
      currentPageLimit = (UINT32)mtrrWBLimitPage;
  }
  else
  {
    currentPageLimit = 0x400; // It's 4 GB limit;  Change It !!!!!!
  }

  {
    UINT32 physLimit = ReadPhysLimit();
    if (physLimit != 0)
    {
      UINT32 physLimitInPages = (physLimit + 3) >> 2;
      if (currentPageLimit > physLimitInPages)
        currentPageLimit = physLimitInPages;
    }
  }


  LimitPage = 0;
  // disable upper 64 MB in 4 GB area; Seems it's used by system. I don't know why.
  UINT32 disableEnd = (UINT32)1 << (32 - kNumBitsInLargePage);
  UINT32 disableSize = (UINT32)1 << (26 - kNumBitsInLargePage);
  UINT32 disableStart = disableEnd - disableSize;
  bool startPageIsDefined = false;
  for (i = StartPage; i < currentPageLimit; i++)
  {
    UINT64 address = ((UINT64)i) << kNumBitsInLargePage;
    CPhysicalPage &page = g_Pages[i];
    if (useMTRR)
    {
      BYTE type;
      if (mtrr.GetVarBlockType(address, (1 << kNumBitsInLargePage), type))
        if (type == NMTRR::kWriteBack)
          if (i < disableStart || i >= disableEnd)
          {
            if (!startPageIsDefined)
              StartPage = i;
            startPageIsDefined = true;
            LimitPage = i + 1;
            page.SetAsFree();
          }
          else
            page.SetAsSystem();
    }
    else
    {
      page.SetAsSystem();
      LimitPage = i + 1;
    }
  }
  if (useMTRR)
    for (; LimitPage > StartPage; LimitPage--)
    {
      UINT32 pageIndex = LimitPage - 1;
      CPhysicalPage &page = g_Pages[pageIndex];
      if (page.IsFree())
      {
        if (CheckPage(device, pageIndex))
          break;
        page.SetAsUnknown();
      }
    }
  else
    for (i = StartPage; i < LimitPage; i++)
    {
      UINT32 pageIndex = i;
      CPhysicalPage &page = g_Pages[pageIndex];
      if (page.IsSystem())
      {
        if (!CheckPage(device, pageIndex))
        {
          LimitPage = pageIndex;
          for (; i < currentPageLimit; i++)
            g_Pages[i].SetAsUnknown();
          break;
        }
        page.SetAsFree();
      }
    }
  return true;
}

bool CPhysicalAllocator::Init()
{
  // MessageBeep(-1);
  // Sleep(10000);
  NWindows::NSynchronization::CMutexLock mutexLock(g_PhysicalAllocatorMutex);
  StructureSize = sizeof(*this);
  Version = 0;
  _canAllocate = 0;
  Flags = 0;
  NumTotalPages = kNumPhysicalLargePagesMax;
  NumProcessesIDs = kNumProcessIDs;
  UINT32 i;
  for (i = 0; i < kNumProcessIDs; i++)
    ProcessIDs[i] = 0;
  for (i = 0; i < sizeof(Reserved1) / sizeof(Reserved1[0]); i++)
    Reserved1[i] = 0;
  for (i = 0; i < sizeof(Reserved2) / sizeof(Reserved2[0]); i++)
    Reserved2[i] = 0;
  if (!FillPagesStatus())
    return false;
  _canAllocate = 1;
  return true;
}

NWindows::NSynchronization::CMutex g_PhysicalAllocatorMutex; 
UINT32 g_PhysicalAllocatorProcessID;
CPhysicalAllocator *g_PhysicalAllocator = 0;
static CFileMapping g_fileMapping;

bool CPhysicalAllocator::Create()
{
  g_PhysicalAllocatorProcessID = ::GetCurrentProcessId();
  UINT32 totalSize = sizeof(CPhysicalAllocator);
  if (!g_fileMapping.Create(INVALID_HANDLE_VALUE, NULL,
      PAGE_READWRITE, totalSize, kFileMappingName))
    return false;
  g_PhysicalAllocator = (CPhysicalAllocator *)
      g_fileMapping.MapViewOfFile(FILE_MAP_WRITE, 0, totalSize);
  if (g_PhysicalAllocator != NULL)
  {
    if (g_PhysicalAllocatorMutex.Create(true, kMutexName))
    {
      if (::GetLastError() == ERROR_ALREADY_EXISTS)
      {
        g_PhysicalAllocatorMutex.Release();
        return true;
      }
      if (g_PhysicalAllocator->Init())
      {
        g_PhysicalAllocatorMutex.Release();
        return true;
      }
      g_PhysicalAllocatorMutex.Release();
      g_PhysicalAllocatorMutex.Close();
    }
    UnmapViewOfFile(g_PhysicalAllocator);
  }
  g_fileMapping.Close();
  return false;
}
    
void CPhysicalAllocator::Destroy()
{
  g_PhysicalAllocatorMutex.Close();
  UnmapViewOfFile(g_PhysicalAllocator);
  g_fileMapping.Close();
}


