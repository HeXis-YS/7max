// RegistryUtils.cpp

#include "StdAfx.h"

#include "RegistryUtils.h"
#include "Windows/Registry.h"

using namespace NWindows;
using namespace NRegistry;

static const TCHAR *kBasePath = TEXT("Software\\7max");
static const TCHAR *kCulumnsValueName = TEXT("Columns");
static const TCHAR *kCommandsHistoryValueName = TEXT("CommandsHistory");
static const TCHAR *kPositionValueName = TEXT("Position");
static const TCHAR *kAllowAttach = TEXT("AllowAttach");

#pragma pack( push, 1)

class CColumnInfoSpec
{
  UINT32 PropID;
  BYTE IsVisible;
  UINT32 Width;
public:
  void GetFromColumnInfo(const CRegColumnInfo &aSrc) 
  {
    PropID = aSrc.PropID;
    IsVisible = aSrc.IsVisible ? 1: 0;
    Width = aSrc.Width;
  }
  void PutColumnInfo(CRegColumnInfo &aDest) 
  {
    aDest.PropID = PropID;
    aDest.IsVisible = (IsVisible != 0);
    aDest.Width = Width;
  }
};

struct CColumnHeader
{
  UINT32 Version;
  UINT32 SortID;
  BYTE Ascending;
};

struct CWindowPosition
{
  RECT Rect;
  UINT32 Maximized;
};

#pragma pack(pop)

static const UINT32 kColumnInfoVersion = 0;

void SaveListViewInfo(const CListViewInfo &viewInfo)
{
  const CRecordVector<CRegColumnInfo> &columns = viewInfo.Columns;
  CByteBuffer buffer;
  UINT32 dataSize = sizeof(CColumnHeader) + sizeof(CColumnInfoSpec) * columns.Size();
  buffer.SetCapacity(dataSize);
  BYTE *dataPointer = (BYTE *)buffer;
  
  CColumnHeader &columnHeader = *(CColumnHeader *)dataPointer;
  columnHeader.Version = kColumnInfoVersion;
  columnHeader.SortID = viewInfo.SortID;
  columnHeader.Ascending = viewInfo.Ascending ? 1 : 0;

  CColumnInfoSpec *destItems = (CColumnInfoSpec *)(dataPointer + sizeof(CColumnHeader));
  for(int i = 0; i < columns.Size(); i++)
  {
    CColumnInfoSpec &columnInfoSpec = destItems[i];
    columnInfoSpec.GetFromColumnInfo(columns[i]);
  }
  {
    CSysString keyName = kBasePath;
    CKey key;
    key.Create(HKEY_CURRENT_USER, keyName);
    key.SetValue(kCulumnsValueName, dataPointer, dataSize);
  }
}

void ReadListViewInfo(CListViewInfo &viewInfo)
{
  viewInfo.Clear();
  CRecordVector<CRegColumnInfo> &columns = viewInfo.Columns;
  CByteBuffer buffer;
  UINT32 size;
  {
    CSysString keyName = kBasePath;
    CKey key;
    if(key.Open(HKEY_CURRENT_USER, keyName, KEY_READ) != ERROR_SUCCESS)
      return;
    if (key.QueryValue(kCulumnsValueName, buffer, size) != ERROR_SUCCESS)
      return;
  }
  if (size < sizeof(CColumnHeader))
    return;
  BYTE *dataPointer = (BYTE *)buffer;
  const CColumnHeader &columnHeader = *(CColumnHeader*)dataPointer;
  if (columnHeader.Version != kColumnInfoVersion)
    return;
  viewInfo.Ascending = (columnHeader.Ascending != 0);

  viewInfo.SortID = columnHeader.SortID;

  size -= sizeof(CColumnHeader);
  if (size % sizeof(CColumnHeader) != 0)
    return;
  int numItems = size / sizeof(CColumnInfoSpec);
  CColumnInfoSpec *specItems = (CColumnInfoSpec *)(dataPointer + sizeof(CColumnHeader));;
  columns.Reserve(numItems);
  for(int i = 0; i < numItems; i++)
  {
    CRegColumnInfo columnInfo;
    specItems[i].PutColumnInfo(columnInfo);
    columns.Add(columnInfo);
  }
}

/*
void SaveRegLang(const CSysString &langFile)
{
  CKey cuKey;
  cuKey.Create(HKEY_CURRENT_USER, kCUBasePath);
  cuKey.SetValue(kLangValueName, langFile);
}

void ReadRegLang(CSysString &langFile)
{
  langFile.Empty();
  CKey cuKey;
  cuKey.Create(HKEY_CURRENT_USER, kCUBasePath);
  cuKey.QueryValue(kLangValueName, langFile);
}
*/

void SaveStringList(LPCTSTR valueName, const UStringVector &strings)
{
  UINT32 sizeInChars = 0;
  int i;
  for (i = 0; i < strings.Size(); i++)
    sizeInChars += strings[i].Length() + 1;
  CBuffer<wchar_t> buffer;
  buffer.SetCapacity(sizeInChars);
  int pos = 0;
  for (i = 0; i < strings.Size(); i++)
  {
    wcscpy(buffer + pos, strings[i]);
    pos += strings[i].Length() + 1;
  }
  CKey key;
  key.Create(HKEY_CURRENT_USER, kBasePath);
  key.SetValue(valueName, buffer, sizeInChars * sizeof(wchar_t));
}

void ReadStringList(LPCTSTR valueName, UStringVector &strings)
{
  strings.Clear();
  CKey key;
  if(key.Open(HKEY_CURRENT_USER, kBasePath, KEY_READ) != ERROR_SUCCESS)
    return;
  CByteBuffer buffer;
  UINT32 dataSize;
  if (key.QueryValue(valueName, buffer, dataSize) != ERROR_SUCCESS)
    return;
  if (dataSize % sizeof(wchar_t) != 0)
    return;
  const wchar_t *data = (const wchar_t *)(const BYTE  *)buffer;
  int sizeInChars = dataSize / sizeof(wchar_t);
  UString s;
  for (int i = 0; i < sizeInChars; i++)
  {
    wchar_t c = data[i];
    if (c == L'\0')
    {
      strings.Add(s);
      s.Empty();
    }
    else
      s += c;
  }
}

void SaveCommandsHistory(const UStringVector &commands)
  { SaveStringList(kCommandsHistoryValueName, commands); }
void ReadCommandsHistory(UStringVector &commands)
  { ReadStringList(kCommandsHistoryValueName, commands); }

static void SaveOption(const TCHAR *value, bool enabled)
{
  CKey cuKey;
  cuKey.Create(HKEY_CURRENT_USER, kBasePath);
  cuKey.SetValue(value, enabled);
}

static bool ReadOption(const TCHAR *value, bool defaultValue)
{
  CKey cuKey;
  cuKey.Create(HKEY_CURRENT_USER, kBasePath);
  bool enabled;
  if (cuKey.QueryValue(value, enabled) != ERROR_SUCCESS)
    return defaultValue;
  return enabled;
}

void SaveAllowAttach(bool allow)
  { SaveOption(kAllowAttach, allow); }
bool GetRecommendedAllowAttach()
  { return false; }
bool ReadAllowAttach()
  { return ReadOption(kAllowAttach, GetRecommendedAllowAttach()); }

void SaveWindowSize(const RECT &rect, bool maximized)
{
  CKey key;
  key.Create(HKEY_CURRENT_USER, kBasePath);
  CWindowPosition position;
  position.Rect = rect;
  position.Maximized = maximized ? 1: 0;
  key.SetValue(kPositionValueName, &position, sizeof(position));
}

bool ReadWindowSize(RECT &rect, bool &maximized)
{
  CKey key;
  if(key.Open(HKEY_CURRENT_USER, kBasePath, KEY_READ) != ERROR_SUCCESS)
    return false;
  CByteBuffer buffer;
  UINT32 size;
  if (key.QueryValue(kPositionValueName, buffer, size) != ERROR_SUCCESS)
    return false;
  if (size != sizeof(CWindowPosition))
    return false;
  const CWindowPosition &position = *(const CWindowPosition *)(const BYTE *)buffer;
  rect = position.Rect;
  maximized = (position.Maximized != 0);
  return true;
}

bool ChechFirstTime()
{
  CKey key;
  return (key.Open(HKEY_CURRENT_USER, kBasePath, KEY_READ) == ERROR_FILE_NOT_FOUND);
}

void SaveNotFirstTime()
{
  CKey key;
  key.Create(HKEY_CURRENT_USER, kBasePath);
}
