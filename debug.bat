@echo off
pushd .\exe
IF EXIST .\win32_shootm.exe devenv win32_shootm.exe
popd
