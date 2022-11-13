// 7maxCommandLine.h

#ifndef __7MAXCOMMANDLINE_H
#define __7MAXCOMMANDLINE_H

#include "Common/CommandLineParser.h"
#include "Common/Types.h"

struct CSizeTest
{
  bool Enabled;
  bool SizeIsDefined;
  UInt64 Size;
  CSizeTest(): Enabled(false), SizeIsDefined(false) {}
};

struct CCommandLineOptions
{
  bool HelpMode;
  CSizeTest VmSmall;
  CSizeTest VmLarge;
  CSizeTest Defrag;
  bool IsProcessIdDefined;
  // bool LoadLib;
  DWORD ProcessId;
  UString Arguments;
  bool Silent;
  bool IsEmpty() const { return Arguments.IsEmpty(); }
  CCommandLineOptions(): IsProcessIdDefined(false), HelpMode(false), Silent(false)
      // , LoadLib(false) 
    {}
};

class CCommandLineParser
{
  NCommandLineParser::CParser parser;
  void SetSizeTest(int switchIndex, CSizeTest &v);
public:
  CCommandLineParser();
  void Parse(CCommandLineOptions &options);
};

#endif
