@echo off

echo Generating project files for Visual Studio...

xmake project -y -m debug -k vsxmake build
