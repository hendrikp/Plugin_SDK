; Plugin SDK - for licensing and copyright see license.txt

; Just downloads the latest normaler installer

!include "MUI2.nsh"
!include "Sections.nsh"

; Requires those NSIS Plugins to build installer:
; Download: http://nsis.sourceforge.net/Inetc_plug-in // Used instead of NSISdl because of SSL, Redirection and better proxy support

##################################
XPStyle on

!define VERSION "1.0.0.0"
Name "Plugin SDK Webinstaller"

; The file to write
OutFile "Plugin_SDK_Webinstaller.exe"

; Default Installdir
InstallDir "C:\CryENGINE_PC\"

; Request application privileges for Windows Vista/7
RequestExecutionLevel user

!define FILES_ROOT ".."

; MUI Settings
!define MUI_ABORTWARNING

!define MUI_ICON "${FILES_ROOT}\images\logos\PluginWizard.ico"
!define MUI_UNICON "${FILES_ROOT}\images\logos\PluginWizard.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${FILES_ROOT}\images\logos\PluginSDK_Logo_Installer.bmp"
!define MUI_HEADERIMAGE_BITMAP_NOSTRETCH

###################################

; Directory page
!define MUI_DIRECTORYPAGE_VARIABLE $INSTDIR
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "IsValidCEInstallation"
!insertmacro MUI_PAGE_DIRECTORY

; Install files page
!insertmacro MUI_PAGE_INSTFILES

###################################

!include "DownloadHelper.nsh"

; Sections
SectionGroup "Redistributable" SEC_BIN
    Section "Plugin Manager" SEC_PLUGINMANAGER
        SectionIn RO
		AddSize 6500

		; Root path for Plugin Downloads and Informations
		Var /GLOBAL PLUGINDOWNLOADDIR
		StrCpy $PLUGINDOWNLOADDIR "$INSTDIR\Plugin_SDK\Downloads"
		CreateDirectory $PLUGINDOWNLOADDIR

		; Automatically close
		SetAutoClose true
		;AutoCloseWindow true

		; Start Webinstaller
		!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_SDK/master/latestredist.info" Exec "/D="
    SectionEnd
SectionGroupEnd

####################################

!insertmacro MUI_LANGUAGE "English"

; Set Fileinfos
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Plugin SDK Webinstaller"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (c) 2012, The authors of the Plugin SDK project"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Plugin SDK Webinstaller"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}"
VIProductVersion "${VERSION}"

####################################

; Check Installation prequisits
Function "IsValidCEInstallation"
	IfFileExists "$INSTDIR\Bin32\CrySystem.dll" cont
		MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND \
			"The path is not a CryEngine installation:$\n\
			$INSTDIR $\n$\n\
			The Plugin SDK, Plugins and the Wizard will NOT work,$\ncontinue anyways?" \
			IDOK cont1
		Abort
        cont:
	cont1:
FunctionEnd
