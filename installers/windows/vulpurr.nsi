Unicode True
SetCompressor /SOLID lzma
RequestExecutionLevel user

!ifndef VERSION
  !define VERSION "0.18.5.3"
!endif
!ifndef BUILD
  !define BUILD "15"
!endif
!ifndef SRC_DIR
  !error "SRC_DIR must be defined"
!endif
!ifndef OUT_FILE
  !define OUT_FILE "Vulpurr-Setup-win64.exe"
!endif
!ifndef ROOT_DIR
  !define ROOT_DIR "../.."
!endif

Name "Vulpurr"
OutFile "${OUT_FILE}"
InstallDir "$LOCALAPPDATA\Vulpurr"
InstallDirRegKey HKCU "Software\Vulpurr\Installer" "InstallDir"

VIProductVersion "${VERSION}.${BUILD}"
VIAddVersionKey "ProductName" "Vulpurr Wallet"
VIAddVersionKey "CompanyName" "Vulpurr Network"
VIAddVersionKey "FileDescription" "Vulpurr Wallet Installer"
VIAddVersionKey "FileVersion" "${VERSION}.${BUILD}"
VIAddVersionKey "ProductVersion" "${VERSION}.${BUILD}"
VIAddVersionKey "LegalCopyright" "Copyright (c) 2026 Vulpurr Network"

!include "MUI2.nsh"
!include "x64.nsh"
!include "FileFunc.nsh"

!define MUI_ABORTWARNING
!define MUI_ICON "${ROOT_DIR}/images/appicon.ico"
!define MUI_UNICON "${ROOT_DIR}/images/appicon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${ROOT_DIR}/installers/windows/vulpurr-welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${ROOT_DIR}/installers/windows/vulpurr-welcome.bmp"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${ROOT_DIR}/installers/windows/vulpurr-header.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define MUI_FINISHPAGE_RUN "$INSTDIR\vulpurr-wallet.exe"
!define MUI_FINISHPAGE_RUN_TEXT "Launch Vulpurr Wallet"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "${ROOT_DIR}/installers/windows/LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"

Section "Vulpurr Wallet" SecMain
  SectionIn RO
  SetShellVarContext current

  SetOutPath "$INSTDIR"
  File "${SRC_DIR}/vulpurr-wallet.exe"
  File "${SRC_DIR}/vulpurrd.exe"

  SetOutPath "$INSTDIR\p2pool"
  File "${SRC_DIR}/p2pool/vulpurr-p2pool.exe"

  SetOutPath "$INSTDIR"
  WriteUninstaller "$INSTDIR\uninstall.exe"

  CreateDirectory "$SMPROGRAMS\Vulpurr"
  CreateShortCut "$SMPROGRAMS\Vulpurr\Vulpurr.lnk" "$INSTDIR\vulpurr-wallet.exe" "" "$INSTDIR\vulpurr-wallet.exe" 0
  CreateShortCut "$SMPROGRAMS\Vulpurr\Uninstall Vulpurr.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$DESKTOP\Vulpurr.lnk" "$INSTDIR\vulpurr-wallet.exe" "" "$INSTDIR\vulpurr-wallet.exe" 0

  WriteRegStr HKCU "Software\Vulpurr\Installer" "InstallDir" "$INSTDIR"
  WriteRegStr HKCU "Software\Vulpurr\Installer" "Build" "${BUILD}"

  WriteRegStr HKCU "Software\Classes\vulpurr" "" "URL:Vulpurr Protocol"
  WriteRegStr HKCU "Software\Classes\vulpurr" "URL Protocol" ""
  WriteRegStr HKCU "Software\Classes\vulpurr\DefaultIcon" "" "$INSTDIR\vulpurr-wallet.exe,0"
  WriteRegStr HKCU "Software\Classes\vulpurr\shell\open\command" "" '"$INSTDIR\vulpurr-wallet.exe" "%1"'

  WriteRegStr HKCU "Software\Classes\vulpurrseed" "" "URL:Vulpurr Seed Protocol"
  WriteRegStr HKCU "Software\Classes\vulpurrseed" "URL Protocol" ""
  WriteRegStr HKCU "Software\Classes\vulpurrseed\DefaultIcon" "" "$INSTDIR\vulpurr-wallet.exe,0"
  WriteRegStr HKCU "Software\Classes\vulpurrseed\shell\open\command" "" '"$INSTDIR\vulpurr-wallet.exe" "%1"'

  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "DisplayName" "Vulpurr Wallet"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "DisplayVersion" "${VERSION}.${BUILD}"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "Publisher" "Vulpurr Network"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "DisplayIcon" "$INSTDIR\vulpurr-wallet.exe,0"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "InstallLocation" "$INSTDIR"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "NoModify" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "NoRepair" 1
  WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr" "EstimatedSize" "$0"
SectionEnd

Section "Uninstall"
  SetShellVarContext current

  Delete "$INSTDIR\vulpurr-wallet.exe"
  Delete "$INSTDIR\vulpurrd.exe"
  Delete "$INSTDIR\monerod.exe"
  Delete "$INSTDIR\p2pool\vulpurr-p2pool.exe"
  RMDir "$INSTDIR\p2pool"
  Delete "$INSTDIR\uninstall.exe"
  RMDir "$INSTDIR"

  Delete "$DESKTOP\Vulpurr.lnk"
  Delete "$SMPROGRAMS\Vulpurr\Vulpurr.lnk"
  Delete "$SMPROGRAMS\Vulpurr\Uninstall Vulpurr.lnk"
  RMDir "$SMPROGRAMS\Vulpurr"

  DeleteRegKey HKCU "Software\Classes\vulpurr"
  DeleteRegKey HKCU "Software\Classes\vulpurrseed"
  DeleteRegKey HKCU "Software\Vulpurr\Installer"
  DeleteRegKey /ifempty HKCU "Software\Vulpurr"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vulpurr"
SectionEnd
