// HelpUtils.cpp

#include "StdAfx.h"

#include <HtmlHelp.h>

#include "HelpUtils.h"
#include "../Common/ProgramLocation.h"

static LPCTSTR kHelpFileName = TEXT("7max.chm::/");

void ShowHelpWindow(HWND hwnd, LPCTSTR topicFile)
{
  CSysString path;
  if (!::GetProgramFolderPath(path))
    return;
  path += kHelpFileName;
  path += topicFile;
  HtmlHelp(hwnd, path, HH_DISPLAY_TOPIC, NULL);
}


