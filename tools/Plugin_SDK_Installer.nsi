!include "MUI2.nsh"

XPStyle on

Name "Plugin SDK for CryEngine"

; The file to write
OutFile "Plugin_SDK.exe"

InstallDir "C:\cryengine3\"

; Request application privileges for Windows Vista/7
RequestExecutionLevel user

!define FILES_ROOT ".."

; MUI Settings
!define MUI_ABORTWARNING

!define MUI_ICON "..\images\logos\PluginWizard.ico"
!define MUI_UNICON "..\images\logos\PluginWizard.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "..\images\logos\PluginSDK_Logo_Installer.bmp"
!define MUI_HEADERIMAGE_BITMAP_NOSTRETCH

;Welcome page
;!define MUI_TEXT_WELCOME_INFO_TITLE     "Welcome to the Plugin SDK installer!"
;!define MUI_TEXT_WELCOME_INFO_TEXT      "This is a test info text which will appear below your welcome page title! Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua."
;!define MUI_WELCOMEFINISHPAGE_BITMAP    "welcome_finish.bmp"
;!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE					"..\license.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!define MUI_DIRECTORYPAGE_VARIABLE				$INSTDIR
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE           "IsValidCEInstallation"
!insertmacro MUI_PAGE_DIRECTORY

; Install files page
!insertmacro MUI_PAGE_INSTFILES

; Sections
SectionGroup "Redistributable" SEC_BIN

    Section "Plugin Manager" SEC_PLUGINMANAGER
    
        SectionIn RO
        
        SetOutPath $INSTDIR\Bin32\Plugins
        File "${FILES_ROOT}\..\..\Bin32\Plugins\Plugin_Manager.dll"

        SetOutPath $INSTDIR\Bin64\Plugins
        File "${FILES_ROOT}\..\..\Bin64\Plugins\Plugin_Manager.dll"
        
        CreateDirectory $INSTDIR\Bin32\Plugins
        CreateDirectory $INSTDIR\Bin64\Plugins
    SectionEnd

    Section "Pre-compiled GameDLL" SEC_CRYGAME
    
        SetOutPath $INSTDIR\Bin32
        File "${FILES_ROOT}\..\..\Bin32\CryGame.dll"

        SetOutPath $INSTDIR\Bin64
        File "${FILES_ROOT}\..\..\Bin64\CryGame.dll"
    SectionEnd
  
SectionGroupEnd

SectionGroup  "Developer Tools" SEC_DEV

    Section "Plugin Wizard VS2010" SEC_WIZ
        SetOutPath "$DOCUMENTS\Visual Studio 2010\Wizards\PluginWizard"
        File "${FILES_ROOT}\wizard\vc10\PluginWizard.vsz"
        File "${FILES_ROOT}\wizard\vc10\PluginWizard.vsdir"
        File "${FILES_ROOT}\wizard\vc10\PluginWizard.ico"
        
        Push C:\cryengine3_3.4.0 #text to be replaced
        Push $INSTDIR #replace with
        Push all #replace all occurrences
        Push all #replace all occurrences
        Push "$DOCUMENTS\Visual Studio 2010\Wizards\PluginWizard\PluginWizard.vsz" #file to replace in
	Call AdvReplaceInFile

        SetOutPath $INSTDIR\Code\Plugin_SDK\wizard\vc10
        File /r /x *.sdf "${FILES_ROOT}\wizard\vc10\"
        
        SetOutPath $INSTDIR\Code\Plugin_SDK\project
        File "${FILES_ROOT}\project\Plugin_Settings.props"
        
        SetOutPath $INSTDIR\Code\Plugin_SDK\inc
        File /r "${FILES_ROOT}\inc\"
        
        SetOutPath $INSTDIR\Code\Plugin_SDK
        File "${FILES_ROOT}\authors.txt"
        File "${FILES_ROOT}\license.txt"
        File "${FILES_ROOT}\readme.md"
        File "${FILES_ROOT}\changelog.md"
    SectionEnd
    
SectionGroupEnd

; Custom functions
Function "IsValidCEInstallation"
    IfFileExists "$INSTDIR\Bin32\Launcher.exe" cont
            MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND \
                "The path is not a CryEngine installation:$\n\
		$INSTDIR $\n$\n\
                The Plugin SDK, Plugins and the Wizard will NOT work,$\ncontinue anyways?" \
                IDOK cont1
                Abort
            cont1:
        cont:
FunctionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC_BIN} "Install the required User components."
        !insertmacro MUI_DESCRIPTION_TEXT ${SEC_PLUGINMANAGER} "The Plugin Manger automatically loads all Plugins and must be integrated in the GameDLL."
        !insertmacro MUI_DESCRIPTION_TEXT ${SEC_CRYGAME} "Pre-compiled GameDLL with pre-integrated Plugin SDK.$\nWARNING! This will overwrite the existing GameDLL."
	!insertmacro MUI_DESCRIPTION_TEXT ${SEC_DEV} "Install optional Developer components."
        !insertmacro MUI_DESCRIPTION_TEXT ${SEC_WIZ} "The Wizard for Visual Studio 2010 allows fast and easy creation of new Plugin projects."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

; Finish files page
;!define MUI_FINISHPAGE_NOAUTOCLOSE
;!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_LANGUAGE   "English"

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