set Target=ShipDevelop
set PluginName=Plugin_Videoplayer
set Source=..\
set GameDir=..\..\..

set PluginName2=Plugin_D3D
set Source2=..\..\%PluginName2%

mkdir %Target%
mkdir %Target%\Code
mkdir %Target%\Code\%PluginName%
mkdir %Target%\Code\%PluginName%\inc

xcopy %Source%\inc %Target%\Code\%PluginName%\inc /E /Y

copy %Source%\license.txt %Target%\Code\%PluginName%
copy %Source%\patents.txt %Target%\Code\%PluginName%
copy %Source%\readme_developer.txt %Target%\Code\%PluginName%\readme.txt
copy %Source%\readme_developer.txt %Target%\readme.txt
copy %Source%\changelog.txt %Target%\Code\%PluginName%

mkdir %Target%\Code\%PluginName2%

copy %Source2%\ID3DSystem.h %Target%\Code\%PluginName2%
copy %Source2%\ID3DSystem_impl.h %Target%\Code\%PluginName2%
copy %Source2%\D3DPlugin_Sample_impl.h %Target%\Code\%PluginName2%
copy %Source2%\license.txt %Target%\Code\%PluginName2%
copy %Source2%\readme.txt %Target%\Code\%PluginName2%
copy %Source2%\changelog.txt %Target%\Code\%PluginName2%

pause