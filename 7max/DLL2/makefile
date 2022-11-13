PROG = 7max.dll
DEF_FILE = dll.def
CFLAGS = $(CFLAGS) -I ../../ -DUNICODE -D_UNICODE
LIBS = $(LIBS) oleaut32.lib Advapi32.lib

MAIN_OBJS = \
  $O\7maxDLL.obj \

COMMON_OBJS = \
  $O\IntToString.obj \
  $O\Vector.obj \

WIN_OBJS = \
  $O\MemoryLock.obj \
  $O\Registry.obj \
  $O\Synchronization.obj \

7MAX_COMMON_OBJS = \
  $O\7maxRegistry.obj \
  $O\InjectDLL2.obj \
  $O\ProcessStatus.obj \

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
