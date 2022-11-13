// RunProcess.cpp

#include "StdAfx.h"

#include "../Common/InjectDLL3.h"

bool RunProcess(const UString &command)
{
  try
  {
    return StartProcessVia7max(command, false);
  }
  catch(...) { MessageBoxW(0, L"Unspecified Error", L"7-max", MB_ICONERROR); }
  return false;
}

bool RunProcess(DWORD processID)
{
  try
  {
    return InjectToProcess(processID);
  }
  catch(...) { MessageBoxW(0, L"Unspecified Error", L"7-max", MB_ICONERROR); }
  return false;
}

