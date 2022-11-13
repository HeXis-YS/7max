;--------------------------------
;Defines

!define VERSION_MAJOR 5
!define VERSION_MINOR 00
!define VERSION_POSTFIX_FULL " beta"
!define VERSION_SYS_POSTFIX_FULL ""
!define NAME_FULL "7-max ${VERSION_MAJOR}.${VERSION_MINOR}${VERSION_POSTFIX_FULL}${VERSION_SYS_POSTFIX_FULL}"
!define VERSION_POSTFIX ""
!define VERSION_SYS_POSTFIX ""


!define FM_LINK "7-max.lnk"
!define HELP_LINK "7-max Help.lnk"

!define CLSID_CONTEXT_MENU {23170F69-40C2-278A-1000-000100020000}

#!define NO_COMPRESSION

  !include "Library.nsh"
  !include "MUI.nsh"

;--------------------------------
;Configuration

  ;General
  Name "${NAME_FULL}"
  BrandingText "www.7-max.com"
  OutFile "..\7max${VERSION_MAJOR}${VERSION_MINOR}${VERSION_POSTFIX}${VERSION_SYS_POSTFIX}.exe"

  ;Folder selection page
  InstallDir "$PROGRAMFILES\7-max"
  
  ;Get install folder from registry if available
  InstallDirRegKey HKLM "Software\7max" "Path"

  ;Compressor
!ifndef NO_COMPRESSION
  SetCompressor /SOLID lzma
  SetCompressorDictSize 1
!else
  SetCompressor zlib
  SetCompress off
!endif


;--------------------------------
;Variables

  Var "MyDllPath"
  Var "MyExePath"

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  #!insertmacro MUI_PAGE_LICENSE "License.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Reserve Files
  
  ;These files should be inserted before other files in the data block
  ;Keep these lines before any File command
  ;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Installer Sections

Section
  # delete old unwanted files
  Delete $INSTDIR\7max.sys
  Delete $INSTDIR\7max.dll
  Delete $INSTDIR\7maxs.dll
  Delete $INSTDIR\7max.exe
  Delete $INSTDIR\7maxc.exe

  # install files
  SetOutPath "$INSTDIR"

  File History.txt
  File License.txt
  File copying.txt
  File readme.txt

  File 7max32.dll
  File 7max64.dll
  File 7max32.exe
  File 7max64.exe
  File 7maxc32.exe
  File 7maxc64.exe
  # File 7maxs32.dll
  # File 7maxs64.dll
  File 7max.chm

  System::Call "kernel32::GetCurrentProcess() i .s"
  System::Call "kernel32::IsWow64Process(i s, *i .r0)"


  IntCmpU $0 0 reg32
    !insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED 7maxs64.dll $INSTDIR\7maxs64.dll $INSTDIR
    File 7maxs32.dll 
    StrCpy "$MyDllPath" "7maxs64.dll"
    StrCpy "$MyExePath" "7max64.exe"
    Goto doneReg
  reg32:
    !insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED 7maxs32.dll $INSTDIR\7maxs32.dll $INSTDIR
    File 7maxs64.dll 
    StrCpy "$MyDllPath" "7maxs32.dll"
    StrCpy "$MyExePath" "7max32.exe"
  doneReg:



  # delete "current user" menu items

  Delete "$SMPROGRAMS\7-max\${FM_LINK}"
  Delete "$SMPROGRAMS\7-max\${HELP_LINK}"
  RMDir  "$SMPROGRAMS\7-max"

  # set "all users" mode

  SetShellVarContext all


  ClearErrors

  # create start menu icons
  SetOutPath $INSTDIR # working directory
  CreateDirectory $SMPROGRAMS\7-max

  CreateShortcut "$SMPROGRAMS\7-max\${FM_LINK}" $INSTDIR\$MyExePath
  CreateShortcut "$SMPROGRAMS\7-max\${HELP_LINK}" $INSTDIR\7max.chm

  IfErrors 0 noScErrors

  SetShellVarContext current

  CreateDirectory $SMPROGRAMS\7-max
  CreateShortcut "$SMPROGRAMS\7-max\${FM_LINK}" $INSTDIR\$MyExePath
  CreateShortcut "$SMPROGRAMS\7-max\${HELP_LINK}" $INSTDIR\7max.chm

  noScErrors:

  # store install folder
  WriteRegStr HKLM "Software\7max" "Path" $INSTDIR
  WriteRegStr HKCU "Software\7max" "Path" $INSTDIR

  # write reg entries
  WriteRegStr HKCR "CLSID\${CLSID_CONTEXT_MENU}" "" "7-max Shell Extension"
  WriteRegStr HKCR "CLSID\${CLSID_CONTEXT_MENU}\InprocServer32" "" "$INSTDIR\$MyDllPath"
  WriteRegStr HKCR "CLSID\${CLSID_CONTEXT_MENU}\InprocServer32" "ThreadingModel" "Apartment"


  WriteRegStr HKCR "*\shellex\ContextMenuHandlers\7-max" "" "${CLSID_CONTEXT_MENU}"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved" "${CLSID_CONTEXT_MENU}" "7-max Shell Extension"
  
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\7max64.exe" "" "$INSTDIR\7max64.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\7max32.exe" "" "$INSTDIR\7max32.exe"

  # create uninstaller
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\7-max" "DisplayName" "${NAME_FULL}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\7-max" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\7-max" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\7-max" "NoRepair" 1
  WriteUninstaller $INSTDIR\Uninstall.exe

  !ifdef WIN64
  ExecWait 'regsvr32 /s "$INSTDIR\$MyDllPath"'
  !endif

SectionEnd
 

;--------------------------------
;Installer Functions

/*
Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd
*/

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  !ifdef WIN64
  ExecWait 'regsvr32 /u /s "$INSTDIR\7maxs32.dll"'
  ExecWait 'regsvr32 /u /s "$INSTDIR\7maxs64.dll"'
  !endif

  # delete files

  Delete $INSTDIR\History.txt
  Delete $INSTDIR\License.txt
  Delete $INSTDIR\copying.txt
  Delete $INSTDIR\readme.txt
  Delete $INSTDIR\7max.chm

  Delete /REBOOTOK $INSTDIR\7maxs32.dll
  Delete /REBOOTOK $INSTDIR\7maxs64.dll

  Delete $INSTDIR\7max32.dll
  Delete $INSTDIR\7max64.dll
  Delete $INSTDIR\7max32.exe
  Delete $INSTDIR\7max64.exe
  Delete $INSTDIR\7maxc32.exe
  Delete $INSTDIR\7maxc64.exe

  Delete $INSTDIR\Uninstall.exe

  RMDir $INSTDIR

  # delete start menu entires

  SetShellVarContext all

  # ClearErrors

  Delete "$SMPROGRAMS\7-max\${FM_LINK}"
  Delete "$SMPROGRAMS\7-max\${HELP_LINK}"
  RMDir  "$SMPROGRAMS\7-max"

  # IfErrors 0 noScErrors

  SetShellVarContext current

  Delete "$SMPROGRAMS\7-max\${FM_LINK}"
  Delete "$SMPROGRAMS\7-max\${HELP_LINK}"
  RMDir  "$SMPROGRAMS\7-max"

  # noScErrors:


  # delete registry entries

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\7-max"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\7max64.exe"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\7max32.exe"

  DeleteRegKey HKLM "Software\7-max"
  DeleteRegKey HKCU "Software\7-max"

  DeleteRegKey HKCR "CLSID\${CLSID_CONTEXT_MENU}"

  DeleteRegKey HKCR "*\shellex\ContextMenuHandlers\7-max"


  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved" "${CLSID_CONTEXT_MENU}"


SectionEnd
