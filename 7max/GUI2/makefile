PROG = 7max.exe
CFLAGS = $(CFLAGS) -I ../../ -DUNICODE -D_UNICODE
LIBS = $(LIBS) oleaut32.lib user32.lib shell32.lib comctl32.lib comdlg32.lib htmlhelp.lib Advapi32.lib

LFLAGS = $(LFLAGS) -LARGEADDRESSAWARE 

MAIN_OBJS = \
  $O\GUI.obj \
  $O\HelpUtils.obj \
  $O\MenuUtils.obj \
  $O\RegistryUtils.obj \
  $O\RunProcess.obj \

COMMON_OBJS = \
  $O\CommandLineParser.obj \
  $O\IntToString.obj \
  $O\NewHandler.obj \
  $O\String.obj \
  $O\StringConvert.obj \
  $O\StringToInt.obj \
  $O\Vector.obj \
  $O\Wildcard.obj \

WIN_OBJS = \
  $O\Error.obj \
  $O\FileDir.obj \
  $O\FileFind.obj \
  $O\FileName.obj \
  $O\MemoryLock.obj \
  $O\Registry.obj \
  $O\Synchronization.obj \
  $O\Security.obj \
  $O\Window.obj \

WIN_CTRL_OBJS = \
  $O\ComboBox.obj \
  $O\Dialog.obj \

7MAX_COMMON_OBJS = \
  $O\7maxCommandLine.obj \
  $O\7maxInstall2.obj \
  $O\7maxRegistry.obj \
  $O\ProcessStatus.obj \
  $O\ProgramLocation.obj \
  $O\InjectDLL2.obj \
  $O\InjectDLL3.obj \

OBJS = \
  $O\StdAfx.obj \
  $(MAIN_OBJS) \
  $(COMMON_OBJS) \
  $(WIN_OBJS) \
  $(WIN_CTRL_OBJS) \
  $(7MAX_COMMON_OBJS) \
  $O\AboutDialog.obj \
  $O\OptionsDialog.obj \
  $O\RunDialog.obj \
  $O\resource.res

!include "../../Build.mak"

$(MAIN_OBJS): $(*B).cpp
	$(COMPL)
$(7MAX_COMMON_OBJS): ../Common/$(*B).cpp
	$(COMPL)
$(COMMON_OBJS): ../../Common/$(*B).cpp
	$(COMPL)
$(WIN_OBJS): ../../Windows/$(*B).cpp
	$(COMPL)
$(WIN_CTRL_OBJS): ../../Windows/Control/$(*B).cpp
	$(COMPL)
$O\AboutDialog.obj: Resource/AboutDialog/$(*B).cpp
	$(COMPL)
$O\OptionsDialog.obj: Resource/OptionsDialog/$(*B).cpp
	$(COMPL)
$O\RunDialog.obj: Resource/RunDialog/$(*B).cpp
	$(COMPL)
