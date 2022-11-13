// ContextMenu.cpp

#include "StdAfx.h"

#include "ContextMenu.h"

#include "Common/StringConvert.h"
#include "Common/MyCom.h"

#include "Windows/Shell.h"
#include "Windows/Memory.h"
#include "Windows/COM.h"
#include "Windows/FileFind.h"
#include "Windows/FileDir.h"

#include "Windows/Menu.h"

#include "../Common/ProgramLocation.h"

using namespace NWindows;

static LPCTSTR kOpenVerb = TEXT("SevenMaxOpen");
static LPCTSTR kCommandString = TEXT("Run with 7-max");
static LPCTSTR kHelpString = TEXT("Starts the selected program with 7-max");

HRESULT C7maxContextMenu::GetFileNames(LPDATAOBJECT dataObject, 
    CSysStringVector &fileNames)
{
  fileNames.Clear();
  if(dataObject == NULL)
    return E_FAIL;
  FORMATETC fmte = {CF_HDROP,  NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
  NCOM::CStgMedium stgMedium;
  HRESULT result = dataObject->GetData(&fmte, &stgMedium);
  if (result != S_OK)
    return result;
  stgMedium._mustBeReleased = true;

  NShell::CDrop drop(false);
  NMemory::CGlobalLock globalLock(stgMedium->hGlobal);
  drop.Attach((HDROP)globalLock.GetPointer());
  drop.QueryFileNames(fileNames);

  return S_OK;
}

STDMETHODIMP C7maxContextMenu::Initialize(LPCITEMIDLIST pidlFolder, 
    LPDATAOBJECT dataObject, HKEY hkeyProgID)
{
  return GetFileNames(dataObject, _fileNames);
}

void MessageBoxError(const wchar_t *s)
{
  MessageBoxW(0, s, L"7-max", MB_ICONERROR);
}

/////////////////////////////
// IContextMenu

static BOOL MyInsertMenu(HMENU hMenu, int pos, UINT id, LPCTSTR s)
{
  MENUITEMINFO menuItem;
  menuItem.cbSize = sizeof(menuItem);
  menuItem.fType = MFT_STRING;
  menuItem.fMask = MIIM_TYPE | MIIM_ID;
  menuItem.wID = id; 
  menuItem.dwTypeData = (LPTSTR)(LPCTSTR)s;
  return ::InsertMenuItem(hMenu, pos++, TRUE, &menuItem);
}


STDMETHODIMP C7maxContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu,
      UINT commandIDFirst, UINT commandIDLast, UINT flags)
{
  if(_fileNames.Size() == 0)
    return E_FAIL;
  UINT currentCommandID = commandIDFirst; 
  if ((flags & 0x000F) != CMF_NORMAL  &&
      (flags & CMF_VERBSONLY) == 0 &&
      (flags & CMF_EXPLORE) == 0) 
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, currentCommandID); 

  CMenu popupMenu;
  popupMenu.Attach(hMenu);
  UINT subIndex = indexMenu;
  int subMenuIndex = 0;
  if(_fileNames.Size() == 1 && currentCommandID + 2 <= commandIDLast)
  {
    const UString &fileName = _fileNames.Front();

    UString folderPrefix;
    NFile::NDirectory::GetOnlyDirPrefix(fileName, folderPrefix);
   
    NFile::NFind::CFileInfoW fileInfo;
    if (!NFile::NFind::FindFile(fileName, fileInfo))
      return E_FAIL;
    if (!fileInfo.IsDirectory())
      MyInsertMenu(popupMenu, subIndex++, currentCommandID++, kCommandString); 
  }
  return MAKE_HRESULT(SEVERITY_SUCCESS, 0, currentCommandID - commandIDFirst); 
}

// extern const char *kShellFolderClassIDString;

static UString GetProgramCommand()
{
  UString path = L"\"";
  UString folder;
  if (GetProgramFolderPath(folder))
    path += folder;
  path += L"7max";
  #ifdef _WIN64
  path += L"64";
  #else
  path += L"32";
  #endif
  path += L".exe\"";
  return path;
}

static HRESULT MyCreateProcess(const UString &params)
{
  STARTUPINFO si;
  si.cb = sizeof(si);
  si.lpReserved = 0;
  si.lpDesktop = 0;
  si.lpTitle = 0;
  si.dwFlags = 0;
  si.cbReserved2 = 0;
  si.lpReserved2 = 0;
  
  PROCESS_INFORMATION pi;
  BOOL result = ::CreateProcess(NULL, (TCHAR *)(const TCHAR *)
      GetSystemString(params), 
      NULL, NULL, FALSE, 0, NULL, NULL, 
      &si, &pi);
  if (result == 0)
    return ::GetLastError();
  ::CloseHandle(pi.hThread);
  ::CloseHandle(pi.hProcess);
  return S_OK;
}

STDMETHODIMP C7maxContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO commandInfo)
{
  if(HIWORD(commandInfo->lpVerb) == 0)
  {
    int commandOffset;
    commandOffset = LOWORD(commandInfo->lpVerb);
    if (commandOffset != 0)
      return E_FAIL;
  }
  else
  {
    UString s = GetUnicodeString(commandInfo->lpVerb);
    if (s.CompareNoCase(kOpenVerb) != 0)
      return E_FAIL;
  }

  HWND aHWND = commandInfo->hwnd;

  // CWindowDisable aWindowDisable(aHWND);

  try
  {
    UString params;
    params = GetProgramCommand();
    params += L" \"";
    params += _fileNames[0];
    params += L"\"";
    if (MyCreateProcess(params) != 0)
    {
      // MessageBoxError(params);
      MessageBoxError(L"Can't start application");
    }
  }
  catch(...)
  {
    MessageBoxError(L"Error");
  }
  return S_OK;
}

static void MyCopyString(void *dest, const wchar_t *src, bool writeInUnicode)
{
  if(writeInUnicode)
  {
    wcscpy((wchar_t *)dest, src);
  }
  else
    lstrcpyA((char *)dest, GetAnsiString(src));
}

STDMETHODIMP C7maxContextMenu::GetCommandString(UINT_PTR commandOffset, UINT uType, 
    UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
  switch(uType)
  { 
    case GCS_VALIDATEA:
    case GCS_VALIDATEW:
      if(commandOffset < 0 || commandOffset >= 1)
        return S_FALSE;
      else 
        return S_OK;
  }
  if(commandOffset < 0 || commandOffset >= 1)
    return E_FAIL;
  if(uType == GCS_HELPTEXTA || uType == GCS_HELPTEXTW)
  {
    MyCopyString(pszName, kHelpString, uType == GCS_HELPTEXTW);
    return NO_ERROR;
  }
  if(uType == GCS_VERBA || uType == GCS_VERBW)
  {
    MyCopyString(pszName, kOpenVerb, uType == GCS_VERBW);
    return NO_ERROR;
  }
  return E_FAIL;
}
