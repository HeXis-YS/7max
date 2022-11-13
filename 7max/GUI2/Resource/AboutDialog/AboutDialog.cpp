// AboutDialog.cpp

#include "StdAfx.h"

#include "AboutDialog.h"

#include "../../HelpUtils.h"

static LPCTSTR kHomePageURL = TEXT("http://www.7-max.com/");
static LPCTSTR kRegisterURL = TEXT("http://www.7-max.com/register.html");
static LPCTSTR kSupportURL  = TEXT("http://www.7-max.com/support.html");
  
static LPCTSTR kHelpTopic = TEXT("start.htm");

void CAboutDialog::OnHelp()
{
  ShowHelpWindow(NULL, kHelpTopic);
}

static void MyShellExecute(LPCTSTR url)
{
  ::ShellExecute(NULL, NULL, url, NULL, NULL, SW_SHOWNORMAL);
}

bool CAboutDialog::OnButtonClicked(int buttonID, HWND buttonHWND)
{
  switch(buttonID)
  {
    case IDC_ABOUT_BUTTON_HOMEPAGE:
      ::MyShellExecute(kHomePageURL);
      break;
    case IDC_ABOUT_BUTTON_REGISTER:
    {
      ::MyShellExecute(kRegisterURL);
      break;
    }
    case IDC_ABOUT_BUTTON_SUPPORT:
    {
      ::MyShellExecute(kSupportURL);
      break;
    }
    default:
      return CModalDialog::OnButtonClicked(buttonID, buttonHWND);
  }
  return true;
}
