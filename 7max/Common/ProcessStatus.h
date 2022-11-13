// ProcessStatus.h

#ifndef __PROCESSSTATUS_H
#define __PROCESSSTATUS_H

#include "Common/Types.h"

void InitProcessStatus();
// bool IsSystem64();
// bool Is64BitProcess();
extern bool g_IsSystem64;
extern bool g_IsProcess64;

const int kPathNames = MAX_PATH + 100;

// extern FARPROC g_LoadLibAddress32;
// extern FARPROC g_LoadLibAddress64;
extern FARPROC g_LoadLibAddress;

// extern TCHAR g_DllPath32[kPathNames];
// extern TCHAR g_DllPath64[kPathNames];
extern TCHAR g_DllPath[kPathNames];

extern TCHAR g_GuiExePath32[kPathNames];
extern TCHAR g_GuiExePath64[kPathNames];
extern TCHAR g_ConExePath32[kPathNames];
extern TCHAR g_ConExePath64[kPathNames];

bool Is64BitProcess(HANDLE process);
UInt64 GetPhysicalRamSize();

#endif
