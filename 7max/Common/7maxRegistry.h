// 7maxRegistry.h

#ifndef __7MAXREGISTRY_H
#define __7MAXREGISTRY_H

#include "Common/String.h"
#include "Common/Types.h"

void SaveUseMTRR(bool showDots);
bool ReadUseMTRR();

UInt32 GetRecommendedAllocMin();
void SaveAllocMin(UInt32 size);
UInt32 ReadAllocMin();

void SavePhysLimit(UInt32 size);
UInt32 ReadPhysLimit();

void SaveMetod(UInt32 method);
UInt32 ReadMethod();

#endif
