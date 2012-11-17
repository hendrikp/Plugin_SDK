; Contains the plugins that can be downloaded by the Plugin SDK installer
; The files need to be executable nsis installers
;  msi or other installers is also possible but may require custom parameters

Section "Videoplayer 1.7" SEC_PLUGIN_VIDEOPLAYER
	!insertmacro DownloadAndExecutePluginInstaller "https://github.com/downloads/hendrikp/Plugin_Videoplayer/Plugin_Videoplayer_1.6.2.zip" "$PLUGINDOWNLOADDIR\Plugin_Videoplayer_1.6.2.zip"
	!insertmacro DownloadAndExecutePluginInstaller "https://github.com/downloads/hendrikp/Plugin_Videoplayer/Plugin_Videoplayer_1.6.2_Samples.zip" "$PLUGINDOWNLOADDIR\Plugin_Videoplayer_1.6.2_Samples.zip"
SectionEnd

Section "OSC 1.1" SEC_PLUGIN_OSC
SectionEnd

Section "Flite 1.1" SEC_PLUGIN_FLITE
SectionEnd

Section "D3D 1.7" SEC_PLUGIN_D3D
SectionEnd

Section "Joystick 1.1" SEC_PLUGIN_JOYSTICK
SectionEnd

Section "Crash 1.1" SEC_PLUGIN_CRASH
SectionEnd
