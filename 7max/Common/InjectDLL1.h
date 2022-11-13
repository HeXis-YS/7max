// InjectDLL1.h

#ifndef __INJECTDLL1_H
#define __INJECTDLL1_H

#pragma pack( push, 1)

struct CCallEax
{
  BYTE CallEax[2];
  CCallEax() { CallEax[0] = 0xFF; CallEax[1] = 0xD0; }
};

struct CImToEax
{
  BYTE ImToEax;
  CImToEax() { ImToEax = 0xB8; }
};

struct CCall32
{
  CImToEax ImToEax;
  UINT32 CallOffset;
  CCallEax CallEax;
};

struct CPrefix64
{
  BYTE Prefix;
  CPrefix64() { Prefix = 0x48; }
};

struct CCall64
{
  CPrefix64 Prefix1;
  CImToEax ImToEax;
  UINT64 CallOffset;
  CPrefix64 Prefix2;
  CCallEax CallEax;
};

struct CPushIm
{
  BYTE Push;
  CPushIm() { Push =  0x68; }
};

struct CCall32Param
{
  CPushIm PushIm;
  UINT32 Param;
  CCall32 CallCommand;
};

struct CMoveImToEax
{
  BYTE Move;
  CMoveImToEax() { Move = 0xB9; }
};

struct CCall64Param64
{
  CPrefix64 Prefix;
  CMoveImToEax MoveEax;
  UINT64 Param;
  CCall64 CallCommand;
};

struct CCall64Param32
{
  CMoveImToEax Move;
  UINT32 Param;
  CCall64 CallCommand;
};

struct CInjectBlock32
{
  CCall32Param LoadLibraryWCall;
  CCall32Param SetEventCall;
  CCall32Param SleepCall;
  void Init()
  {
    HMODULE kernel32Handle = GetModuleHandle(TEXT("kernel32.dll"));
    LoadLibraryWCall.CallCommand.CallOffset = (UINT32)GetProcAddress(kernel32Handle, "LoadLibraryW");
    SetEventCall.CallCommand.CallOffset = (UINT32)GetProcAddress(kernel32Handle, "SetEvent");
    SleepCall.CallCommand.CallOffset = (UINT32)GetProcAddress(kernel32Handle, "Sleep");
    SleepCall.Param = INFINITE;
  }
};

struct CInjectBlock64
{
  CCall64Param64 LoadLibraryWCall;
  CCall64Param64 SetEventCall;
  CCall64Param32 SleepCall;
  void Init()
  {
    HMODULE kernel32Handle = GetModuleHandle(TEXT("kernel32.dll"));
    LoadLibraryWCall.CallCommand.CallOffset = (UINT64)GetProcAddress(kernel32Handle, "LoadLibraryW");
    SetEventCall.CallCommand.CallOffset = (UINT64)GetProcAddress(kernel32Handle, "SetEvent");
    SleepCall.CallCommand.CallOffset = (UINT64)GetProcAddress(kernel32Handle, "Sleep");
    SleepCall.Param = INFINITE;
  }
};
#pragma pack(pop)

// bool InjectDllRemote(HANDLE hProcess, const wchar_t *dllPath);

#endif
