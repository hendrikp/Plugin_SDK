; Plugin SDK - for licensing and copyright see license.txt

; The latest normaler installer

!include "MUI2.nsh"
!include "Sections.nsh"

; Requires those NSIS Plugins to build installer:
; Versioncheck: http://nsis.sourceforge.net/Crypto_plug-in
; Download: http://nsis.sourceforge.net/Inetc_plug-in // Used instead of NSISdl because of SSL, Redirection and better proxy support

##################################
XPStyle on

!define VERSIONCDK "3.4.5"
!define VERSION "1.3.0.0"
Name "Plugin SDK ${VERSION} for CryEngine ${VERSIONCDK}"

; The file to write
OutFile "Plugin_SDK_${VERSION}.exe"

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

;Welcome page
;!define MUI_TEXT_WELCOME_INFO_TITLE "Welcome to the Plugin SDK installer!"
;!define MUI_TEXT_WELCOME_INFO_TEXT "This is a test info text which will appear below your welcome page title! Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua."
;!define MUI_WELCOMEFINISHPAGE_BITMAP "welcome_finish.bmp"
;!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "${FILES_ROOT}\license.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!define MUI_DIRECTORYPAGE_VARIABLE $INSTDIR
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE "IsValidCEInstallation"
!insertmacro MUI_PAGE_DIRECTORY

; Install files page
!insertmacro MUI_PAGE_INSTFILES

###################################

; Sections
SectionGroup "Redistributable" SEC_BIN
    Section "Plugin Manager" SEC_PLUGINMANAGER
        SectionIn RO

        SetOutPath "$INSTDIR\Bin32\Plugins"
        File "${FILES_ROOT}\..\..\Bin32\Plugins\Plugin_Manager.dll"

        SetOutPath "$INSTDIR\Bin64\Plugins"
        File "${FILES_ROOT}\..\..\Bin64\Plugins\Plugin_Manager.dll"

		; Standard Code directory also used in git repo
        SetOutPath "$INSTDIR\Code\Plugin_SDK"
        File "${FILES_ROOT}\authors.txt"
        File "${FILES_ROOT}\license.txt"
        File "${FILES_ROOT}\readme.md"
        File "${FILES_ROOT}\changelog.md"

		Call ShowChangelog

		; Root path for Plugin Downloads and Informations
        SetOutPath "$INSTDIR\Plugin_SDK"
        File "${FILES_ROOT}\authors.txt"
        File "${FILES_ROOT}\license.txt"
        File "${FILES_ROOT}\readme.md"
        File "${FILES_ROOT}\changelog.md"

		; Root path for Plugin Downloads and Informations
		Var /GLOBAL PLUGINDOWNLOADDIR
		StrCpy $PLUGINDOWNLOADDIR "$INSTDIR\Plugin_SDK\Downloads"
		CreateDirectory $PLUGINDOWNLOADDIR
    SectionEnd

    Section "Prebuilt GameDLL" SEC_CRYGAME
        SetOutPath "$INSTDIR\Bin32"
        File "${FILES_ROOT}\..\..\Bin32\CryGame.dll"

        SetOutPath "$INSTDIR\Bin64"
        File "${FILES_ROOT}\..\..\Bin64\CryGame.dll"
    SectionEnd
SectionGroupEnd


SectionGroup "Developer Tools" SEC_DEV
    Section "VS2010 Wizard" SEC_WIZ
        SetOutPath "$DOCUMENTS\Visual Studio 2010\Wizards\PluginWizard"
        File "${FILES_ROOT}\wizard\vc10\PluginWizard.vsz"
        File "${FILES_ROOT}\wizard\vc10\PluginWizard.vsdir"
        File "${FILES_ROOT}\wizard\vc10\PluginWizard.ico"
        
		; Set Wizard install location
        Push C:\cryengine3_3.4.0 #text to be replaced
        Push $INSTDIR #replace with
        Push all #replace all occurrences
        Push all #replace all occurrences
        Push "$DOCUMENTS\Visual Studio 2010\Wizards\PluginWizard\PluginWizard.vsz" #file to replace in
        Call AdvReplaceInFile

        SetOutPath "$INSTDIR\Code\Plugin_SDK\wizard\vc10"
        File /r /x *.sdf /x *.aps /x *.suo /x *.user /x Release /x Debug /x x64 "${FILES_ROOT}\wizard\vc10\"
        
        SetOutPath "$INSTDIR\Code\Plugin_SDK\project"
        File "${FILES_ROOT}\project\Plugin_Settings.props"
        
        SetOutPath "$INSTDIR\Code\Plugin_SDK\inc"
        File /r "${FILES_ROOT}\inc\"
    SectionEnd
SectionGroupEnd

####################################

!include "DownloadHelper.nsh"

SectionGroup /e "Download Plugins" SEC_PLUGINS
	!include "Plugin_SDK_PluginList.nsh"
SectionGroupEnd

####################################

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC_BIN} "Install the required User components."
        !insertmacro MUI_DESCRIPTION_TEXT ${SEC_PLUGINMANAGER} "The Plugin Manger automatically loads all Plugins and must be integrated in the GameDLL."
        !insertmacro MUI_DESCRIPTION_TEXT ${SEC_CRYGAME} "Pre-built GameDLL with pre-integrated Plugin SDK.$\nWARNING! This will overwrite the existing GameDLL."
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEV} "Install optional Developer components."
        !insertmacro MUI_DESCRIPTION_TEXT ${SEC_WIZ} "The Wizard for Visual Studio 2010 allows fast and easy creation of new Plugin projects."
	!include "Plugin_SDK_PluginDescription.nsh"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Finish files page
;!define MUI_FINISHPAGE_NOAUTOCLOSE
;!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

; Set Fileinfos
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Plugin SDK"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (c) 2013, The authors of the Plugin SDK project"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Plugin SDK ${VERSION} for CryEngine ${VERSIONCDK}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}"
VIProductVersion "${VERSION}"

####################################

; Show changelog
Function "ShowChangelog"
	Exec 'notepad "$INSTDIR\Code\Plugin_SDK\changelog.md"'
FunctionEnd

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
	
	; If prebuilt gamedll is selected
	!insertmacro SectionFlagIsSet ${SEC_CRYGAME} ${SF_SELECTED} isSel notSel
	isSel:
		; Now check if is the version a prebuilt CryGame will work with
		Crypto::HashFile "MD5" "$INSTDIR\Bin32\CrySystem.dll"
		Pop $0

		; 3.4.0 640E910A60654A0CF91CC827640F7314
		; 3.4.3 2693B54AECDB7C63361ABB3437E628FA
		; 3.4.4 43B3E57037F6DF8CF5A4D3568420BF74 <- current
		StrCmp $0 "43B3E57037F6DF8CF5A4D3568420BF74" hashok
			MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND \
				"This version of CryEngine is not compatible with the pre-built GameDLL$\n\
				The pre-built GameDLL will be deselected you need to build it yourself."\
				/SD IDOK IDOK hashfailok
			Abort
				
			hashfailok:
				!insertmacro UnselectSection ${SEC_CRYGAME}
		hashok:
	notSel:
	cont1:
FunctionEnd

###################################

; Helper function to replace text in a file
Function AdvReplaceInFile
	Exch $0 ;file to replace in
	Exch
	Exch $1 ;number to replace after
	Exch
	Exch 2
	Exch $2 ;replace and onwards
	Exch 2
	Exch 3
	Exch $3 ;replace with
	Exch 3
	Exch 4
	Exch $4 ;to replace
	Exch 4
	Push $5 ;minus count
	Push $6 ;universal
	Push $7 ;end string
	Push $8 ;left string
	Push $9 ;right string
	Push $R0 ;file1
	Push $R1 ;file2
	Push $R2 ;read
	Push $R3 ;universal
	Push $R4 ;count (onwards)
	Push $R5 ;count (after)
	Push $R6 ;temp file name
	 
		GetTempFileName $R6
		FileOpen $R1 $0 r ;file to search in
		FileOpen $R0 $R6 w ;temp file
		StrLen $R3 $4
		StrCpy $R4 -1
		StrCpy $R5 -1
	 
	loop_read:
		ClearErrors
		FileRead $R1 $R2 ;read line
		IfErrors exit

		StrCpy $5 0
		StrCpy $7 $R2
	 
	loop_filter:
		IntOp $5 $5 - 1
		StrCpy $6 $7 $R3 $5 ;search
		StrCmp $6 "" file_write1
		StrCmp $6 $4 0 loop_filter
		
		StrCpy $8 $7 $5 ;left part
		IntOp $6 $5 + $R3
		IntCmp $6 0 is0 not0
	is0:
		StrCpy $9 ""
		Goto done
	not0:
		StrCpy $9 $7 "" $6 ;right part
	done:
		StrCpy $7 $8$3$9 ;re-join
	 
		IntOp $R4 $R4 + 1
		StrCmp $2 all loop_filter
		StrCmp $R4 $2 0 file_write2
		IntOp $R4 $R4 - 1
		 
		IntOp $R5 $R5 + 1
		StrCmp $1 all loop_filter
		StrCmp $R5 $1 0 file_write1
		IntOp $R5 $R5 - 1
		Goto file_write2
		 
	file_write1:
		FileWrite $R0 $7 ;write modified line
		Goto loop_read
	 
	file_write2:
		FileWrite $R0 $R2 ;write unmodified line
		Goto loop_read
	 
	exit:
		FileClose $R0
		FileClose $R1

		SetDetailsPrint none
		Delete $0
		Rename $R6 $0
		Delete $R6
		SetDetailsPrint both
		
	Pop $R6
	Pop $R5
	Pop $R4
	Pop $R3
	Pop $R2
	Pop $R1
	Pop $R0
	Pop $9
	Pop $8
	Pop $7
	Pop $6
	Pop $5
	Pop $0
	Pop $1
	Pop $2
	Pop $3
	Pop $4
FunctionEnd
