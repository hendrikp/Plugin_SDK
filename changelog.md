Plugin SDK 2.1.0 (02.10.2014)
-----------------------------
Stable Release for CryEngine 3.6.8+ (32/64 bit)

New:
* Support for update/delays during editing
* Included hooking tools

Plugin SDK 2.0.2 (06.09.2014)
-----------------------------
Stable Release for CryEngine 3.6.6+ (32/64 bit)
There is no binary release it requires compilation.

Changes:
* Reduced game dependencies
* Now using Visual Studio 2012 project

Fixes:
* Debug Build compilable out of the box

Plugin SDK 2.0.1 (17.08.2014)
-----------------------------
Stable Release for CryEngine 3.5.X - 3.6.5+ (32/64 bit)
There is no binary release it requires compilation.

New:
* Built in support for the GameObjectExtenstion REGISTER macros
 (custom entity class inside of plugin)

Plugin SDK 2.0.0 (29.11.2013)
-----------------------------
Stable Release for CryEngine 3.5.4 (32/64 bit)

New:
* Plugin Wizard for Visual Studio 2012
* Plugin Wizard for Visual Studio 2013
* Basic Camera plugin in installer
* More optional components in installer

Changes:
* Updated indenter for plugin wizards

Fixes:
* Manager buildable from path with spaces now (don't recommend it though, because its not officially supported by Sandbox)

Thanks to all who tested and contributed to the 3.5.4 port,
it wouldn't have been possible without your support.

Plugin SDK 1.3.0 (02.03.2013)
-----------------------------
Stable Release for CryEngine 3.4.5 (32/64 bit)

New:
* Build version scripts (detection of CryEngine version on rebuild)
* UCS-2 conversions utils

Plugin SDK 1.2.0 (08.02.2013)
-----------------------------
Stable Release for CryEngine 3.4.4 (32/64 bit)

New:
* Support for CryEngine 3.4.4

Changes:
* CDKVersion header to keep backward compatibility

Installer:
* All redistributable binaries moved from github to bintray

Plugin SDK 1.1.0 (19.11.2012)
-----------------------------
Stable Release for CryEngine 3.4.3 (32/64 bit)

New:
* Support for CryEngine 3.4.3
* Support to delay/loop cvar/lua/C commands/functions

Installer:
* Can now directly download plugins
* Check for correct SDK version
* Silent Mode

Changes:
* New game type registration interface
* Wizard adjusted for new registration interface
* Wizard generates nsis installer script

Fixes:
* Sandbox CryDark skin didn't load

Plugin SDK 1.0.1 (29.10.2012)
-----------------------------
Stable Release for CryEngine 3.4.0 (32/64 bit)

Fix:
* Included a missing wizard file (project/Plugin_Settings.props)

Known Limitations:
* When compiled for 3.4.3 flownodes don't register correctly so use Plugin SDK version 1.1 instead

Plugin SDK 1.0 (29.10.2012)
---------------------------
Initial Stable Release for CryEngine 3.4.0 (32/64 bit)
