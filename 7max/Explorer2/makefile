PROG = 7maxs.dll
DEF_FILE = Explorer.def
LIBS = $(LIBS) user32.lib oleaut32.lib advapi32.lib htmlhelp.lib shell32.lib comctl32.lib ole32.lib comdlg32.lib
CFLAGS = $(CFLAGS) -I ../../ -DUNICODE -D_UNICODE

EXPLORER_OBJS = \
  $O\DllExports.obj \
  $O\ContextMenu.obj \

COMMON_OBJS = \
  $O\NewHandler.obj \
  $O\String.obj \
  $O\StringConvert.obj \
  $O\Vector.obj \
  $O\Wildcard.obj \

WIN_OBJS = \
  $O\FileDir.obj \
  $O\FileFind.obj \
  $O\FileName.obj \
  $O\Registry.obj \
  $O\Shell.obj \

7MAX_COMMON_OBJS = \
  $O\ProgramLocation.obj \

OBJS = \
  $O\StdAfx.obj \
  $(EXPLORER_OBJS) \
  $(COMMON_OBJS) \
  $(WIN_OBJS) \
  $(7MAX_COMMON_OBJS) \
  $O\resource.res

!include "../../Build.mak"

$(EXPLORER_OBJS): $(*B).cpp
	$(COMPL)
$(COMMON_OBJS): ../../Common/$(*B).cpp
	$(COMPL)
$(WIN_OBJS): ../../Windows/$(*B).cpp
	$(COMPL)
$(7MAX_COMMON_OBJS): ../Common/$(*B).cpp
	$(COMPL)
