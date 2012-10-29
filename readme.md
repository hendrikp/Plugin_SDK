Plugin SDK for CryEngine
========================
Purpose is to automatically load plugins without touching the GameDll each time. Developers can use the wizard to save time when setting up plugin projects.

Available Plugins
-----------------
* See [Wiki: Plugin SDK](https://github.com/hendrikp/Plugin_SDK/wiki)

Feature requests/bug reports on github:
 [Wishes / Issues](https://github.com/hendrikp/Plugin_SDK/issues)

Installation / Integration
==========================
Extract the files to your Cryengine SDK Folder so that the Code and BinXX/Plugins directory match up.

* If you have a custom GameDll that doesn't contain the PluginManager yet then you will need to recompile it see [Wiki: GameDLL Integration](https://github.com/hendrikp/Plugin_SDK/wiki/GameDLL-Integration).
* If you don't use the Standard FreeSDK version but e.g. got a licensed, educational, custom, newer or older version then you might also need to recompile all plugins and the plugin manager.
  * Adjust the SDK version in ```#define PLUGIN_COMPILED_CDK_VERSION "3.4.0"``` in the file "Plugin_SDK\inc\IPluginBase.h"
  * Plugins might implement their own ```Check``` Method in this case you might have to adjust the versions used there to match your SDK version.
  * If the SDK API/ABI didn't change and the version stayed the same then the plugin might be compatible
    (depends on the buildnr containing breaking changes or different configuration) 

CVars / Commands
================
* See [Wiki: CVar Commands](https://github.com/hendrikp/Plugin_SDK/wiki/CVar-Commands)

Creating a new plugin
=====================
* See [Wiki: Manual Plugin Wizard Installation](https://github.com/hendrikp/Plugin_SDK/wiki/Manual-Plugin-Wizard-Installation)
* See [Wiki: Creating a New Plugin](https://github.com/hendrikp/Plugin_SDK/wiki/Creating-a-new-Plugin)
* You can also look into this sample [Plugin_Crash](https://github.com/hendrikp/Plugin_Crash)

For plugin dependencies
-----------------------
* See [Wiki: Plugin Dependencies](https://github.com/hendrikp/Plugin_SDK/wiki/Plugin-Dependencies)
* [Plugin_D3D](https://github.com/hendrikp/Plugin_D3D) (See C++ Integration)
* [Plugin_Videoplayer](https://github.com/hendrikp/Plugin_Videoplayer) (Optionally uses Plugin_D3D)

Building / Compiling
--------------------
* See [Wiki: Build System](https://github.com/hendrikp/Plugin_SDK/wiki/Build-System)

Contributing
============
* See [Wiki: Contribution Guideline](https://github.com/hendrikp/Plugin_SDK/wiki/Contribution-Guideline)
