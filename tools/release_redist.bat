set Target=ShipRedist
set PluginName=Plugin_Videoplayer
set Source=..\
set GameDir=..\..\..

mkdir %Target%
mkdir %Target%\Bin32
mkdir %Target%\Bin64

copy %Source%\license.txt %Target%
copy %Source%\patents.txt %Target%
copy %Source%\readme.txt %Target%
copy %Source%\changelog.txt %Target%

copy %GameDir%\Bin32\Plugin_D3DSystem.dll %Target%\Bin32
copy %GameDir%\Bin64\Plugin_D3DSystem.dll %Target%\Bin64
copy %GameDir%\Bin32\CryGame.dll %Target%\Bin32
copy %GameDir%\Bin64\CryGame.dll %Target%\Bin64
copy %GameDir%\Bin32\Plugin_Videoplayer.dll %Target%\Bin32
copy %GameDir%\Bin64\Plugin_Videoplayer.dll %Target%\Bin64

pause