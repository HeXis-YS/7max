// VirtAlloc.h

#ifndef __VIRTALLOC_H
#define __VIRTALLOC_H

#include "x86VirtMem.h"
#include "7maxDevice.h"
#include "7maxIpcCaller.h"

const UINT32 kPDBase = (0xC0300000);
const UINT32 kLargePDBase = kPDBase + (1 << 11);
const UINT32 kLargeVirtualStartPos = 0x80000000;
const UINT32 kNumVirtualLargePages = 0x100;

const int kNumBitsInVirtPage = 16;
const UINT32 kNumVirtPages = (1 << (30 - kNumBitsInVirtPage));

const int kNumLargeToVirtDifferenceBits = 
    (kNumBitsInLargePage - kNumBitsInVirtPage);
const UINT32 kNumVirtPagesInLargePage = (1 << kNumLargeToVirtDifferenceBits);

struct CVirtPageInfo
{
  bool Allocated;
  bool Continued;
};

// extern bool g_UseAllocateServer;
const bool g_UseAllocateServer = false;
extern C7maxIpc g_7maxIpc;

struct CVirtualAllocator
{
  NWindows::NSynchronization::CCriticalSection _virtualCriticalSection; 
  C7maxDevice _device;
  bool _created;
  NPE::CPE _storedPEs[kNumVirtualLargePages];
  NPE::CPE _currentPEs[kNumVirtualLargePages];
  CVirtPageInfo _pages[kNumVirtPages];
  bool Create()
  {
    _created = _device.Create();
    return _created;
  };
  void StorePEs()
  { 
    if (!_created)
      return;
    _device.CopyMem(_storedPEs, (const void *)kLargePDBase, 
        kNumVirtualLargePages * sizeof(_storedPEs[0])); 
  }
  void RestorePEs()
  { 
    if (!_created)
      return;
    _device.CopyMem((void *)kLargePDBase, _storedPEs, 
        kNumVirtualLargePages * sizeof(_storedPEs[0]));
  }

  bool Allocate(UINT32 numPages, UINT32 *pfnArray)
  {
    if (g_UseAllocateServer)
      return g_7maxIpc.Allocate(numPages, pfnArray);
    else
      return g_PhysicalAllocator->Allocate(numPages, pfnArray);
  }

  bool Free(UINT32 numPages, const UINT32 *pfnArray)
  {
    if (g_UseAllocateServer)
      return g_7maxIpc.Free(numPages, pfnArray);
    else
      return g_PhysicalAllocator->Free(numPages, pfnArray);
  }
  
  bool AllocateInternal(UINT32 startPage, UINT32 numPages)
  {
    UINT32 startLargePage = startPage >> kNumLargeToVirtDifferenceBits;
    UINT32 numLargePages = 
        ((startPage + numPages + 
        (1 << kNumLargeToVirtDifferenceBits) - 1) >> 
        kNumLargeToVirtDifferenceBits) - startLargePage;
    UINT32 i;
    UINT32 numNewLargePages = 0;
    for (i = startLargePage; i < startLargePage + numLargePages; i++)
      if (!_currentPEs[i].GetPresent())
        numNewLargePages++;
    if (numNewLargePages > 0)
    {
      UINT32 newLargePages[kNumVirtualLargePages];
      if (!Allocate(numNewLargePages, newLargePages))
      {
        ::SetLastError(ERROR_COMMITMENT_LIMIT);
        return false; 
      }
      UINT32 curNewLargePageIndex = 0;
      for (i = startLargePage; i < startLargePage + numLargePages; i++)
      {
        NPE::CPE &pe = _currentPEs[i];
        if (!pe.GetPresent())
        {
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
          pe.SetPfn4M(newLargePages[curNewLargePageIndex++]);
        }
      }
      _device.CopyMem((void *)((UINT32)kLargePDBase + 
          startLargePage * sizeof(_currentPEs[0])), 
          _currentPEs + startLargePage, 
          numLargePages * sizeof(_currentPEs[0]));
    }
    for (i = startPage; i < startPage + numPages; i++)
    {
      CVirtPageInfo &page = _pages[i];
      page.Allocated = true;
      page.Continued = (i + 1) < startPage + numPages;
    }
    return true;
  }
public:
  CVirtualAllocator(): _created(false)
  {
    // MessageBox(0, "", "CVirtualAllocator", 0);
    Create();
    if (!_created)
      return;
    StorePEs();
    UINT32 i;
    for (i = 0; i < kNumVirtualLargePages; i++)
    {
      if (_storedPEs[i].GetPresent())
      {
        _created = false;
        return;
      }
      _currentPEs[i].Value = 0;
      // _currentPEs[i].SetPresent(false);
    }
    for (i = 0; i < kNumVirtPages; i++)
      _pages[i].Allocated = false;
  }
  ~CVirtualAllocator()
  {
    Destroy();
  }
  void Destroy()
  {
    if (!_created)
      return;
    UINT32 numNewLargePages = 0;
    UINT32 newLargePages[kNumVirtualLargePages];
    for (UINT32 i = 0; i < kNumVirtualLargePages; i++)
    {
      NPE::CPE &pe = _currentPEs[i];
      if (pe.GetPresent())
        newLargePages[numNewLargePages++] = pe.GetPfn4MB();
    }
    if (numNewLargePages > 0)
      Free(numNewLargePages, newLargePages);
    
    RestorePEs();
    _device.Close();
    // MessageBox(0, "", "~CVirtualAllocator", 0);
    _created = false;
  }
  bool CanAllocate() const { return _created; }
  
  LPVOID AllocatePages(UINT32 numPages)
  {
    if (!CanAllocate())
      return 0;
    if (numPages == 0)
    {
      ::SetLastError(ERROR_INVALID_PARAMETER);
      return (LPVOID)0;
    }
    NWindows::NSynchronization::CCriticalSectionLock lock(_virtualCriticalSection); 
    UINT32 startFree = 0;
    for (UINT32 i = 0; i < kNumVirtPages; i++)
    {
      if (_pages[i].Allocated)
      {
        startFree = i + 1;
        continue;
      }
      if (i - startFree + 1 == numPages)
      {
        if (!AllocateInternal(startFree, numPages))
          return 0;
        UINT32 *data = (UINT32 *)(kLargeVirtualStartPos + 
            (startFree << kNumBitsInVirtPage));
        /*
        UINT32 numWords = numPages << (kNumBitsInVirtPage - 2);
        for (UINT32 j = 0; j < numWords; j++)
          data[j] = 0;
        */
        ::ZeroMemory(data, (numPages << kNumBitsInVirtPage));
        return data;
      }
    }
    ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return 0;
  }
  
  LPVOID AllocateBytes(UINT32 &numBytes)
  {
    if (numBytes > ((UINT32)1 << 30))
    {
      numBytes = 0;
      ::SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return 0;
    }
    UINT32 numPages = (numBytes + (1 << kNumBitsInVirtPage) - 1) >> 
        kNumBitsInVirtPage;
    LPVOID address = AllocatePages(numPages);
    if (address == 0)
      numBytes = 0;
    else
      numBytes = numPages << kNumBitsInVirtPage;
    return address;
  }

  bool FreePages(UINT32 pageIndex)
  {
    if (!CanAllocate())
      return 0;
    NWindows::NSynchronization::CCriticalSectionLock lock(_virtualCriticalSection); 
    if (pageIndex > 0)
    {
      CVirtPageInfo &prevPage = _pages[pageIndex  - 1];
      if (prevPage.Allocated && prevPage.Continued)
      {
        ::SetLastError(ERROR_INVALID_ADDRESS);
        return false;
      }
    }
    UINT32 currentPage = pageIndex;
    while(true)
    {
      CVirtPageInfo &page = _pages[currentPage];
      if (!page.Allocated)
      {
        ::SetLastError(ERROR_INVALID_ADDRESS);
        return false;
      }
      page.Allocated = false;
      currentPage++;
      if (!page.Continued)
        break;
    }
    UINT32 numNewLargePages = 0;
    UINT32 newLargePages[kNumVirtualLargePages];
    UINT32 startLargePage = pageIndex >> kNumLargeToVirtDifferenceBits;
    UINT32 numLargePages = ((currentPage + 
        (1 << kNumLargeToVirtDifferenceBits) - 1) >> 
        kNumLargeToVirtDifferenceBits) - startLargePage;
    for (UINT32 i = startLargePage; i < startLargePage + numLargePages; i++)
    {
      NPE::CPE &pe = _currentPEs[i];
      if (!pe.GetPresent())
        continue;
      UINT32 j;
      for (j = 0; j < kNumVirtPagesInLargePage; j++)
        if (_pages[(i << kNumLargeToVirtDifferenceBits) + j].Allocated)
          break;
      if (j == kNumVirtPagesInLargePage)
      {
        newLargePages[numNewLargePages++] = pe.GetPfn4MB();
        pe.Value = 0;
        // pe.SetPresent(false);
      }
    }
    if (numNewLargePages > 0)
    {
      Free(numNewLargePages, newLargePages);
      _device.CopyMem((void *)((UINT32)kLargePDBase + 
          startLargePage * sizeof(_currentPEs[0])), 
          _currentPEs + startLargePage, 
          numLargePages * sizeof(_currentPEs[0]));
    }
    return true;
  }

  bool Free(LPVOID address)
  {
    return FreePages(((UINT32)address - kLargeVirtualStartPos) >> kNumBitsInVirtPage);
  }
};

#endif
