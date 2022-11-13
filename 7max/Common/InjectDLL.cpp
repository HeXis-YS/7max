// InjectDLL.cpp

#include "StdAfx.h"

#include "Windows/Synchronization.h"

#include "InjectDLL.h"
#include "InjectDLL1.h"
#include "InjectDLL2.h"
#include "ProgramLocation.h"

#ifdef _SMAX_MANAGER
#include "../Common/PhysAlloc.h"
#endif

using namespace NWindows;

static LPCTSTR kDllName = TEXT("7max.dll");

static const wchar_t *kCantFindDLL = L"Can't find 7max.dll";

enum EInstructionType
{
  kPushIm,
  kPushEax,
  kImToEax,
  kCallEax,
  kEaxToMem,
  kBreakPoint
};

struct CInstruction
{
  EInstructionType Type;
  // CByteVector Post;
  CByteVector Buffer;
  UINT32 Param;
  CInstruction() {}
  UINT32 GetSize() const 
  {
    switch(Type)
    {
      case kPushIm:  return 1 + 4; 
      case kPushEax: return 1;
      case kImToEax: return 1 + 4;
      case kCallEax: return 2;
      case kEaxToMem:  return 1 + 4;
      case kBreakPoint:  return 1;
      default: throw 1;
    }
  }

  void AddString(const BYTE *s, int size)
  {
    for (int i = 0; i < size; i++)
      Buffer.Add(s[i]);
    while((Buffer.Size() & 0x3) != 0)
      Buffer.Add(0);
  }
  CInstruction(EInstructionType instType, UINT32 param = 0)
  {
    Type = instType;
    Param = param;
  }
  CInstruction(EInstructionType instType, const char *s)
  {
    Type = instType;
    AddString((const BYTE *)s, (lstrlenA(s) + 1) * sizeof(s[0]));
  }
  CInstruction(EInstructionType instType, const wchar_t *s)
  {
    Type = instType;
    AddString((const BYTE *)s, (lstrlenW(s) + 1) * sizeof(s[0]));
  }
};
  
class CMyCompiler
{
public:
  CObjectVector<CInstruction> Instructions;
  UINT32 GenerateCode(CByteVector &buffer, UINT32 base, UINT32 totalSize) const;
  UINT32 GenerateCode(CByteVector &buffer, UINT32 base) const
  {
    GenerateCode(buffer, base, 0);
    UINT32 totalSize = buffer.Size();
    buffer.Clear();
    return GenerateCode(buffer, base, totalSize);
  }
  void Add(EInstructionType instType, UINT32 param = 0)
    { Instructions.Add(CInstruction(instType, param)); }
  void Add(EInstructionType instType, const char *param)
    { Instructions.Add(CInstruction(instType, param)); }
  void Add(EInstructionType instType, const wchar_t *param)
    { Instructions.Add(CInstruction(instType, param)); }
  void AddCallCommand(HMODULE module, const char *funcName, UINT32 param)
  {
    Add(kPushIm, param);
    Add(kImToEax, (DWORD)GetProcAddress(module, funcName));
    Add(kCallEax, param);
  }
};

static void AddValueToBuffer(CByteVector &buffer, UINT32 v)
{
  for (int i = 0; i < sizeof(v); i++)
  {
    buffer.Add(BYTE(v));
    v >>= 8;
  }
}

UINT32 CMyCompiler::GenerateCode(CByteVector &buffer, 
    UINT32 base, UINT32 totalSize) const
{
  UINT32 resultOffest = (UINT32)-1;
  CRecordVector<BYTE> dataBuffer;
  UINT32 realCodeSize = 0;
  int i;
  for (i = 0; i < Instructions.Size(); i++)
    realCodeSize += Instructions[i].GetSize();
  // align
  UINT32 codeSize = (realCodeSize + 3) & (~3);

  DWORD dataBase = base + codeSize;
  for (i = 0; i < Instructions.Size(); i++)
  {
    const CInstruction &instr = Instructions[i];
    switch(instr.Type)
    {
      case kPushIm:
      {
        buffer.Add(0x68); // push
        if (instr.Buffer.Size() > 0)
        {
          AddValueToBuffer(buffer, dataBase + dataBuffer.Size());
          dataBuffer += instr.Buffer;
        }
        else
          AddValueToBuffer(buffer, instr.Param);
        // buffer += instr.Post;
        break;
      }
      case kPushEax:
      {
        buffer.Add(0x50);
        break;
      }
      case kImToEax:
      {
        buffer.Add(0xB8); // move EAX
        AddValueToBuffer(buffer, instr.Param);
        break;
      }
      case kCallEax:
      {
        buffer.Add(0xFF); // call EAX
        buffer.Add(0xD0);
        break;
      }
      case kEaxToMem:
      {
        buffer.Add(BYTE(0xA3));
        AddValueToBuffer(buffer, base + totalSize - sizeof(UINT32));
        break;
      }
      case kBreakPoint:
      {
        buffer.Add(BYTE(0xCC));
        break;
      }
      default:
        throw 1;
    }
  }
  if ((UINT32)buffer.Size() != realCodeSize)
    throw 1;
  while ((UINT32)buffer.Size() < codeSize)
    buffer.Add(0);
  buffer += dataBuffer;
  AddValueToBuffer(buffer, (UINT32)0);
  return realCodeSize;
}

////////////////////////
////////////////////////

void CDebugInjectorProcess::ReadTargetMemory(const void *base, void *buf, DWORD size)
{
  DWORD wr;
  if (!ReadProcessMemory(_process, base, buf, size, &wr))
    throw 124324;
  if (wr != size)
    throw 124324;
}

void CDebugInjectorProcess::WriteTargetMemory(void *base, const void *buf, DWORD size)
{
  DWORD wr;
  if (!WriteProcessMemory(_process, base, buf, size, &wr))
    throw 124324;
  if (wr != size)
    throw 124321;
}

bool CDebugInjectorProcess::SaveEntryPointContext(DEBUG_EVENT & dbgEvent)
{
  // Make sure that the thread we have the handle for is
  // the same thread that hit the BP
  // OutputDebugString(L"before _threadID != dbgEvent.dwThreadId\n");
  if (_threadID != dbgEvent.dwThreadId)
  {
    return false;
    // DebugBreak();    
  }
  _originalThreadContext.ContextFlags = CONTEXT_ALL;
  // OutputDebugString(L"before GetThreadContext\n");
  if (!GetThreadContext(_thread, &_originalThreadContext))
  {
    return false;
    // DebugBreak();    
  }
  // The EIP in the context structure points past the BP, so
  // decrement EIP to point at the original instruction
  _originalThreadContext.Eip = _originalThreadContext.Eip -1;
  return true;
}

DWORD CDebugInjectorProcess::ProcessEvent(DEBUG_EVENT &debugEvent)
{
  DWORD continueStatus = DBG_CONTINUE; // exception continuation 
  switch (debugEvent.dwDebugEventCode) 
  { 
    case CREATE_PROCESS_DEBUG_EVENT: 
      // OutputDebugString(L"CREATE_PROCESS_DEBUG_EVENT\n");
      _createProcessDebugInfo = debugEvent.u.CreateProcessInfo;
      _process = _createProcessDebugInfo.hProcess;
      _thread = _createProcessDebugInfo.hThread;
      _threadID = debugEvent.dwThreadId;
      if (_createProcessDebugInfo.lpStartAddress == 0)
        _dllWasInjected = true;
      break;
    case EXCEPTION_DEBUG_EVENT: 
    {
      // OutputDebugString(L"EXCEPTION_DEBUG_EVENT\n");
      EXCEPTION_RECORD &exceptionRecord = 
          debugEvent.u.Exception.ExceptionRecord;
      /*
      if (debugEvent.u.Exception.dwFirstChance == FALSE )
      {
        printf( "\nException code: %X  Addr: %08X\r\n",
          exceptionRecord.ExceptionCode, exceptionRecord.ExceptionAddress );
      }
      */
        
      if (_injected || exceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT)
      {
        // OutputDebugString(L"_injected1\n");
        continueStatus = DBG_EXCEPTION_NOT_HANDLED;
        // printf("\nDBG_EXCEPTION_NOT_HANDLED");
        break;
      }
      if (_createProcessDebugInfo.lpStartAddress == 0)
        break;
      if (!_firstBP)
      {
        _exeEntryPoint = _createProcessDebugInfo.lpStartAddress;
        // OutputDebugString(L"ReadTargetMemory\n");
        ReadTargetMemory(_exeEntryPoint, 
            &_originalExeEntryPointOpcode, 
            sizeof(_originalExeEntryPointOpcode));
        BYTE bpCode = 0xCC;
        WriteTargetMemory(_exeEntryPoint, &bpCode, sizeof(bpCode));
        FlushInstructionCache(_process, 0, 0);
        _firstBP = true;
      }
      else if (exceptionRecord.ExceptionAddress == _exeEntryPoint)
      {
        WriteTargetMemory(_exeEntryPoint, &_originalExeEntryPointOpcode, sizeof(_originalExeEntryPointOpcode));
        // OutputDebugString(L"before SaveEntryPointContext\n");
        if (!SaveEntryPointContext(debugEvent))
          break;
        // OutputDebugString(L"SaveEntryPointContext\n");

        HMODULE kernel32Handle = GetModuleHandle(TEXT("kernel32.dll"));
        
        CMyCompiler compiler;
        
        compiler.Add(kPushIm, _dllPath);
        compiler.Add(kImToEax, (DWORD)GetProcAddress(kernel32Handle, "LoadLibraryW"));
        compiler.Add(kCallEax);
        compiler.Add(kBreakPoint);
        
        UINT32 blockSize;
        {
          CByteVector buffer;
          compiler.GenerateCode(buffer, 0);
          blockSize = buffer.Size();
        }

        _stubInTarget =  ::VirtualAllocEx(_process, 0, blockSize, 
            MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        

        CByteVector buffer;
        UINT32 codeSize = compiler.GenerateCode(buffer, (DWORD)_stubInTarget);

        _stubInTargetBP = (PBYTE)_stubInTarget + codeSize - 1;

        void *v = &buffer.Front();
        
        WriteTargetMemory(_stubInTarget, &buffer.Front(), blockSize);
        
        CONTEXT stubContext = _originalThreadContext;
        stubContext.Eip = (DWORD)_stubInTarget;
        SetThreadContext(_thread, &stubContext);
        // m_pThread->Context = stubContext;
        FlushInstructionCache(_process, 0, 0);
      }
      else if (exceptionRecord.ExceptionAddress == _stubInTargetBP)
      {
        // m_pThread->Context = m_originalThreadContext;

        CONTEXT tempContext;
        tempContext.ContextFlags = CONTEXT_ALL;
        if (GetThreadContext(_thread, &tempContext))
          _dllWasInjected = (tempContext.Eax != 0);
        SetThreadContext(_thread, &_originalThreadContext);
        VirtualFreeEx(_process, _stubInTarget, 0, MEM_RELEASE);
        _injected = true;
        // OutputDebugString(L"VirtualFreeEx\n");
      }
      break;
    }
     
    case EXIT_PROCESS_DEBUG_EVENT: 
    {
      // OutputDebugString(L"EXIT_PROCESS_DEBUG_EVENT\n");
      break;
    }
      
    case LOAD_DLL_DEBUG_EVENT: 
    {
      // OutputDebugString(L"LOAD_DLL_DEBUG_EVENT\n");
      CloseHandle(debugEvent.u.LoadDll.hFile);
      break;
    }
  } 
  return continueStatus;
}

CDebugInjectorProcess::CDebugInjectorProcess()
{
  _injected = false;
  _dllWasInjected = false;
  _firstBP = false;
  _exeEntryPoint = 0;
}

void CDebugInjectorProcess::Init(C7maxDevice *device, const CSysString &dllPath)
{
  _device = device;
  _dllPath = dllPath;
}

bool CDebugger::Debug(const UString &command, DWORD processID)
{
  CSysString dllPath;
  if (!GetSevenMaxFilePath(kDllName, dllPath))
    throw kCantFindDLL;

  const UINT32 kPDBase = (0xC0300000);
  const UINT32 kLargePDBase = kPDBase + (1 << 11);
  const kNumPages = 0x100;
  C7maxDevice device;
  UINT32 data[kNumPages];
  const kSize = kNumPages * sizeof(UINT32);
  if (!device.Create())
    throw L"Can't create device";
  UINT32 written;

  if (processID == 0)
  {
    TCHAR *commandLine = new TCHAR[command.Length() + 300];
    lstrcpy(commandLine, command);
    
    STARTUPINFO si;
    si.cb = sizeof(si);
    si.lpReserved = 0;
    si.lpDesktop = 0;
    si.lpTitle = 0;
    si.dwFlags = 0;
    si.cbReserved2 = 0;
    si.lpReserved2 = 0;
    
    PROCESS_INFORMATION pi;
    // MessageBox(0, commandLine, 0, 0);
    BOOL result = ::CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 
      0
      // | CREATE_SUSPENDED
      | DEBUG_PROCESS 
      // | DEBUG_ONLY_THIS_PROCESS
      , NULL, NULL, &si, &pi);
    delete []commandLine;
    if (!result)
      return false;
    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);
  }
  else
  {
    CProcess process;
    if (!process.Open(PROCESS_ALL_ACCESS, FALSE, processID))
      return false;
    if (!InjectViaRemoteThread(process, dllPath))
      return false;
    if (!::DebugActiveProcess(processID))
    {
      OutputDebugString(L"Error 94936273\n");
      return false;
    }
  }
  
  bool keepLoop = true;
  // int t = 0;
  // printf("\n");
  // printf("Start Debug\n");
  while(keepLoop) 
  { 
    DEBUG_EVENT debugEvent;
    ::WaitForDebugEvent(&debugEvent, INFINITE); 
    // printf("%d\n", t++);
    DWORD processID = debugEvent.dwProcessId;
    DWORD continueStatus = DBG_CONTINUE;
    int index;
    for (index = 0; index < _map.Size(); index++)
      if (_map[index].ProcessId == processID)
        break;
    bool finded = (index < _map.Size());
    switch (debugEvent.dwDebugEventCode)
    {
      case CREATE_PROCESS_DEBUG_EVENT: 
      {
        if (finded)
        {
          break; // error
        }
        CloseHandle(debugEvent.u.CreateProcessInfo.hFile);
        // printf("CopyFromRemote\n");
        device.CopyFromRemote(processID, (void *)(kLargePDBase), data, kSize, written);
        if (written == kSize)
        {
          int j;
          for (j = 0; j < kNumPages; j++)
            if (data[j] != 0)
              break;
          if (j == kNumPages)
          {
            // printf("Inject\n");
            CDebugPair debugPair;
            debugPair.ProcessId = processID;
            index = _map.Add(debugPair);
            CDebugInjectorProcess &proc = _map[index].ProcessInfo;
            proc.Init(&device, dllPath);
            finded = true;
          }
        }
        break;
      }
      case EXIT_PROCESS_DEBUG_EVENT:
      {
        if (finded)
        {
          if (_map[index].ProcessInfo._dllWasInjected)
          {
            for (int j = 0; j < kNumPages; j++)
              data[j] = 0;
            device.CopyToRemote(processID, (void *)(kLargePDBase), data, kSize, written);
            // printf("\nEXIT_PROCESS_DEBUG_EVENT %d = \n", written);
            #ifdef _SMAX_MANAGER
            g_PhysicalAllocator->FreeForProcess(processID);
            #endif
          }
          _map.Delete(index);
          finded = false;
        }
        keepLoop = (_map.Size() > 0);
        // printf("EXIT_PROCESS_DEBUG_EVENT\n");
        break;
      }
    }
    if (finded)
      continueStatus = _map[index].ProcessInfo.ProcessEvent(debugEvent);
    if (continueStatus == DBG_EXCEPTION_NOT_HANDLED)
    {
      // Sleep(1000);
      // printf("\nDBG_EXCEPTION_NOT_HANDLED2");
    }
    ContinueDebugEvent(processID, debugEvent.dwThreadId, continueStatus); 
  }
  return true;
}


