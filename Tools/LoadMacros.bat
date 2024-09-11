@echo off

echo Loading xmake macros

xmake macro --import=Xmake/Macros/BuildAllConfigs.lua BuildAllConfigs
xmake macro --import=Xmake/Macros/Clean.lua Clean
xmake macro --import=Xmake/Macros/DeepClean.lua DeepClean
xmake macro --import=Xmake/Macros/GenerateSolutionVS2022.lua GenVS

echo Finished loading macros