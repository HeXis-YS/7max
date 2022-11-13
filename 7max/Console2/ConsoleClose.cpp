// ConsoleClose.cpp

#include "StdAfx.h"

#include "ConsoleClose.h"

static bool g_EnableBreak = false;
static int g_BreakCounter = 0;
// static const int kBreakAbortThreshold = 2;

static BOOL WINAPI HandlerRoutine(DWORD ctrlType)
{
  // printf("\nHandlerRoutine = %d", ctrlType);
  // Sleep(1000);
  g_BreakCounter++;
  return g_EnableBreak ? FALSE : TRUE;
  /*
  if (g_BreakCounter < kBreakAbortThreshold)
    return TRUE;
  return FALSE;
  */
  /*
  switch(ctrlType)
  {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
      if (g_BreakCounter < kBreakAbortThreshold)
      return TRUE;
  }
  return FALSE;
  */
}

bool TestBreakSignal()
{
  /*
  if (g_BreakCounter > 0)
    return true;
  */
  return (g_BreakCounter > 0);
}

void CheckCtrlBreak()
{
  if (TestBreakSignal())
    throw CCtrlBreakException();
}

CCtrlHandlerSetter::CCtrlHandlerSetter()
{
  if(!SetConsoleCtrlHandler(HandlerRoutine, TRUE))
    throw "SetConsoleCtrlHandler fails";
}

CCtrlHandlerSetter::~CCtrlHandlerSetter()
{
  if(!SetConsoleCtrlHandler(HandlerRoutine, FALSE))
    throw "SetConsoleCtrlHandler fails";
}

void CCtrlHandlerSetter::EnableBreak(bool enable)
{
  g_EnableBreak = enable;
}

