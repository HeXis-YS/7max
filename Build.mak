!IFDEF CPU
LIBS = $(LIBS) bufferoverflowU.lib 
CFLAGS = $(CFLAGS) -GS- -Zc:forScope
!ENDIF

!IFNDEF O
!IFDEF CPU
O=$(CPU)
!ELSE
O=O
!ENDIF
!ENDIF

CFLAGS = $(CFLAGS) -nologo -c -Fo$O/ -EHsc -Gz -WX
!IFDEF MY_STATIC_LINK
!IFNDEF MY_SINGLE_THREAD
CFLAGS = $(CFLAGS) -MT
!ENDIF
!ELSE
CFLAGS = $(CFLAGS) -MD
!ENDIF
CFLAGS_O1 = $(CFLAGS) -O1
CFLAGS_O2 = $(CFLAGS) -O2

LFLAGS = $(LFLAGS) -nologo 
!IFDEF OLD_COMPILER
LFLAGS = $(LFLAGS) -OPT:NOWIN98
!ENDIF

!IFDEF DEF_FILE
LFLAGS = $(LFLAGS) -DLL -DEF:$(DEF_FILE)
!ENDIF

PROGPATH = $O\$(PROG)

COMPL_O1   = $(CPP) $(CFLAGS_O1) $**
COMPL_O2   = $(CPP) $(CFLAGS_O2) $**
COMPL_PCH  = $(CPP) $(CFLAGS_O1) -Yc"StdAfx.h" -Fp$O/a.pch $** 
COMPL      = $(CPP) $(CFLAGS_O1) -Yu"StdAfx.h" -Fp$O/a.pch $**

all: $(PROGPATH) 

clean:
	-del /Q $(PROGPATH) $O\*.exe $O\*.dll $O\*.obj $O\*.lib $O\*.exp $O\*.res $O\*.pch 

$O:
	if not exist "$O" mkdir "$O"

$(PROGPATH): $O $(OBJS) $(DEF_FILE)
	link $(LFLAGS) -out:$(PROGPATH) $(OBJS) $(LIBS)
$O\resource.res: $(*B).rc
	rc -fo$@ $**
$O\StdAfx.obj: $(*B).cpp
	$(COMPL_PCH)
