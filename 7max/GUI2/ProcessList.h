// ProcessList.h

#ifndef __PROCESSLIST_H
#define __PROCESSLIST_H

#include "Windows/Control/ListView.h"
#include "Windows/Control/Static.h"
#include "Windows/Control/Dialog.h"

#include "resource.h"

enum MyMessages
{
  kRefreshButtons  = WM_USER + 1
};

struct CProcessInfo
{
  DWORD ProcessID;
  CSysString Name;
  CSysString Path;
  MODULEINFO ModuleInfo;
  PROCESS_MEMORY_COUNTERS MemCounters;

  bool TimeIsSpeified;
  FILETIME CreationTime;
  FILETIME ExitTime;
  FILETIME KernelTime;
  FILETIME UserTime;

  UINT64 GetSevenMaxMem() const 
  { 
    // return IsSevenMaxMemSpecified ? SevenMaxMem: 0; 
    return 0;
  }
  bool HasSevenMax() const 
  { 
    return false;
  }
  UINT64 GetTotalVirtMem() const 
    { return MemCounters.PagefileUsage + GetSevenMaxMem(); } 

  bool IsSpecified;
  bool SetProcessInfo();
  // HWND hWnd;
  CProcessInfo(): 
    IsSpecified(false), 
    // IsSevenMaxMemSpecified(false),
    TimeIsSpeified(false)
    {}
};

class CProcessesInfo
{
  int FindProcess(DWORD processID);
public:
  CObjectVector<CProcessInfo> Processes;
  // CRecordVector<HWND> Windows;
  void Reload();
};

struct CItemProperty
{
  UString Name;
  PROPID ID;
  // VARTYPE Type;
  int Order;
  // bool IsVisible;
  UINT32 Width;
};

inline bool operator<(const CItemProperty &a1, const CItemProperty &a2)
  { return (a1.Order < a2.Order); }

inline bool operator==(const CItemProperty &a1, const CItemProperty &a2)
  { return (a1.Order == a2.Order); }

class CItemProperties: public CObjectVector<CItemProperty>
{
public:
  int FindItemWithID(PROPID id)
  {
    for (int i = 0; i < Size(); i++)
      if ((*this)[i].ID == id)
        return i;
    return -1;
  }
};

class CMemPanel: public NWindows::NControl::CModelessDialog
{
public:
  void SetInfo(UINT64 total, UINT64 free)
  {
    SetItemInt(IDC_INFO_TOTAL, UINT(total >> 20), false);
    SetItemInt(IDC_INFO_USED, UINT((total - free) >> 20), false);
    SetItemInt(IDC_INFO_FREE, UINT(free >> 20), false);
  }
};

class CButtonsPanel: public NWindows::NControl::CModelessDialog
{
  virtual bool OnButtonClicked(int buttonID, HWND buttonHWND);
public:
  void UpdateButtons();
};

class CProcessList
{
  CItemProperties _visibleProperties;
  UINT32 _ListViewMode;

  void AddColumns();
  void InsertColumn(int columnIndex, const CItemProperty &prop);
  void SetListViewMode(UINT32 index);
  void RefreshListCtrl(bool focusedDefined, DWORD processID);
  LRESULT SetItemText(LVITEM &item);
  CSysString GetPropertyString(int index, PROPID propID);
public:
  NWindows::NControl::CListView _listView;
  CMemPanel _dialogSystemPhys;
  CMemPanel _dialogSystemVirt;
  CMemPanel _dialogSevenMax;
  CMemPanel _dialogTotalPhys;
  CMemPanel _dialogTotalVirt;
  // NWindows::NControl::CStatic _static;

  CButtonsPanel _dialogButtons;

  CProcessesInfo _processes;
  bool _ascending;
  PROPID _sortID;

  bool HideSystemProcesses;

  bool Create(HWND hWnd);
  bool Move(int x, int y, int width, int height, bool repaint = true)
  { 
    int ySizePanel = 0;
    int ySizeButtons = 0;
    if ((HWND)_dialogSystemPhys != 0)
    {
      RECT rect;
      _dialogSystemPhys.GetWindowRect(&rect);
      ySizePanel = rect.bottom - rect.top;
      int xSizePanel = rect.right - rect.left;
      int yPosPanel = y + height - ySizePanel;
      _dialogSystemPhys.Move(0, yPosPanel, xSizePanel, ySizePanel, repaint); 
      _dialogSystemVirt.Move(xSizePanel* 1, yPosPanel, xSizePanel, ySizePanel, repaint); 
      _dialogSevenMax.Move(xSizePanel * 2, yPosPanel, xSizePanel, ySizePanel, repaint); 
      _dialogTotalPhys.Move(xSizePanel * 3, yPosPanel, xSizePanel, ySizePanel, repaint); 
      _dialogTotalVirt.Move(xSizePanel * 4, yPosPanel, xSizePanel, ySizePanel, repaint); 
    }
    if ((HWND)_dialogButtons != 0)
    {
      RECT rect;
      _dialogButtons.GetWindowRect(&rect);
      ySizeButtons = rect.bottom - rect.top;
      int xSizeButtons = rect.right - rect.left;
      int yPosButtons = y + height - ySizePanel - ySizeButtons;
      _dialogButtons.Move(0, yPosButtons, xSizeButtons, ySizeButtons, repaint); 
      _dialogButtons.InvalidateRect(0);
    }
    _listView.Move(x, y, width, height - ySizePanel - ySizeButtons, repaint);
    return true;
  }

  void SetFocusToLastItem()
  { 
    _listView.SetFocus(); 
  }
  void RefreshList();
  bool OnNotifyList(LPNMHDR header, LRESULT &result);
  void OnDestroy()
  { 
    _dialogSystemVirt.Destroy(); 
    _dialogSystemPhys.Destroy(); 
    _dialogSevenMax.Destroy(); 
    _dialogTotalPhys.Destroy(); 
    _dialogTotalVirt.Destroy(); 
  }
  void SortItemsWithPropID(PROPID propID);

  int GetRealItemIndex(int indexInListView) const
  {
    LPARAM param;
    if (!_listView.GetItemParam(indexInListView, param))
      throw 1;
    return param;
  }
  void OnColumnClick(LPNMLISTVIEW info);
};

#endif
