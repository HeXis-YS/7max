// MenuUtils.cpp

#include "StdAfx.h"

#include "Windows/Menu.h"
#include "RegistryUtils.h"
#include "resource.h"
// #include "ProcessList.h"
#include "Resource/RunDialog/RunDialog.h"
#include "Resource/AboutDialog/AboutDialog.h"
#include "Resource/OptionsDialog/OptionsDialog.h"
#include "HelpUtils.h"
#include "RunProcess.h"
#include "../Common/ProcessStatus.h"

static LPCTSTR kHelpTopic = TEXT("start.htm");

using namespace NWindows;

extern HWND g_HWND;
// extern CProcessList g_ProcessList;

static const int kViewMenuIndex = 1;
static const UINT kRunFromHistoryMenuID = 200;
static const UINT32 kNumHistoryItems = 4;

UStringVector g_Commands;

static bool MyCreateProcess(TCHAR *cmdLine, bool suspend, PROCESS_INFORMATION *pi)
{
  STARTUPINFO si;
  ::ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  return ::CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, suspend ? CREATE_SUSPENDED : 0, NULL, NULL, &si, pi) != FALSE;
}
  

static bool DefragRam()
{
  TCHAR commandLine[kPathNames + 100];
  lstrcpy(commandLine, g_IsSystem64 ? g_ConExePath64: g_ConExePath32);
  lstrcat(commandLine, L" -defrag");
  PROCESS_INFORMATION pi;
  bool result = ::MyCreateProcess(commandLine, false, &pi);
  ::CloseHandle(pi.hThread);
  ::CloseHandle(pi.hProcess);
  return result;
}

void OnMenuActivating(HWND hWnd, HMENU hMenu, int position)
{
  if (::GetSubMenu(::GetMenu(g_HWND), position) != hMenu)
    return;
  if (position == 0) // File
  {
    ReadCommandsHistory(g_Commands);

    CMenu menu;
    menu.Attach(hMenu);

    const int kNumKeepBefore = 2;
    const int kNumKeepAfter = 1;

    while (menu.GetItemCount() > kNumKeepBefore + kNumKeepAfter)
      menu.RemoveItem(kNumKeepBefore, MF_BYPOSITION);
    int i;
    for (i = 0; i < g_Commands.Size() && i < kNumHistoryItems; i++)
    {
      UString path = g_Commands[i];
      const int kMaxSize = 100;
      const int kFirstPartSize = kMaxSize / 2;
      if (path.Length() > kMaxSize)
        path = path.Left(kFirstPartSize) + UString(L" ... ") +
            path.Right(kMaxSize - kFirstPartSize);
      menu.Insert(kNumKeepBefore + i, MF_BYPOSITION | MF_STRING , 
          kRunFromHistoryMenuID + i, path);
    }
    if (g_Commands.Size() > 0)
    {
      menu.Insert(kNumKeepBefore + i, 
          MF_BYPOSITION | MF_SEPARATOR, 
          kRunFromHistoryMenuID + i, TEXT(""));
    }
  }
  /*
  else if (position == kViewMenuIndex)
  {
    CMenu menu;
    menu.Attach(hMenu);
    menu.CheckItem(IDM_VIEW_HIDE_SYSTEM_PROCESSES, 
        MF_BYCOMMAND | ((g_ProcessList.HideSystemProcesses) 
        ? MF_CHECKED : MF_UNCHECKED));
  }
  */
}

void inline AddUniqueStringToHead(UStringVector &list, 
    const UString &string)
{
  for(int i = 0; i < list.Size();)
    if (string.CompareNoCase(list[i]) == 0)
      list.Delete(i);
    else
      i++;
  list.Insert(0, string);
}

static void RunCommand(const UString &s)
{
  if (!RunProcess(s))
    MessageBox(0, L"Can't run", TEXT("7-max error"), MB_ICONERROR);
}

static void OnRun(HWND hWnd)
{
  CRunDialog runDialog;
  UStringVector &strings = runDialog.Strings;
  ReadCommandsHistory(strings);
  if (strings.Size() > 0)
    runDialog.Value = strings.Front();
  if (runDialog.Create(hWnd) != IDOK)
    return;
  AddUniqueStringToHead(strings, runDialog.Value);
  const int kMaxSize = 30;
  if (strings.Size() > kMaxSize)
    strings.Delete(kMaxSize, strings.Size() - kMaxSize);
  SaveCommandsHistory(strings);
  RunCommand(runDialog.Value);
}

static void RunCommand(int index)
{
  if (index >= g_Commands.Size())
    return;
  UString s = g_Commands[index];
  if (index != 0)
  {
    g_Commands.Delete(index);
    g_Commands.Insert(0, s);
    SaveCommandsHistory(g_Commands);
  }
  RunCommand(s);
}

bool OnMenuCommand(HWND hWnd, int id)
{
  if (id >= kRunFromHistoryMenuID && 
      id < kRunFromHistoryMenuID + kNumHistoryItems)
  {
    RunCommand(id - kRunFromHistoryMenuID);
    return true;
  }

  switch (id)
  {
    case IDM_RUN:
    {
      OnRun(hWnd);
      return true;
    }
    /*
    case IDM_VIEW_HIDE_SYSTEM_PROCESSES:
    {
      g_ProcessList.HideSystemProcesses = 
        !g_ProcessList.HideSystemProcesses;
      g_ProcessList.RefreshList();
      return true;
    }
    */
    case IDM_ABOUT:
    {
      CAboutDialog dialog;
      dialog.Create(hWnd);
      return true;
    }
    case IDM_OPTIONS:
    {
      COptionsDialog dialog;
      dialog.Create(hWnd);
      return true;
    }
    case IDM_DEFRAG:
    {
      DefragRam();
      return true;
    }
    case IDM_HELPTOPICS:
      ShowHelpWindow(NULL, kHelpTopic);
      break;
  }
  return false;
}
