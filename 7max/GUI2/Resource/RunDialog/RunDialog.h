// RunDialog.h

#ifndef __RUNDIALOG_H
#define __RUNDIALOG_H

#include "Windows/Control/Dialog.h"
#include "Windows/Control/ComboBox.h"
#include "resource.h"

class CRunDialog: public NWindows::NControl::CModalDialog
{
  NWindows::NControl::CComboBox _comboBox;
  virtual void OnOK();
  virtual bool OnInit();
  virtual bool OnButtonClicked(int buttonID, HWND buttonHWND);
public:
  // bool Sorted;
  UString Value;
  CSysStringVector Strings;

  // CRunDialog(): Sorted(false) {};
  INT_PTR Create(HWND parentWindow = 0)
    { return CModalDialog::Create(MAKEINTRESOURCE(IDD_DIALOG_RUN), parentWindow); }
  void OnButtonBrowse();

};

#endif
