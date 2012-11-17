; Plugin SDK - for licensing and copyright see license.txt


; Removal of new lines
; http://nsis.sourceforge.net/StrTrimNewLines

Function StrTrimNewLines
/*After this point:
  ------------------------------------------
  $R0 = String (input)
  $R1 = TrimCounter (temp)
  $R2 = Temp (temp)*/
 
  ;Get input from user
  Exch $R0
  Push $R1
  Push $R2
 
  ;Initialize trim counter
  StrCpy $R1 0
 
  loop:
  ;Subtract to get "String"'s last characters
  IntOp $R1 $R1 - 1
 
  ;Verify if they are either $\r or $\n
  StrCpy $R2 $R0 1 $R1
  ${If} $R2 == `$\r`
  ${OrIf} $R2 == `$\n`
    Goto loop
  ${EndIf}
 
  ;Trim characters (if needed)
  IntOp $R1 $R1 + 1
  ${If} $R1 < 0
    StrCpy $R0 $R0 $R1
  ${EndIf}
 
/*After this point:
  ------------------------------------------
  $R0 = ResultVar (output)*/
 
  ;Return output to user
  Pop $R2
  Pop $R1
  Exch $R0
FunctionEnd

; Macros to support downloading and executing plugin installers

!macro DownloadAndExecuteLatestPluginInstaller dlsource dlcommand dlparam
	; Pluginfo file
	StrCpy $3 "$PLUGINDOWNLOADDIR\plugindl.info"
	
	; Cleanup last tempfile
	IfFileExists $3 0 +2
	Delete $3

	; Download
	inetc::get ${dlsource} $3 /END
	Pop $R0 ; Get the return value
	StrCmp $R0 "OK" +3
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Failed to get Plugin Information:$\n${dlsource}$\n$R0"
		Goto skipplugin

	; Check if File exists
	IfFileExists $3 +3 0
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Plugin Information not present:$\n${dlsource}$\n$R0"
		Goto skipplugin

	; Read Plugin Information
	ClearErrors
	StrCpy $1 ""
	StrCpy $2 ""
	FileOpen $0 $3 r
	IfErrors +5
	FileRead $0 $1
	IfErrors +2
	FileRead $0 $2
	FileClose $0
	Delete $3 ; Cleanup temp file

	; Remove New Lines
	Push "$1"
	Call StrTrimNewLines
	Pop $1

	Push "$2"
	Call StrTrimNewLines
	Pop $2

	; Check Errors
	StrCmp $2 "" 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Failed to read Plugin Information:$\n$3"
		Goto skipplugin

	; Install
	DetailPrint "Downloading Plugin: $1"
	!insertmacro DownloadAndExecutePluginInstaller "$2" "$PLUGINDOWNLOADDIR\$1" ${dlcommand} "${dlparam}"

skipplugin:
	ClearErrors
!macroend

!macro DownloadAndExecutePluginInstaller dlsource dltarget dlcommand dlparam
	; Download
	inetc::get ${dlsource} ${dltarget} /END
	Pop $R0 ; Get the return value
	StrCmp $R0 "OK" +3
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Plugin Download failed:$\n${dlsource}$\n${dltarget}$\n$R0"
		Goto +4
	${dlcommand} '${dltarget} ${dlparam}"$INSTDIR"'
	IfErrors 0 +2
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Plugin Installation failed:$\n${dltarget}"
	ClearErrors
!macroend
