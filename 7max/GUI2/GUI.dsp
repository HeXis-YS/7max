# Microsoft Developer Studio Project File - Name="GUI" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GUI - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GUI.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GUI.mak" CFG="GUI - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GUI - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GUI - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GUI - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MD /W3 /GX /O2 /I "..\..\\" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Psapi.lib htmlhelp.lib comctl32.lib /nologo /subsystem:windows /machine:I386 /out:"C:\Util\7max.exe" /opt:NOWIN98
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "GUI - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Psapi.lib htmlhelp.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"C:\Util\7max.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GUI - Win32 Release"
# Name "GUI - Win32 Debug"
# Begin Group "Resources"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\GUI.ico
# End Source File
# Begin Source File

SOURCE=.\Resource\AboutDialog\GUI.ICO
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# Begin Group "Spec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Group "Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource\OptionsDialog\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource\OptionsDialog\OptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource\OptionsDialog\resource.h
# End Source File
# Begin Source File

SOURCE=.\Resource\OptionsDialog\resource.rc
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "About"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource\AboutDialog\AboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource\AboutDialog\AboutDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource\AboutDialog\resource.h
# End Source File
# Begin Source File

SOURCE=.\Resource\AboutDialog\resource.rc
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Run"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Resource\RunDialog\resource.h
# End Source File
# Begin Source File

SOURCE=.\Resource\RunDialog\resource.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Resource\RunDialog\RunDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource\RunDialog\RunDialog.h
# End Source File
# End Group
# End Group
# Begin Group "7max Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\7maxCommandLine.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\7maxCommandLine.h
# End Source File
# Begin Source File

SOURCE=..\Common\7maxInstall2.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\7maxInstall2.h
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

SOURCE=..\Common\ProcessStatus.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ProcessStatus.h
# End Source File
# Begin Source File

SOURCE=..\Common\ProgramLocation.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ProgramLocation.h
# End Source File
# End Group
# Begin Group "Windows"

# PROP Default_Filter ""
# Begin Group "Control"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Windows\Control\ComboBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Control\ComboBox.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Control\Dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Control\Dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Control\ListView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Control\ListView.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\Windows\Device.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\FileDir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\FileDir.h
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

SOURCE=..\..\Windows\Menu.h
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
# Begin Source File

SOURCE=..\..\Windows\Thread.h
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Window.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Windows\Window.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\Common\CommandLineParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\CommandLineParser.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\IntToString.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\IntToString.h
# End Source File
# Begin Source File

SOURCE=..\..\Common\NewHandler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Common\NewHandler.h
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

SOURCE=.\7max.exe.manifest
# End Source File
# Begin Source File

SOURCE=.\GUI.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpUtils.h
# End Source File
# Begin Source File

SOURCE=.\MenuUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\RegistryUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\RegistryUtils.h
# End Source File
# Begin Source File

SOURCE=.\RunProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\RunProcess.h
# End Source File
# End Target
# End Project
