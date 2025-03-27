@echo off
echo Cleaning Workspace...
xmake clean

echo Testing Debug...
xmake f -m debug
xmake test

echo Cleaning Workspace...
xmake clean

echo Testing Release...
xmake f -m release
xmake test
