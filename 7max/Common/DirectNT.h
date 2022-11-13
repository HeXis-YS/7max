// DirectNT.h

#ifndef __DIRECTNT_H
#define __DIRECTNT_H

#define DIRECTNT_TYPE 40000

#define IOCTL_DIRECTNT_CONTROL \
  CTL_CODE(DIRECTNT_TYPE, 0x0800, METHOD_BUFFERED, FILE_READ_ACCESS)

#define OP_GetMR 20
#define OP_SetMR 21
// #define	OP_ReadPhysMemDword 40
#define OP_CopyData 0x7001
#define OP_ReadCR3 0x7003
#define OP_CopyFromRemote 0x7004
#define OP_CopyToRemote 0x7005

typedef struct
{
  ULONG OpCode;
  ULONG Par1;
  ULONG Par2;
  ULONG Par3;
  ULONG Par4;
} CKModeInfo, *PKModeInfo;

#endif

