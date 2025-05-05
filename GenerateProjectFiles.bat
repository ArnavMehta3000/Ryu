@echo off
REM Pass'x' to run the script in extended mode
powershell -ExecutionPolicy Bypass -File ./Xmake/GenerateProjectFiles.ps1 %*
