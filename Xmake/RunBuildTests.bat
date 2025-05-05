@echo off
REM Run `xmake test` for all build configs with default params

echo Testing ReleaseDbg...
xmake f -m releasedbg
xmake test

echo Testing Release...
xmake f -m release
xmake test

echo Testing Debug...
xmake f -m debug
xmake test
