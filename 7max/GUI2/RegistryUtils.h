// RegistryUtils.h

#include "Common/StringConvert.h"

#ifndef __REGISTRYUTILS_H
#define __REGISTRYUTILS_H

#include "Common/Vector.h"
#include "Common/String.h"

struct CRegColumnInfo
{
  PROPID PropID;
  bool IsVisible;
  UINT32 Width;
};
inline bool operator==(const CRegColumnInfo &a1, const CRegColumnInfo &a2)
{ 
  return (a1.PropID == a2.PropID) && 
      (a1.IsVisible == a2.IsVisible) && (a1.Width == a2.Width); 
}
inline bool operator!=(const CRegColumnInfo &a1, const CRegColumnInfo &a2)
  { return !(a1 == a2); }

struct CListViewInfo
{
  CRecordVector<CRegColumnInfo> Columns;
  PROPID SortID;
  bool Ascending;

  void Clear()
  {
    SortID = 0;
    Ascending = true;
    Columns.Clear();
  }

  int FindColumnWithID(PROPID propID) const
  {
    for (int i = 0; i < Columns.Size(); i++)
      if (Columns[i].PropID == propID)
        return i;
    return -1;
  }

  bool IsEqual(const CListViewInfo &newInfo) const 
  {
    if (Columns.Size() != newInfo.Columns.Size() ||
        SortID != newInfo.SortID ||  
        Ascending != newInfo.Ascending)
      return false;
    for (int i = 0; i < Columns.Size(); i++)
      if (Columns[i] != newInfo.Columns[i])
        return false;
    return true;
  }
};

void SaveListViewInfo(const CListViewInfo &viewInfo);
void ReadListViewInfo(CListViewInfo &viewInfo);

void SaveCommandsHistory(const UStringVector &commands);
void ReadCommandsHistory(UStringVector &commands);

void SaveAllowAttach(bool allow);
bool GetRecommendedAllowAttach();
bool ReadAllowAttach();

void SaveWindowSize(const RECT &rect, bool maximized);
bool ReadWindowSize(RECT &rect, bool &maximized);

bool ChechFirstTime();
void SaveNotFirstTime();

#endif
