PROG = 7maxc.exe
CFLAGS = $(CFLAGS) -I ../../ -DUNICODE -D_UNICODE
LIBS = $(LIBS) oleaut32.lib user32.lib Advapi32.lib
LFLAGS = $(LFLAGS) -LARGEADDRESSAWARE 

MAIN_OBJS = \
  $O\Console.obj \
  $O\ConsoleClose.obj \

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
  $O\FileFind.obj \
  $O\FileName.obj \
  $O\MemoryLock.obj \
  $O\Registry.obj \
  $O\Synchronization.obj \
  $O\Security.obj \

7MAX_COMMON_OBJS = \
  $O\7maxCommandLine.obj \
  $O\7maxInstall2.obj \
  $O\7maxRegistry.obj \
  $O\ProcessStatus.obj \
  $O\InjectDLL2.obj \
  $O\InjectDLL3.obj \

OBJS = \
  $O\StdAfx.obj \
  $(MAIN_OBJS) \
  $(COMMON_OBJS) \
  $(WIN_OBJS) \
  $(7MAX_COMMON_OBJS) \
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
