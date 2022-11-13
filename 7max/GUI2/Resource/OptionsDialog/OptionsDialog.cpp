// OptionsDialog.cpp

#include "StdAfx.h"

#include "../../../Common/7maxRegistry.h"
#include "../../RegistryUtils.h"

#include "resource.h"
#include "OptionsDialog.h"

#include "../../HelpUtils.h"

using namespace NWindows;

static LPCTSTR kHelpTopic = TEXT("options.htm");

static const int kMoveBits = 10;
// extern bool g_AllowAttach;

bool COptionsDialog::OnInit() 
{
  // CheckButton(IDC_OPTIONS_ALLOW_ATTACH, ReadAllowAttach());
  SetItemInt(IDC_OPTIONS_ALLOC_MIN, 
    (ReadAllocMin() + (1 << kMoveBits) - 1) >> kMoveBits, false);
  return CModalDialog::OnInit();
}

bool COptionsDialog::OnButtonClicked(int buttonID, HWND buttonHWND)
{
  switch(buttonID)
  {
    case IDC_OPTIONS_BUTTON_RECOMMENDED:
      SetRecommended();
      break;
    default:
      return CModalDialog::OnButtonClicked(buttonID, buttonHWND);
  }
  return true;
}

void COptionsDialog::SetRecommended()
{
  SetItemInt(IDC_OPTIONS_ALLOC_MIN, GetRecommendedAllocMin() >> kMoveBits, false);
}

void COptionsDialog::OnOK()
{
  UINT systemSize;
  if (!GetItemInt(IDC_OPTIONS_ALLOC_MIN, false, systemSize))
  {
    ::MessageBoxW(HWND(*this), L"Incorrect size", L"7-max", MB_ICONERROR);
    return;
  }
  if (systemSize >= (1 << (32 - kMoveBits)))
  {
    ::MessageBoxW(HWND(*this), L"Size is too big", L"7-max", MB_ICONERROR);
    return;
  }
  systemSize <<= kMoveBits;
  if (systemSize < (1 << 12))
  {
    ::MessageBoxW(HWND(*this), L"Please specify minimal block size 4 KB or larger", L"7-max", MB_ICONERROR);
    return;
  }

  SaveAllocMin(systemSize);

  // g_AllowAttach = IsButtonCheckedBool(IDC_OPTIONS_ALLOW_ATTACH);
  // SaveAllowAttach(g_AllowAttach);
  CModalDialog::OnOK();
}

void COptionsDialog::OnHelp()
{
  ShowHelpWindow(NULL, kHelpTopic);
}
