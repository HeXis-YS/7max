// RunDialog.cpp

#include "StdAfx.h"
#include "RunDialog.h"

#include "Windows/Control/Static.h"
#include "Windows/FileDir.h"

#ifdef LANG        
#include "../../LangUtils.h"
#endif

using namespace NWindows;

bool CRunDialog::OnInit() 
{
  _comboBox.Attach(GetItem(IDC_COMBO_COMBO));

  /*
  // why it doesn't work ?
  DWORD style = _comboBox.GetStyle();
  if (Sorted)
    style |= CBS_SORT;
  else
    style &= ~CBS_SORT;
  _comboBox.SetStyle(style);
  */
  NControl::CStatic staticContol;
  staticContol.Attach(GetItem(IDC_COMBO_STATIC));
  _comboBox.SetText(Value);
  for(int i = 0; i < Strings.Size(); i++)
    _comboBox.AddString(Strings[i]);
  return CModalDialog::OnInit();
}

class CDoubleZeroStringList
{
  CRecordVector<int> m_Indexes;
  CSysString m_String;
public:
  void Add(LPCTSTR s);
  void SetForBuffer(LPTSTR buffer);
};

const TCHAR kDelimiterSymbol = TEXT(' ');
void CDoubleZeroStringList::Add(LPCTSTR s)
{
  m_String += s;
  m_Indexes.Add(m_String.Length());
  m_String += kDelimiterSymbol;
}

void CDoubleZeroStringList::SetForBuffer(LPTSTR buffer)
{
  lstrcpy(buffer, m_String);
  for (int i = 0; i < m_Indexes.Size(); i++)
    buffer[m_Indexes[i]] = TEXT('\0');
}

void CRunDialog::OnButtonBrowse() 
{
  const int kBufferSize = MAX_PATH * 2;
  WCHAR buffer[kBufferSize];
  UString fileName;
  _comboBox.GetText(fileName);
  fileName.Trim();
  UString param;
  if (fileName.Length() >= 2)
    if (fileName[0] == L'\"')
    {
      int pos = fileName.Find(L'\"', 1);
      if (pos >= 0)
      {
        param = fileName.Mid(pos);
        param.Trim();
        fileName = fileName.Mid(1, pos - 1);
      }
    }
  UString fullFileName;
  if (!NFile::NDirectory::MyGetFullPathName(fileName, fullFileName))
  {
    fullFileName = fileName;
  }
  lstrcpy(buffer, fullFileName);

  OPENFILENAME info;
  info.lStructSize = sizeof(info); 
  info.hwndOwner = HWND(*this); 
  info.hInstance = 0; 
  
  const int kFilterBufferSize = MAX_PATH;
  TCHAR filterBuffer[kFilterBufferSize];
  CDoubleZeroStringList doubleZeroStringList;
  
  doubleZeroStringList.Add(TEXT("Programs (*.exe)"));
  doubleZeroStringList.Add(TEXT("*.exe"));

  CSysString s = TEXT("All files");
  s += TEXT(" (*.*)");
  doubleZeroStringList.Add(s);
  doubleZeroStringList.Add(TEXT("*.*"));
  doubleZeroStringList.SetForBuffer(filterBuffer);
  info.lpstrFilter = filterBuffer; 
  
  
  info.lpstrCustomFilter = NULL; 
  info.nMaxCustFilter = 0; 
  info.nFilterIndex = 0; 
  
  info.lpstrFile = buffer; 
  info.nMaxFile = kBufferSize;
  
  info.lpstrFileTitle = NULL; 
    info.nMaxFileTitle = 0; 
  
  info.lpstrInitialDir= NULL; 

  CSysString title = TEXT("Browse");
  
  info.lpstrTitle = title;

  info.Flags = OFN_EXPLORER | OFN_HIDEREADONLY; 
  info.nFileOffset = 0; 
  info.nFileExtension = 0; 
  info.lpstrDefExt = NULL; 
  
  info.lCustData = 0; 
  info.lpfnHook = NULL; 
  info.lpTemplateName = NULL; 

  if(!GetOpenFileName(&info))
    return;
  UString sTemp = UString(L"\"") + UString(buffer) + UString(L"\"");
  _comboBox.SetText(sTemp);
}

bool CRunDialog::OnButtonClicked(int buttonID, HWND buttonHWND)
{
  switch(buttonID)
  {
    case IDC_RUN_BROWSE:
    {
      OnButtonBrowse();
      return true;
    }
  }
  return CModalDialog::OnButtonClicked(buttonID, buttonHWND);
}

void CRunDialog::OnOK()
{
  _comboBox.GetText(Value);
  CModalDialog::OnOK();
}
