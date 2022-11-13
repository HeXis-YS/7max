// BootIni.h
 
#ifndef __BOOTINI_H
#define __BOOTINI_H

#include "Common/Vector.h"
#include "Common/String.h"

class CBootIniSystemNTSwitches
{
public:
  bool ThreeGB;
  bool PAE;
  bool NoPAE;
  bool MaxMemIsSpecified;
  UINT64 MaxMemSize;
  bool BurnMemoryIsSpecified;
  UINT64 BurnMemorySize;
  AStringVector OtherSwitches;

  CBootIniSystemNTSwitches() { Clear(); }
  void Clear()
  { 
    ThreeGB = PAE = NoPAE = MaxMemIsSpecified = BurnMemoryIsSpecified = false;
    OtherSwitches.Clear();
  }

  void ParseFromSwitches(const AStringVector &switches);
  void WriteToSwitches(AStringVector &switches) const;
};

class CBootIniSystem
{
public:
  AString Path;
  bool IsNormal;
  AString Title;
  bool AreNormalSwitches;
  AString SwitchesString;
  CBootIniSystemNTSwitches Switches;
  bool IsNT() const;
  void Clear()
  { 
    AreNormalSwitches = false;
    IsNormal = false;
    Path.Empty();
    Title.Empty();
    SwitchesString.Empty();
    Switches.Clear();
  }
  void ParseFromString(const AString &s);
  void WriteToString(AString &s) const;
  void BuildSwitchesString(AString &s) const;
  void BuildSwitchesString() { BuildSwitchesString(SwitchesString); }
};

class CBootIniInfo
{
  void Clear()
  {
    Systems.Clear();
    TimeoutIsSpecified = false;
    Timeout = 0;
    DefultString.Empty();
  }
  bool ParseFromString(const AString &s);
  void WriteToString(AString &s) const;
  AString DefultString;
  int FindPath(const AString &path) const;
public:
  CObjectVector<CBootIniSystem> Systems;
  bool TimeoutIsSpecified; 
  int Timeout;

  void SetDefault(int index);
  int GetDefault() const { return FindPath(DefultString); };
  void Delete(int index);

  bool ParseFromFile(LPCTSTR fileName);
  bool WriteToFile(LPCTSTR fileName) const;
};


#endif
