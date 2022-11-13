# Microsoft Developer Studio Project File - Name="Console" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Console - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Console.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Console.mak" CFG="Console - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Console - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Console - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Console - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O2 /I "../../" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /FAcs /Yu"StdAfx.h" /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"C:\Util\7maxc32.exe" /opt:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Console - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /W3 /Gm /GX /ZI /Od /I "../../" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /Yu"StdAfx.h" /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"C:\Util\7maxc32.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Console - Win32 Release"
# Name "Console - Win32 Debug"
# Begin Group "7max Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\7maxCommandLine.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\7maxCommandLine.h
# End Source File
# Begin Source File

SOURCE=..\Common\7maxRegistry.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\7maxRegistry.h
# End Source File
# Begin Source File

SOURCE=..\Common\InjectDLL2.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\InjectDLL2.h
# End Source File
# Begin Source File

SOURCE=..\Common\InjectDLL3.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\InjectDLL3.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\NewHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\NewHandler.h
# End Source File
# Begin Source File

SOURCE=..\Common\ProcessStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ProcessStatus.h
# End Source File
# End Group
# Begin Group "Spec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"StdAfx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Windows\Error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Error.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\FileFind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\FileFind.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\FileName.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\FileName.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Handle.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\MemoryLock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\MemoryLock.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Process.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Registry.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Registry.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Security.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Security.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Synchronization.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Synchronization.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Common\Buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\CommandLineParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\CommandLineParser.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\DynamicBuffer.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\IntToString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\IntToString.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\String.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\String.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\StringConvert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\StringConvert.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\StringToInt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\StringToInt.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Vector.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Vector.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\Wildcard.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\Wildcard.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Console.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleClose.cpp
# End Source File
# Begin Source File

SOURCE=.\ConsoleClose.h
# End Source File
# End Target
# End Project
