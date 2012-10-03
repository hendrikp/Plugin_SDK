Plugin SDK for CryEngine SDK
=====================================
This is still a work in progress, a plugin sample and plugins themselves will follow.

Tested on CryEngine 3 FreeSDK Version 3.4 (32/64 Bit)

Purpose is to create a tailored way for automatically loading plugins.
For redistribution please see license.txt.

Feature requests/latest version on github.
https://github.com/hendrikp/Plugin_SDK

Installation / Integration
==========================
Extract the files to your Cryengine SDK Folder so that the Code and BinXX/Plugins directory match up.

Its now possible to have plugins in their own plugin subdirectory (e.g. "C:\ce3.4.0\Bin32\Plugins\Plugin_Sample\Plugin_Sample.dll") to handle plugins with many additional link library dependencies.

If you have a custom GameDll that doesn't contain the PluginManager yet then you will need to recompile it see C++ Integration.

Installing the Visual Studio 2010 Plugin Wizard
-----------------------------------------------
An installer is planned, until then:

* Copy the following 3 files to "Documents\Visual Studio 2010\Wizards\PluginWizard"
  * PluginWizard.vsz
  * PluginWizard.vsdir
  * PluginWizard.ico

* Open the file "PluginWizard.vsz" with a text editor
  * Fix the path so it points to your Plugin SDK Wizard installation directory 
    ```Param="ABSOLUTE_PATH = C:\cryengine3_3.4.0\Code\Plugin_SDK\wizard\vc10"```

* The Wizard should now work, please note the following things (otherwise the wizard won't work)
  * the project name shouldn't contain "Plugin" as this will be added automatically
  * untick the checkbox to create solution directory
  * only create projects in the Code directory e.g. "C:\cryengine3_3.4.0\Code"

Creating a new plugin
=====================
Please choose a unique name for your plugin, each plugin must have its own name.

For now please use as base 
https://github.com/hendrikp/Plugin_Crash (Search & Replace "Crash" with your plugin name)

In the future there will be a Visual Studio plugin wizard.

CVars
=====
* ```pm_list```
  List one info row for all plugins
* ```pm_dump```
  Dump info on a specific plugins
* ```pm_dumpall```
  Dump info on all plugins
* ```pm_unload```
  Unload a specific plugin using its name. Works only limited for plugins containing resources that are in use (the CryEngine usage of those objects needs to be fully cleaned up beforehand).
* ```pm_unloadall```
  Unload all plugins with the exception of the plugin manager. Works only limited for plugins containing resources that are in use (the CryEngine usage of those objects needs to be fully cleaned up beforehand).
* ```pm_reload```
  Reload a specific plugin using its path
* ```pm_reloadall```
  Reload all plugins

C++ Integration
===============
Follow these steps to integrate the Plugin SDK into your GameDLL.

Repository Setup
----------------
* Clone the repository to your local machine to your Code directory e.g. C:\cryengine3_3.4.0\Code

```
    git clone https://github.com/hendrikp/Plugin_SDK.git
```

Compiler Settings
-----------------
* Open the Solution CryEngine_GameCodeOnly.sln
* Open CryGame Properties
* Set the Dropdowns to All Configurations, All Platforms
* Add to C/C++ -> General -> Additional Include Directories:

```
    ;$(SolutionDir)..\Plugin_SDK\inc
```

* Apply those properties and close the dialog

Source Files
------------
* Inside the CryGame Project (Solution Explorer)
  Open the following File: Startup Files / GameStartup.cpp
* Add behind the existing includes the following:

```C++
    #include <IPluginManager_impl.h>
```

* Find the function CGameStartup::Init
* Add in the middle of the function (before ```REGISTER_COMMAND("g_loadMod", RequestLoadMod,VF_NULL,"");```) the following:

```C++
	PluginManager::InitPluginManager(startupParams);
	PluginManager::InitPluginsBeforeFramework();
    REGISTER_COMMAND("g_loadMod", RequestLoadMod,VF_NULL,""); // <--
```

* The initialization of the plugin manager can only happen once because its a singleton.
  Also don't forget to supply the SDK Version as parameter so the plugins can check compatiblity.
* Add at the end of the function (before ```return pOut;```) the following:

```C++
    PluginManager::InitPluginsLast();
    return pOut; // <--
```

Compiling
---------
Either add the project file to your CryGame solution and the plugin manager as dependency for each plugin.

Or use the supplied seperate solution to compile the plugin manager.

Code Style
----------
In general look how its done in the CryEngine FreeSDK. Exceptions prove the rule.

* For variables (e.g. sTestMe or m_sTestMe)
  * lower camel casing
  * hungarian notation
  * m_ prefix for member variables
  * g prefix for global variables (in general dont use globals and when still needed then static)
* For functions (e.g. DoSomething)
  * upper camel casing
* Naming conventions
  * class declarations begin with C
  * structur declarations begin with S
  * interface declarations begin with I
  * char*/all string definitions with s
  * pointer defintions begin with p
  * integers defintions begin with n/i
  * floats begin with f
  * doubles begin with d
  * rest can be looked up e.g. here http://www.yolinux.com/TUTORIALS/LinuxTutorialC++CodingStyle.html
* Macros in capitals and words seperated by _
* Dont use RTTI
* Everything should be in a namespace
* Use tools/stylecode.bat before committing changes
* The rest will be fixed when your pull gets merged

Contribution Guideline
----------------------
* Change as little as possible to have the effect you want
* Discuss major/version changes beforehand
* Update the doxygen documentation

Contributing
------------
* Fork it
* Create a branch (`git checkout -b my_PluginSDK`)
* Put your name into authors.txt
* Commit your changes (`git commit -am "Added ...."`)
* Push to the branch (`git push origin my_PluginSDK`)
* Open a Pull Request


