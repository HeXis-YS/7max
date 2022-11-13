// 7maxDevice.cpp

#include "StdAfx.h"
#include "7maxDevice.h"

static LPCTSTR kDirectNTDrvName = TEXT("\\\\.\\7max");

bool C7maxDevice::Create()
{
  return CDevice::Create(kDirectNTDrvName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0);
}





