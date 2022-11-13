// 7maxDevice.h

#ifndef __7MAXDEVICE_H
#define __7MAXDEVICE_H

#include "Windows/Device.h"
#include "MTRR.h"
#include "DirectNT.h"

class C7maxDevice: public NWindows::NDevice::CDevice, public NMTRR::IReadMSR
{
public:
  bool Create();
  bool IoControl(const CKModeInfo &directNTInfo, LPVOID outBuffer, DWORD outBufferSize, 
        LPDWORD bytesReturned)
  {
    return CDevice::IoControl(IOCTL_DIRECTNT_CONTROL, (void *)&directNTInfo, 
        sizeof(directNTInfo), outBuffer, outBufferSize, bytesReturned, 0);
  }
  bool CopyMem(void *dest, const void *src, UINT32 size)
  {
    CKModeInfo info = { OP_CopyData, (ULONG)dest, (ULONG)src, size};
    ULONG resultLen;
    return IoControl(info, NULL, 0, &resultLen);
  }
  bool CopyFromRemote(UINT32 processID, const void *src, void *dest, UINT32 size, UINT32 &processedSize)
  {
    CKModeInfo info = { OP_CopyFromRemote, processID, (ULONG)src, (ULONG)dest, size};
    ULONG resultLen;
    return IoControl(info, &processedSize, sizeof(processedSize), &resultLen);
  }
  bool CopyToRemote(UINT32 processID, void *dest, const void *src, UINT32 size, UINT32 &processedSize)
  {
    CKModeInfo info = { OP_CopyToRemote, processID, (ULONG)dest, (ULONG)src, size};
    ULONG resultLen;
    return IoControl(info, &processedSize, sizeof(processedSize), &resultLen);
  }
  bool ReadMSR(UINT32 address, UINT64 &result)
  {
    CKModeInfo info = { OP_GetMR, address, 0, 0 };
    ULONG resultLen;
    return IoControl(info, &result, sizeof(result), &resultLen);
  }
  bool ReadCR3(UINT32 &result)
  {
    CKModeInfo info = { OP_ReadCR3, 0, 0, 0 };
    ULONG resultLen;
    return IoControl(info, &result, sizeof(result), &resultLen);
  }
  bool WriteMSR(UINT32 address, UINT64 value)
  {
    CKModeInfo info = { OP_SetMR, address, ULONG(value), ULONG(value>>64) };
    ULONG resultLen;
    return IoControl(info, NULL, 0, &resultLen);
  }
  /*
  bool FlushTLB ()
  {
    CKModeInfo info = { OP_ClearCR3, (ULONG)0, (ULONG)0, 0};
    ULONG resultLen;
    return IoControl(info, NULL, 0, &resultLen);
  }
  */
  /*
  bool Int15(BYTE *data)
  {
    CKModeInfo info = { OP_Int15, (LONG_PTR)data, 0, 0 };
    ULONG resultLen;
    return IoControl(info, NULL, 0, &resultLen);
  }
  */
};

#endif