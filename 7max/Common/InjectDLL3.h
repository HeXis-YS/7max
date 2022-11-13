// InjectDLL3.h

#ifndef __INJECTDLL3_H
#define __INJECTDLL3_H

#include "Common/String.h"

bool StartProcessVia7max(const UString &command, bool wait = false);
bool InjectToProcess(DWORD processID);

#endif
