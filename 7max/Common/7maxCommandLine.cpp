// 7maxCommandLine.cpp

#include "StdAfx.h"

#include "7maxCommandLine.h"
#include "Common/StringToInt.h"

using namespace NCommandLineParser;

static const char *kUserErrorMessage  = "Incorrect command line";

static const int kNumSwitches = 7;

namespace NKey {
enum Enum
{
  kHelp1 = 0,
  kHelp2,
  kPid,
  kVmSmall,
  kVmLarge,
  kDefrag,
  kSilent,
  // kLoadLib
};

}
static const CSwitchForm kSwitchForms[kNumSwitches] = 
{
  { L"?",  NSwitchType::kSimple, false },
  { L"H",  NSwitchType::kSimple, false },
  { L"PID", NSwitchType::kUnLimitedPostString, false, 0},
  { L"VMS", NSwitchType::kUnLimitedPostString, false, 0},
  { L"VML", NSwitchType::kUnLimitedPostString, false, 0},
  { L"DEFRAG", NSwitchType::kUnLimitedPostString, false, 0},
  { L"S", NSwitchType::kSimple, false }
  // { L"LLA", NSwitchType::kSimple, false }
};

CCommandLineParser::CCommandLineParser(): parser(kNumSwitches) {}

void CCommandLineParser::SetSizeTest(int switchIndex, CSizeTest &v)
{
  if (!parser[switchIndex].ThereIs)
    return;
  v.Enabled = true;
  const UString &s = parser[switchIndex].PostStrings.Front();
  if (s.IsEmpty())
    return;
  const wchar_t *end;
  v.Size = ConvertStringToUInt64(parser[switchIndex].PostStrings.Front(), &end);
  if (*end != 0)
    throw kUserErrorMessage;
  v.SizeIsDefined = true;
}

void CCommandLineParser::Parse(CCommandLineOptions &options)
{
  UString cmdLine = ::GetCommandLineW();
  {
    UString programName;
    NCommandLineParser::SplitCommandLine(cmdLine, programName, options.Arguments);
    options.Arguments.Trim();
    if (options.IsEmpty())
      return;
  }
  UStringVector commandStrings;
  NCommandLineParser::SplitCommandLine(cmdLine, commandStrings);
  try
  {
    parser.ParseStrings(kSwitchForms, commandStrings);
  }
  catch(...) 
  {
    return;
  }

  options.HelpMode = parser[NKey::kHelp1].ThereIs || parser[NKey::kHelp2].ThereIs;
  if (parser[NKey::kSilent].ThereIs)
    options.Silent = true;
  CSizeTest id;
  SetSizeTest(NKey::kPid, id);
  if (id.Enabled)
  {
    if (id.Size > 0xFFFFFFFF)
      throw kUserErrorMessage;
    options.IsProcessIdDefined = true;
    options.ProcessId = (DWORD)id.Size;
    return;
  }
  SetSizeTest(NKey::kPid, id);
  SetSizeTest(NKey::kVmSmall, options.VmSmall);
  options.VmSmall.Size <<= 20;
  SetSizeTest(NKey::kVmLarge, options.VmLarge);
  options.VmLarge.Size <<= 20;
  SetSizeTest(NKey::kDefrag, options.Defrag);
  options.Defrag.Size <<= 20;
  // options.LoadLib = parser[NKey::kLoadLib].ThereIs;
};