; Plugin SDK - for licensing and copyright see license.txt

; Contains the plugins that can be downloaded by the Plugin SDK installer
; The files need to be executable nsis installers
;  msi or other installers is also possible but may require custom parameters

; .info file description
; First line: Filename Installer with Version string (will get stored in the Plugin SDK Download directory)
;  e.g. Plugin_Videoplayer_1.7.0.0.exe
; Second line: Source URL (http/https and 301 redirects supported)
;  e.g. https://github.com/downloads/hendrikp/Plugin_Videoplayer/Plugin_Videoplayer_1.7.0.0.exe

SectionGroup "Videoplayer" SEC_PLUGIN_VIDEOPLAYER
	Section /o "Plugin" SEC_PLUGIN_VIDEOPLAYER_MAIN
		AddSize 1500
		!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_Videoplayer/master/latestredist.info" ExecWait "/S /D="
	SectionEnd

	Section /o "Samples" SEC_PLUGIN_VIDEOPLAYER_SAMPLES
		AddSize 54000
		!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_Videoplayer/master/latestsamples.info" ExecWait "/S /D="
	SectionEnd
SectionGroupEnd

Section /o "OSC" SEC_PLUGIN_OSC
	AddSize 500
	!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_OSC/master/latestredist.info" ExecWait "/S /D="
SectionEnd

Section /o "Flite" SEC_PLUGIN_FLITE
	AddSize 14000
	!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_Flite/master/latestredist.info" ExecWait "/S /D="
SectionEnd

Section /o "D3D" SEC_PLUGIN_D3D
	AddSize 200
	!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_D3D/master/latestredist.info" ExecWait "/S /D="
SectionEnd

Section /o "Joystick" SEC_PLUGIN_JOYSTICK
	AddSize 200
	!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_Joystick/master/latestredist.info" ExecWait "/S /D="
SectionEnd

Section /o "VariousStuff" SEC_PLUGIN_VARIOUS
	AddSize 500
	!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_VariousStuff/master/latestredist.info" ExecWait "/S /D="
SectionEnd

Section /o "Crash" SEC_PLUGIN_CRASH
	AddSize 200
	!insertmacro DownloadAndExecuteLatestPluginInstaller "https://raw.github.com/hendrikp/Plugin_Crash/master/latestredist.info" ExecWait "/S /D="
SectionEnd
