// ConsoleCloseUtils.h

#pragma once

#ifndef __CONSOLECLOSEUTILS_H
#define __CONSOLECLOSEUTILS_H

bool TestBreakSignal();

class CCtrlHandlerSetter
{
public:
  CCtrlHandlerSetter();
  virtual ~CCtrlHandlerSetter();
  void EnableBreak(bool enable);
};

class CCtrlBreakException 
{};

void CheckCtrlBreak();

#endif

