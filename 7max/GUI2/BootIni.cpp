// BootIni.cpp

#include "StdAfx.h"

#include "BootIni.h"
#include "Common/StringToInt.h"
#include "Common/IntToString.h"
#include "Common/StdInStream.h"
#include "Common/StdOutStream.h"

static const char *kBootLoader = "[boot loader]";
static const char *kOperatingSystems = "[operating systems]";
static const char *kTimeout = "timeout";
static const char *kDefault = "default";

static const char *kThreeGB = "3GB";
static const char *kPAE = "PAE";
static const char *kNoPAE = "NOPAE";
static const char *kMaxMem = "MAXMEM";
static const char *kBurnMemory = "BURNMEMORY";

static void SplitString(const AString &srcString, char splitChar, AStringVector &destStrings)
{
  destStrings.Clear();
  for (int pos = 0; pos < srcString.Length();)
  {
    int splitPos = srcString.Find(splitChar, pos);
    if (splitPos < 0)
    {
      AString s = srcString.Mid(pos);
      s.Trim();
      if (!s.IsEmpty())
        destStrings.Add(s);
      return;
    }
    if (splitPos != pos)
    {
      AString s = srcString.Mid(pos, splitPos - pos);
      s.Trim();
      if (!s.IsEmpty())
        destStrings.Add(s);
    }
    pos = splitPos + 1;
  }
}
  
static bool ParseEqPair(const AString &src, AString &destKey, AString &destValue)
{
  int eqPos = src.Find("=");
  if (eqPos < 0)
    return false;
  destKey = src.Left(eqPos);
  destKey.Trim();
  destValue = src.Mid(eqPos + 1);
  destValue.Trim();
  return true;
}

///////////////////////////////////////////////////

void CBootIniSystemNTSwitches::ParseFromSwitches(const AStringVector &switches)
{
  Clear();
  for (int i = 0; i < switches.Size(); i++)
  {
    const AString &sw = switches[i];
    int eqPos = sw.Find("=");
    if (eqPos < 0)
    {
      if (sw.CompareNoCase(kThreeGB) == 0)
      {
        ThreeGB = true;
        continue;
      }
      if (sw.CompareNoCase(kPAE) == 0)
      {
        PAE = true;
        continue;
      }
      if (sw.CompareNoCase(kNoPAE) == 0)
      {
        NoPAE = true;
        continue;
      }
    }
    else
    {
      AString key = sw.Left(eqPos);
      AString value = sw.Mid(eqPos + 1);
      if (key.CompareNoCase(kMaxMem) == 0 && value.Length() < 20)
      {
        const char *end;
        MaxMemSize = ConvertStringToUINT64(value, &end);
        if (MaxMemIsSpecified = (*end == '\0'))
          continue;
      }
      if (key.CompareNoCase(kBurnMemory) == 0 && value.Length() < 20)
      {
        const char *end;
        BurnMemorySize = ConvertStringToUINT64(value, &end);
        if (BurnMemoryIsSpecified = (*end == '\0'))
          continue;
      }
    }
    OtherSwitches.Add(sw);
  }
}

void CBootIniSystemNTSwitches::WriteToSwitches(AStringVector &switches) const
{
  switches.Clear();
  if (MaxMemIsSpecified)
  {
    AString sw = kMaxMem;
    sw += "=";
    char temp[32];
    ConvertUInt64ToString(MaxMemSize, temp);
    sw += temp;
    switches.Add(sw);
  }
  if (BurnMemoryIsSpecified)
  {
    AString sw = kBurnMemory;
    sw += "=";
    char temp[32];
    ConvertUInt64ToString(BurnMemorySize, temp);
    sw += temp;
    switches.Add(sw);
  }
  if (ThreeGB)
    switches.Add(kThreeGB);
  if (PAE)
    switches.Add(kPAE);
  if (NoPAE)
    switches.Add(kNoPAE);
  switches += OtherSwitches;
}

///////////////////////////////////////////////////

bool CBootIniSystem::IsNT() const
{
  if (!IsNormal || !AreNormalSwitches)
    return false;
  AString start = "multi(";
  if (start.CompareNoCase(Path.Left(start.Length())) == 0)
    return true;
  start = "scsi(";
  if (start.CompareNoCase(Path.Left(start.Length())) == 0)
    return true;
  return false;
}

void CBootIniSystem::ParseFromString(const AString &s)
{
  AString s2 = s;
  s2.Trim();
  Clear();
  AString misc;
  if (!ParseEqPair(s2, Path, misc))
  {
    Path = s2;
    return; 
  }
  if (misc.Length() < 2 || misc[0] != '\"')
  {
    Path = s2;
    return;
  }
  int quIndex = misc.Find('\"', 1);
  if (quIndex < 0)
  {
    Path = s2;
    return;
  }
  IsNormal = true;
  Title = misc.Mid(1, quIndex - 1);
  SwitchesString = misc.Mid(quIndex + 1);
  SwitchesString.Trim();
  AStringVector switchStrings;
  SplitString(SwitchesString, ' ', switchStrings);
  for (int i = 0 ; i < switchStrings.Size(); i++)
  {
    AString &sw = switchStrings[i];
    if (sw[0] != '/')
      return;
    sw.Delete(0);
  }
  Switches.ParseFromSwitches(switchStrings);
  AreNormalSwitches = true;
  return;
}

void CBootIniSystem::BuildSwitchesString(AString &s) const
{
  s.Empty();
  AStringVector switchStrings;
  Switches.WriteToSwitches(switchStrings);
  for (int i = 0; i < switchStrings.Size(); i++)
  {
    if (i > 0)
      s += " ";
    s += "/";
    s += switchStrings[i];
  }
}

void CBootIniSystem::WriteToString(AString &s) const
{
  s.Empty();
  s += Path;
  if (!IsNormal)
    return;
  s += '=';
  s += '\"';
  s += Title;
  s += '\"';
  if (!AreNormalSwitches)
  {
    if (!SwitchesString.IsEmpty())
    {
      s += " ";
      s += SwitchesString;
    }
    return;
  }
  AString s2;
  BuildSwitchesString(s2);
  if (!s2.IsEmpty())
  {
    s += " ";
    s += s2;
  }
}

///////////////////////////////////////////////////

int CBootIniInfo::FindPath(const AString &path) const
{
  for (int i = 0; i < Systems.Size(); i++)
  {
    const CBootIniSystem &system = Systems[i];
    if (!system.IsNormal)
      continue;
    if (system.Path.CompareNoCase(path) == 0)
      return i;
  }
  return -1;
}

void CBootIniInfo::SetDefault(int index)
{
  const CBootIniSystem &system = Systems[index];
  if (!system.IsNormal)
    return;
  DefultString = system.Path;
  int firstIndex = FindPath(system.Path);
  if (firstIndex < 0 || firstIndex >= index)
    return;
  Systems.Insert(firstIndex, system);
  Systems.Delete(index + 1);
}

void CBootIniInfo::Delete(int index)
{
  const CBootIniSystem &system = Systems[index];
  if (!system.IsNormal)
    return;
  bool wasDefault = (index == GetDefault());
  Systems.Delete(index);
  if (wasDefault)
    if (GetDefault() < 0)
      DefultString.Empty();
}

bool CBootIniInfo::ParseFromString(const AString &s)
{
  Clear();
  AStringVector lines;
  SplitString(s, '\n', lines);
  for (int i = 0; i < lines.Size();)
  {
    const AString &line = lines[i];
    if (line.CompareNoCase(kBootLoader) == 0)
    {
      i++;
      for (;i < lines.Size(); i++)
      {
        const AString &line = lines[i];
        AString key, value;
        if (!ParseEqPair(line, key, value))
          break;
        if (key.CompareNoCase(kTimeout) == 0)
        {
          Timeout = (int)ConvertStringToINT64(value, NULL);
          TimeoutIsSpecified = true;
          continue;
        }
        if (key.CompareNoCase(kDefault) == 0)
        {
          DefultString = value;
          continue;
        }
      }
      continue;
    }
    if (line.CompareNoCase(kOperatingSystems) == 0)
    {
      i++;
      for (;i < lines.Size(); i++)
      {
        const AString &line = lines[i];
        if (line[0] == '[')
          break;
        CBootIniSystem system;
        system.ParseFromString(line);
        Systems.Add(system);
      }
      continue;
    }
    return false;
  }
  return true;
}

void CBootIniInfo::WriteToString(AString &s) const
{
  s.Empty();
  s += kBootLoader;
  s += "\n";
  
  if (TimeoutIsSpecified)
  {
    s += kTimeout;
    s += "=";
    char temp[32];
    ConvertInt64ToString(Timeout, temp);
    s += temp;
    s += "\n";
  }

  if (!DefultString.IsEmpty())
  {
    s += kDefault;
    s += "=";
    s += DefultString;
    s += "\n";
  }

  s += kOperatingSystems;
  s += "\n";

  for (int i = 0; i < Systems.Size(); i++)
  {
    AString sysString;
    Systems[i].WriteToString(sysString);
    s += sysString;
    s += "\n";
  }
}

bool CBootIniInfo::ParseFromFile(LPCTSTR fileName)
{
  CStdInStream file;
  if (!file.Open(fileName))
    return false;
  AString s;
  file.ReadToString(s);
  return ParseFromString(s);
}

bool CBootIniInfo::WriteToFile(LPCTSTR fileName) const
{
  CStdOutStream file;
  if (!file.Open(fileName))
    return false;
  AString s;
  WriteToString(s);
  file << s;
  return true;
}
