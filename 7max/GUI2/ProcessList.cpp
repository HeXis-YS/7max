// ProcessList.cpp

#include "StdAfx.h"

#include "Common/StringConvert.h"
#include "Common/IntToString.h"
#include "Common/Defs.h"
#include "Windows/Process.h"
#include "Windows/FileDir.h"
#include "Windows/Control/Dialog.h"
#include "Windows/Synchronization.h"
#include "ProcessList.h"
#include "RunProcess.h"
#include "RegistryUtils.h"

#include "../Common/7maxRegistry.h"

extern HINSTANCE g_hInstance;
extern HWND g_HWND;

UINT32 _ID = 100;

using namespace NWindows;

bool g_AllowAttach = false;
extern CProcessList g_ProcessList;
static NSynchronization::CCriticalSection g_ProcessesCS;
extern void MessageBoxError(const wchar_t *s);

enum EColumnID
{
  kColumnIDName,
  kColumnIDPath,
  kColumnIDSize,
  kColumnIDBase,
  kColumnIDEntryPoint,
  kColumnIDWorkingSet,
  kColumnIDPageFileUsage,

  kColumnIDStartTime,

  kColumnID7maxMem,
  kColumnIDTotalVirtMem,
  kColumnProcessID,
};

static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lpData);

bool CProcessInfo::SetProcessInfo()
{
  CProcess process;
  bool result = false;
  if (process.Open(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
        ,false, ProcessID))
  {
    TimeIsSpeified = process.GetTimes(&CreationTime, &ExitTime, &KernelTime, &UserTime);
    HMODULE module;
    DWORD cbNeeded;
    if (process.EnumModules(&module, sizeof(module), &cbNeeded))
      if (cbNeeded = sizeof(module))
      {
        result = true;
        result &= process.MyGetModuleBaseName(module, Name);
        result &= process.MyGetModuleFileNameEx(module, Path);
        result &= process.GetModuleInformation(module, &ModuleInfo);
        result &= process.GetMemoryInfo(&MemCounters);
      }
  }
  return result;
}

/*
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
  CProcessesInfo *processesInfo = (CProcessesInfo *)lParam;
  processesInfo->Windows.Add(hwnd);
  return TRUE;
}
*/

int CProcessesInfo::FindProcess(DWORD processID)
{
  for (int i = 0; i < Processes.Size(); i++)
    if (Processes[i].ProcessID == processID)
      return i;
  return -1;
}

void CProcessesInfo::Reload()
{
  Processes.Clear();
  // Windows.Clear();
  DWORD processes[1024 * 4];
  DWORD returnedBytes;
  
  if (!::EnumProcesses(processes, sizeof(processes), &returnedBytes) )
    return;
  int numProcesses = returnedBytes / sizeof(DWORD);
  int i;
  for(i = 0; i < numProcesses; i++)
  {
    CProcessInfo processInfo;
    processInfo.ProcessID = processes[i];
    processInfo.IsSpecified = processInfo.SetProcessInfo();
    if (!processInfo.IsSpecified)
      processInfo.Name = TEXT("Unknown");
    Processes.Add(processInfo);
  }
  /*
  EnumWindows(EnumWindowsProc, (LPARAM)this);
  for(i = 0; i < Windows.Size(); i++)
  {
    HWND hWnd = Windows[i];
    DWORD processID;
    GetWindowThreadProcessId(hWnd, &processID);
    int index = FindProcess(processID);
    if (index < 0)
      continue;
    Processes[index].hWnd = hWnd;
  }
  */
}


static int GetRealIconIndex(const CSysString &fileName)
{
  SHFILEINFO shellInfo;
  ::SHGetFileInfo(fileName, FILE_ATTRIBUTE_NORMAL, &shellInfo, 
      sizeof(shellInfo), SHGFI_SYSICONINDEX);
  return shellInfo.iIcon;
}

static void OnChangedFocused()
{
  PostMessage(g_HWND, kRefreshButtons, 0, 0);
}

bool CProcessList::Create(HWND hWnd)
{
  HideSystemProcesses = true;
  _ascending = true;
  _sortID = kColumnIDName;

  DWORD style = WS_CHILD | WS_VISIBLE
    // | WS_BORDER
    | LVS_SHAREIMAGELISTS
    | LVS_SHOWSELALWAYS
    // | LVS_SHAREIMAGELISTS
    // | LVS_AUTOARRANGE
    | WS_CLIPCHILDREN
    | WS_CLIPSIBLINGS
    | WS_TABSTOP 
    | LVS_REPORT 
    // | LVS_EDITLABELS 
    | LVS_SINGLESEL
    ;
  DWORD exStyle= WS_EX_CLIENTEDGE;
  if (!_listView.CreateEx(exStyle, style, 0, 0, 116, 260, 
      hWnd, (HMENU)_ID, g_hInstance, NULL))
    return false;

  /*
  _static.Attach(CreateWindow(TEXT("STATIC"), TEXT("Test"), 
    WS_CHILD | WS_VISIBLE | SS_LEFT,
    0,0,0,0, hWnd, (HMENU)(_ID + 1), g_hInstance, NULL));
  */

  _dialogButtons.Create(MAKEINTRESOURCE(IDD_BUTTONS), hWnd);
  _dialogButtons.Show(SW_SHOWNORMAL);

  _dialogSystemPhys.Create(MAKEINTRESOURCE(IDD_INFO_PANEL), hWnd);
  _dialogSystemPhys.Show(SW_SHOWNORMAL);
  _dialogSystemPhys.SetItemText(IDC_INFO_CAPTION, TEXT("Physical Sys (MB)"));
  
  _dialogSystemVirt.Create(MAKEINTRESOURCE(IDD_INFO_PANEL), hWnd);
  _dialogSystemVirt.Show(SW_SHOWNORMAL);
  _dialogSystemVirt.SetItemText(IDC_INFO_CAPTION, TEXT("Virtual Sys (MB)"));

  _dialogSevenMax.Create(MAKEINTRESOURCE(IDD_INFO_PANEL), hWnd);
  _dialogSevenMax.Show(SW_SHOWNORMAL);
  _dialogSevenMax.SetItemText(IDC_INFO_CAPTION, TEXT("7-max (MB)"));
  
  _dialogTotalPhys.Create(MAKEINTRESOURCE(IDD_INFO_PANEL), hWnd);
  _dialogTotalPhys.Show(SW_SHOWNORMAL);
  _dialogTotalPhys.SetItemText(IDC_INFO_CAPTION, TEXT("Physical Totals (MB)"));

  _dialogTotalVirt.Create(MAKEINTRESOURCE(IDD_INFO_PANEL), hWnd);
  _dialogTotalVirt.Show(SW_SHOWNORMAL);
  _dialogTotalVirt.SetItemText(IDC_INFO_CAPTION, TEXT("Virtual Totals (MB)"));

  /*
  _static.CreateEx(0, TEXT("Statiñ"), TEXT("Test"), 
    WS_CHILD | WS_VISIBLE | SS_LEFT,
    0,0,0,0, hWnd, (HMENU)(_ID + 1), g_hInstance, NULL);
  */
  SHFILEINFO shellInfo;
  HIMAGELIST imageList = (HIMAGELIST)SHGetFileInfo(TEXT(""), 
      FILE_ATTRIBUTE_NORMAL |
      FILE_ATTRIBUTE_DIRECTORY, 
      &shellInfo, sizeof(shellInfo), 
      SHGFI_USEFILEATTRIBUTES | 
      SHGFI_SYSICONINDEX |
      SHGFI_SMALLICON
      );
  _listView.SetImageList(imageList, LVSIL_SMALL);
  imageList = (HIMAGELIST)SHGetFileInfo(TEXT(""), 
      FILE_ATTRIBUTE_NORMAL |
      FILE_ATTRIBUTE_DIRECTORY, 
      &shellInfo, sizeof(shellInfo), 
      SHGFI_USEFILEATTRIBUTES | 
      SHGFI_SYSICONINDEX |
      SHGFI_ICON
      );
  _listView.SetImageList(imageList, LVSIL_NORMAL);

  SetListViewMode(4);
  _listView.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
  AddColumns();
  _listView.SetFocus();

  g_AllowAttach = ReadAllowAttach();
  RefreshListCtrl(false, 0);


  return true;

  /*
  _listView.SetUserDataLongPtr(LONG_PTR(&_listView));
  _listView._panel = this;
  _listView._origWindowProc = (WNDPROC)_listView.SetLongPtr(GWLP_WNDPROC,
      LONG_PTR(ListViewSubclassProc));
  */
}

void CProcessList::SetListViewMode(UINT32 index)
{
  if (index >= 4)
    return;
  _ListViewMode = index;
  DWORD oldStyle = _listView.GetStyle();
  static DWORD kStyles[4] = { LVS_ICON, LVS_SMALLICON, LVS_LIST, LVS_REPORT };
  DWORD newStyle = kStyles[index];
  if ((oldStyle & LVS_TYPEMASK) != newStyle)
    _listView.SetStyle((oldStyle & ~LVS_TYPEMASK) | newStyle);
  // RefreshListCtrlSaveFocused();
}

struct CColumnInfo
{
  UINT32 ID;
  TCHAR *Name;
  int Align;
  int Width;
  bool Show;
};

static CColumnInfo g_Columns[] = 
{
  { kColumnIDName, TEXT("Process"), LVCFMT_LEFT,  100, true },
  { kColumnIDPath, TEXT("Path"), LVCFMT_LEFT, 100, true },
  { kColumnIDSize, TEXT("Size"), LVCFMT_RIGHT, 60, false  },
  { kColumnIDBase, TEXT("Base"), LVCFMT_RIGHT, 80, false  },
  { kColumnIDEntryPoint, TEXT("Entry"), LVCFMT_RIGHT, 80, false },
  { kColumnIDWorkingSet, TEXT("Sys Phys"), LVCFMT_RIGHT, 60, true },
  { kColumnIDPageFileUsage, TEXT("Sys VM"), LVCFMT_RIGHT, 60, true },

  { kColumnID7maxMem, TEXT("7-max"), LVCFMT_RIGHT, 60, true },
  { kColumnIDTotalVirtMem, TEXT("Total VM"), LVCFMT_RIGHT, 60, true },

  { kColumnIDStartTime, TEXT("Start Time"), LVCFMT_RIGHT, 80, true },
  { kColumnProcessID, TEXT("PID"), LVCFMT_RIGHT, 40, true }

};

int FindColumn(UINT32 id)
{
  for (int i = 0; i < sizeof(g_Columns) / sizeof(g_Columns[0]); i++)
    if (g_Columns[i].ID == id)
      return i;
  return -1;
}

void CProcessList::InsertColumn(int columnIndex, 
    const CItemProperty &prop)
{
  const CColumnInfo &columnInfo = g_Columns[FindColumn(prop.ID)];
  LV_COLUMN column;
  column.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCF_ORDER;
  TCHAR string[256];
  column.pszText = string;
  column.cx = prop.Width;
  column.fmt = columnInfo.Align;
  column.iOrder = prop.Order;
  column.iSubItem = columnIndex;
  lstrcpy(string, prop.Name);
  _listView.InsertColumn(columnIndex, &column);
}

void CProcessList::AddColumns()
{
  _visibleProperties.Clear();
  int i;
  for(i = 0; i < sizeof(g_Columns) / sizeof(g_Columns[0]); i++)
  {
    const CColumnInfo &column = g_Columns[i];
    if (!column.Show)
      continue;
    CItemProperty prop;
    prop.Name = column.Name;
    prop.ID = column.ID;
    prop.Width = column.Width;
    prop.Order = i;
    _visibleProperties.Add(prop);
  }
  for(i = 0; i < _visibleProperties.Size(); i++)
    InsertColumn(i, _visibleProperties[i]);
}




static CSysString GetTimeString(FILETIME &fileTime)
{
  TCHAR result[64] = { 0 };
  SYSTEMTIME systemTime;
  if (FileTimeToSystemTime(&fileTime, &systemTime))
  {
    wsprintf(result, TEXT(" %02d:%02d:%02d"), 
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
  }
  return result;
}

static CSysString GetSizeString(UINT64 size)
{
  TCHAR result[32] = { 0 };
  // if (size >= (10 << 20))
  {
    ConvertUInt64ToString(((size + (1 << 20) - 1)>> 20), result);
    lstrcat(result, TEXT(" M"));
  }
  /*
  else   
  {
    ConvertUInt64ToString(((size + (1 << 10) - 1) >> 10), result);
    lstrcat(result, TEXT(" K"));
  }
  */
  return result;
}

CSysString CProcessList::GetPropertyString(int index, PROPID propID)
{
  const CProcessInfo &processInfo = _processes.Processes[index];
  TCHAR result[1024] = { 0 };
  switch(propID)
  {
    case kColumnIDName:
      return processInfo.Name;
    case kColumnIDPath:
      return processInfo.Path;
    case kColumnIDSize:
      if (processInfo.IsSpecified)
        return GetSizeString(processInfo.ModuleInfo.SizeOfImage);
      break;
    case kColumnIDBase:
      if (processInfo.IsSpecified)
        wsprintf(result, TEXT("%08X"), UINT32(processInfo.ModuleInfo.lpBaseOfDll));
      break;
    case kColumnIDEntryPoint:
      if (processInfo.IsSpecified)
        wsprintf(result, TEXT("%08X"), UINT32(processInfo.ModuleInfo.EntryPoint));
      break;
    case kColumnIDWorkingSet:
      if (processInfo.IsSpecified)
        return GetSizeString(processInfo.MemCounters.WorkingSetSize);
      break;
    case kColumnIDPageFileUsage:
      if (processInfo.IsSpecified)
        return GetSizeString(processInfo.MemCounters.PagefileUsage);
      break;
    
    case kColumnID7maxMem:
    {
      UINT64 size = 0;
      if (size > 0)
        return GetSizeString(size);
      break;
    }
    case kColumnIDTotalVirtMem:
      if (processInfo.IsSpecified)
        return GetSizeString(processInfo.GetTotalVirtMem());
      break;
    case kColumnIDStartTime:
      if (processInfo.TimeIsSpeified)
      {
        FILETIME localTime;
        if (FileTimeToLocalFileTime(&processInfo.CreationTime, &localTime))
          return GetTimeString(localTime);
      }
      break;
    case kColumnProcessID:
      wsprintf(result, TEXT("%d"), UINT32(processInfo.ProcessID));
      break;
  }
  return result;
}

void CProcessList::RefreshListCtrl(bool focusedDefined, DWORD processID)
{
  // OutputDebugStringA("=======\n");
  // OutputDebugStringA("s1 \n");
  // CDisableTimerProcessing timerProcessing(*this);

  int focusedPos = 0;

  _listView.SetRedraw(false);
  // m_RedrawEnabled = false;

  LVITEM item;
  ::ZeroMemory(&item, sizeof(item));
  
  int cursorIndex = -1;  
  {
  NSynchronization::CCriticalSectionLock lock(g_ProcessesCS);
  _listView.DeleteAllItems();
  // _selectedStatusVector.Clear();
  // _realIndices.Clear();
  // _selectionIsDefined = false;
  
  // InitColumns();

  // OutputDebugString(TEXT("Start Dir\n"));

  // _selectedStatusVector.Reserve(numItems);

  // OutputDebugStringA("S1\n");

  // g_PhysicalAllocator->GetSnapshot(_physAllocator);

  _processes.Reload();
  UINT32 numItems = _processes.Processes.Size();
  _listView.SetItemCount(numItems);

  CSysString systemDirectory;
  NFile::NDirectory::MyGetSystemDirectory(systemDirectory);
  int systemDirectoryPathLength = systemDirectory.Length();
  CSysString systemDirectory2 = CSysString(TEXT("\\??\\")) + systemDirectory;
  int systemDirectory2PathLength = systemDirectory2.Length();
  CSysString systemDirectory3 = TEXT("\\SystemRoot\\System32\\");
  for(UINT32 i = 0; i < numItems; i++)
  {
    const CProcessInfo &processInfo = _processes.Processes[i];

    if (HideSystemProcesses)
    {
      if (!processInfo.IsSpecified)
        continue;
      if (systemDirectoryPathLength > 0)
      {
        if (systemDirectory.CompareNoCase(
          processInfo.Path.Left(systemDirectoryPathLength)) == 0)
          continue;
        if (systemDirectory2.CompareNoCase(
          processInfo.Path.Left(systemDirectory2PathLength)) == 0)
          continue;
      }
      if (systemDirectory3.CompareNoCase(
        processInfo.Path.Left(systemDirectory3.Length())) == 0)
        continue;
    }

    /*
    if (processInfo.hWnd == 0)
      continue;
    if (IsWindowVisible(processInfo.hWnd) == TRUE)
      continue;
    */
    bool isSpecified = processInfo.IsSpecified;

    bool selected = false;
    if (processInfo.ProcessID == processID)
    {
      cursorIndex = _listView.GetItemCount();
      selected = true;
    }
    // _selectedStatusVector.Add(selected);

    item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
    // item.mask = LVIF_TEXT | LVIF_PARAM;

    int subItem = 0;
    item.iItem = _listView.GetItemCount();
    item.lParam = i;
    item.state = selected ? (LVIS_SELECTED) : 0;
    // item.stateMask = item.state;
   
    item.iSubItem = subItem++;
    
    const int kMaxNameSize = MAX_PATH * 2;
    TCHAR string[kMaxNameSize];
    lstrcpyn(string, processInfo.Name, kMaxNameSize);
    item.pszText = string;
    if (isSpecified)
      item.iImage = GetRealIconIndex(processInfo.Path);
    else
      item.iImage = -1;
    if(_listView.InsertItem(&item) == -1)
      return; // error
  }
  if(_listView.GetItemCount() > 0 && cursorIndex >= 0)
  {
    UINT state = LVIS_FOCUSED | LVIS_SELECTED;
    _listView.SetItemState(cursorIndex, state, state);
  }
  _listView.SortItems(CompareItems, (LPARAM)this);
  if (cursorIndex < 0 && _listView.GetItemCount() > 0)
  {
    if (focusedPos >= _listView.GetItemCount())
      focusedPos = _listView.GetItemCount() - 1;
    UINT state = LVIS_FOCUSED | LVIS_SELECTED;
    _listView.SetItemState(focusedPos, state, state);
  }
  }

  // OutputDebugStringA("End2\n");

  UINT64 smaxTotalSize = 0, smaxFreeSize = 0;
  // _physAllocator.GetSizes(smaxTotalSize, smaxFreeSize);

  _dialogSevenMax.SetInfo(smaxTotalSize, smaxFreeSize);

  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof(statex);
  if (::GlobalMemoryStatusEx(&statex))
  {
    _dialogSystemPhys.SetInfo(statex.ullTotalPhys, statex.ullAvailPhys);
    _dialogSystemVirt.SetInfo(statex.ullTotalPageFile, statex.ullAvailPageFile  );
    _dialogTotalPhys.SetInfo(statex.ullTotalPhys + smaxTotalSize, 
        statex.ullAvailPhys + smaxFreeSize);
    _dialogTotalVirt.SetInfo(statex.ullTotalPageFile  + smaxTotalSize, 
        statex.ullAvailPageFile + smaxFreeSize);
  }

  // m_RedrawEnabled = true;
  _listView.SetRedraw(true);
  _listView.InvalidateRect(NULL, true);
  _listView.EnsureVisible(_listView.GetFocusedItem(), false);
  // OutputDebugStringA("End1\n");
  /*
  _listView.UpdateWindow();
  */
  _dialogButtons.UpdateButtons();
  OnChangedFocused();
}

void CProcessList::RefreshList()
{
  bool focusedDefined = false;
  DWORD processID = 0;
  {
    NSynchronization::CCriticalSectionLock lock(g_ProcessesCS);
    int index = _listView.GetFocusedItem();
    if (index >= 0)
    {
      focusedDefined = true;
      processID = _processes.Processes[g_ProcessList.GetRealItemIndex(index)].ProcessID;
    }
  }
  Sleep(0);
  RefreshListCtrl(focusedDefined, processID);
}

LRESULT CProcessList::SetItemText(LVITEM &item)
{
  if ((item.mask & LVIF_TEXT) == 0)
    return 0;
  UINT32 subItemIndex = item.iSubItem;
  PROPID propID = _visibleProperties[subItemIndex].ID;
  CSysString propString = GetPropertyString(item.lParam, propID);
  int size = item.cchTextMax;
  if(size > 0)
  {
    if(propString.Length() + 1 > size)
      propString = propString.Left(size - 1);
    lstrcpy(item.pszText, propString);
  }
  return 0;
}

bool CProcessList::OnNotifyList(LPNMHDR header, LRESULT &result)
{
  switch(header->code)
  {
    case LVN_GETDISPINFO:
    {
      LV_DISPINFO  *dispInfo = (LV_DISPINFO *)header;

      //is the sub-item information being requested?

      if((dispInfo->item.mask & LVIF_TEXT) != 0 || 
        (dispInfo->item.mask & LVIF_IMAGE) != 0)
        SetItemText(dispInfo->item);
      return false;
    }
    case LVN_KEYDOWN:
    case NM_DBLCLK:
    case NM_RCLICK:
    case NM_CLICK:
    case LVN_BEGINDRAG:
    case LVN_BEGINRDRAG:
    {
      OnChangedFocused();
      break;
    }
    case LVN_COLUMNCLICK:
      OnColumnClick(LPNMLISTVIEW(header));
      return false;
    /*
    case NM_RETURN:
    {
      bool alt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
      bool ctrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
      bool leftCtrl = (::GetKeyState(VK_LCONTROL) & 0x8000) != 0;
      bool RightCtrl = (::GetKeyState(VK_RCONTROL) & 0x8000) != 0;
      bool shift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
      if (!shift && alt && !ctrl)
      {
        Properties();
        return false;
      }
      OpenSelectedItems(true);
      return false;
    }
      OnChangedFocused();
      break;
    */
    /*
    case NM_CLICK:
    {
      if(g_ComCtl32Version >= MAKELONG(71, 4))
      {
        OnLeftClick((LPNMITEMACTIVATE)header);
      }
      return false;
      break;
    }
    */
  }
  return false;
}

int CALLBACK CompareItems2(LPARAM lParam1, LPARAM lParam2, LPARAM lpData)
{
  CProcessList *processList = (CProcessList *)lpData;
  const int kNameSize = 1024;
  
  const CProcessInfo &p1 = processList->_processes.Processes[lParam1];
  const CProcessInfo &p2 = processList->_processes.Processes[lParam2];
  switch (processList->_sortID)
  {
    case kColumnIDName:
      return p1.Name.CompareNoCase(p2.Name);
    case kColumnIDPath:
      return p1.Path.CompareNoCase(p2.Path);
    case kColumnIDStartTime:
      if (p1.TimeIsSpeified && p2.TimeIsSpeified)
        return CompareFileTime(&p1.CreationTime, &p2.CreationTime);
      break;
    case kColumnIDWorkingSet:
      if (p1.TimeIsSpeified && p2.TimeIsSpeified)
        return MyCompare(p1.MemCounters.WorkingSetSize, p2.MemCounters.WorkingSetSize);
      break;
    case kColumnIDPageFileUsage:
      if (p1.TimeIsSpeified && p2.TimeIsSpeified)
        return MyCompare(p1.MemCounters.PagefileUsage, p2.MemCounters.PagefileUsage);
      break;
    case kColumnID7maxMem:
      {
        UINT64 s1 = p1.GetSevenMaxMem();
        UINT64 s2 = p2.GetSevenMaxMem();
        int t = MyCompare(s1, s2);
        if (t != 0)
          return  t;
        bool h1 = p1.HasSevenMax();
        bool h2 = p2.HasSevenMax();
        return MyCompare(h1, h2);
      }
    case kColumnIDTotalVirtMem:
      {
        UINT64 s1 = p1.GetTotalVirtMem();
        UINT64 s2 = p2.GetTotalVirtMem();
        return MyCompare(s1, s2);
      }
  }
  return 0;
}

int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lpData)
{
  if(lpData == NULL)
    return 0;
  CProcessList *processList = (CProcessList *)lpData;
  int result = CompareItems2(lParam1, lParam2, lpData);
  if(lpData == NULL)
    return 0;
  return processList->_ascending ? result: (-result);
}

void CProcessList::SortItemsWithPropID(PROPID propID)
{
  if(propID == _sortID)
    _ascending = !_ascending;
  else
  {
    _sortID = propID;
    _ascending = true;
    switch (propID)
    {
      case kColumnIDWorkingSet:
      case kColumnIDStartTime:
      case kColumnIDPageFileUsage:
      case kColumnID7maxMem:
      case kColumnIDTotalVirtMem:
        _ascending = false;
        break;
    }
  }
  _listView.SortItems(CompareItems, (LPARAM)this);
  _listView.EnsureVisible(_listView.GetFocusedItem(), false);
}

void CProcessList::OnColumnClick(LPNMLISTVIEW info)
{
  SortItemsWithPropID(_visibleProperties[info->iSubItem].ID);
}

static bool CanAttach(DWORD &processID, UString &name)
{
  {
    NSynchronization::CCriticalSectionLock lock(g_ProcessesCS);
    int index = g_ProcessList._listView.GetFocusedItem();
    if (index < 0)
      return false;
    int realIndex = g_ProcessList.GetRealItemIndex(index);
    const CProcessInfo &processInfo = g_ProcessList._processes.Processes[realIndex];
    processID = processInfo.ProcessID;
    name = processInfo.Name;
  }
  if (name.CompareNoCase(L"7max.exe") == 0 || 
      name.CompareNoCase(L"7maxc.exe") == 0)
    return false;
  // return (!_physAllocator.Is7maxProcess(processID));
  return true;
}

static void OnButtonAttach()
{
  UString name;
  DWORD processID;
  if (!CanAttach(processID, name))
  {
    MessageBoxError(L"7-max is already attached to that process");
    return;
  }
  if(!RunProcess(processID))
  {
    MessageBoxError(L"Cannot attach 7-max to process");
    return;
  }
  g_ProcessList.RefreshList();
}

bool CButtonsPanel::OnButtonClicked(int buttonID, HWND buttonHWND)
{
  switch(buttonID)
  {
    case IDC_BUTTON_ATTACH:
      OnButtonAttach();
      break;
    default:
      return false;
  }
  return 0;
}

void CButtonsPanel::UpdateButtons()
{
  UString name;
  DWORD processID;
  ::EnableWindow(GetItem(IDC_BUTTON_ATTACH), g_AllowAttach && CanAttach(processID, name));
}

