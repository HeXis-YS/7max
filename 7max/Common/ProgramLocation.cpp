// ProgramLocation.h

#include "StdAfx.h"

#include "ProgramLocation.h"

#include "Windows/Registry.h"
#include "Windows/FileName.h"
#include "Windows/FileFind.h"

using namespace NWindows;

extern HINSTANCE g_hInstance;

// static LPCTSTR kTestFile = TEXT("7-max.sys");
static LPCTSTR kRegistryPath = TEXT("Software\\7max");
static LPCTSTR kProgramPathValue = TEXT("Path");

bool GetProgramFolderPath(CSysString &folder)
{
  folder.Empty();
  TCHAR fullPath[MAX_PATH + 1];
  ::GetModuleFileName(g_hInstance, fullPath, MAX_PATH);
  CSysString path = fullPath;
  int pos = path.ReverseFind(TEXT('\\'));
  if (pos < 0)
    return false;
  folder = path.Left(pos + 1);
  return true;
}

static bool CheckFile(LPCTSTR path)
{
  NFile::NFind::CFileInfo fileInfo;
  if (!NFile::NFind::FindFile(path, fileInfo))
    return false;
  return (!fileInfo.IsDirectory());
}

static bool CheckFileUsingRegistry(bool currentUser, LPCTSTR path, 
    CSysString &fullPath)
{
  NRegistry::CKey key;
  if(key.Open(currentUser ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, 
        kRegistryPath, KEY_READ) == ERROR_SUCCESS)
    if (key.QueryValue(kProgramPathValue, fullPath) == ERROR_SUCCESS)
    {
      NFile::NName::NormalizeDirPathPrefix(fullPath);
      fullPath += path;
      if (CheckFile(fullPath))
        return true;
    }
  return false;
}


bool GetSevenMaxFilePath(LPCTSTR name, CSysString &fullPath)
{
  CSysString moduleFolderPrefix;
  GetProgramFolderPath(moduleFolderPrefix);
  NFile::NFind::CFileInfo fileInfo;
  fullPath = moduleFolderPrefix + name;
  if (CheckFile(fullPath))
    return true;
  if (CheckFileUsingRegistry(true, name, fullPath))
    return true;
  return CheckFileUsingRegistry(false, name, fullPath);
}

/*
static LPCTSTR kLMBasePath = _T("Software\\7-max");
static LPCTSTR kAppPathPathKeyValueName2 = _T("Path");

bool GetProgramFolderPath(CSysString &folder)
{
  folder.Empty();
  CKey key;
  if (key.Open(HKEY_LOCAL_MACHINE, kLMBasePath, KEY_READ) != ERROR_SUCCESS)
    return false;
  if (key.QueryValue(kAppPathPathKeyValueName2, folder) != ERROR_SUCCESS)
    return false;
  NFile::NName::NormalizeDirPathPrefix(folder);
  return true;
}
*/

/*
bool GetProgramDirPrefix(CSysString &folder)
{
  folder.Empty();
  CKey aKey;
  CSysString aKeyPath = REGSTR_PATH_APPPATHS;
  aKeyPath += kKeyNameDelimiter;
  aKeyPath += kAppPathProgramName;
  if (aKey.Open(HKEY_LOCAL_MACHINE, aKeyPath, KEY_READ) != ERROR_SUCCESS)
    return false;
  if (aKey.QueryValue(kAppPathPathKeyValueName, folder) != ERROR_SUCCESS)
    return false;
  NFile::NName::NormalizeDirPathPrefix(folder);
  return true;
}
*/
