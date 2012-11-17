; Plugin SDK - for licensing and copyright see license.txt

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

	; Check Errors
	StrCmp $2 "" 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Failed to read Plugin Information:$\n$3"
		Goto skipplugin

	; Install
	DetailPrint "Downloading Plugin: $1"
	!insertmacro DownloadAndExecutePluginInstaller "$1" "$PLUGINDOWNLOADDIR\$2" ${dlcommand} "${dlparam}"

skipplugin:
	ClearErrors
!macroend

!macro DownloadAndExecutePluginInstaller dlsource dltarget dlcommand dlparam
	; Download
	inetc::get ${dlsource} ${dltarget} /END
	Pop $R0 ; Get the return value
	StrCmp $R0 "OK" +3
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Plugin Download failed:$\n${dlsource}$\n$R0"
		Goto +4
	${dlcommand} '${dltarget} ${dlparam}"$INSTDIR"'
	IfErrors 0 +2
		MessageBox MB_OK|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "Plugin Installation failed:$\n${dltarget}"
	ClearErrors
!macroend