// OptionsDialog.h
 
#ifndef __OPTIONSDIALOG_H
#define __OPTIONSDIALOG_H

#include "resource.h"
#include "Windows/Control/Dialog.h"
#include "Windows/Control/Edit.h"

#include "../../BootIni.h"

class COptionsDialog: public NWindows::NControl::CModalDialog
{
  virtual bool OnInit();
  virtual void OnHelp();
  virtual void OnOK();
  bool OnButtonClicked(int buttonID, HWND buttonHWND);
  void SetRecommended();
public:
  INT_PTR Create(HWND parentWindow = 0)
    { return CModalDialog::Create(MAKEINTRESOURCE(IDD_OPTIONS), parentWindow); }
};

#endif
