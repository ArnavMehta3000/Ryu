@echo off

REM Check if current directory has Docs folder
IF NOT EXIST Docs (
	echo Docs folder not found. Please check the current directory.
	exit /b 1
)

cd ../Docs
REM Run Doxygen to generate XML documentation
doxygen.exe ./Docs/Doxyfile
