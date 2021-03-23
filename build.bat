:::::
::
::  Windows C++ Project Builder. (DOS/CMD)
::
::  Author: Omar M.Gindia
::  Date:   13-1-2021
::
::  Sets up folders/directories and builds with MSVC compiler "CL" and clangd lsp
::
:::::
@echo off
Set _fGreen=[32m
Set _fBWhite=[97m
Set _fBBlue=[94m
:: timer start.
set /A _tic=%time:~0,2%*3600^
            +%time:~3,1%*10*60^
            +%time:~4,1%*60^
            +%time:~6,1%*10^
            +%time:~7,1% >nul

:: building script

IF NOT EXIST .\exe mkdir .\exe
ROBOCOPY .\bin\x64 .\exe *.dll /NDL /NJH /NJS /nc /ns /np

echo %_fBWhite%
echo ----------------------------------------
echo ---------  %_fGreen%COMPILING MSVC %_fBBlue%x64%_fBWhite%  ---------
echo ----------------------------------------
pushd exe


set nameflag= -Fe:.\win32_shootm.exe
set cflags= -MT -nologo -Gm- -GR- -EHa- -fp:fast -FC -Z7 -Oi -Od -WX -W4 -wd4100 -wd4244 -EHsc
::-showIncludes
set lflags= -INCREMENTAL:no -opt:ref -SUBSYSTEM:WINDOWS,5.02

:: all the passed libs
set libs=..\lib\x64\SDL2.lib ..\lib\x64\SDL2_image.lib user32.lib

:: all the cpp/c files thats needed for the build
set compile_units= ..\src\win32_shootm.c

cl %nameflag% %cflags% %compile_units% %libs% -I..\include -link %lflags%

popd


:: finished
set /A _toc=%time:~0,2%*3600^
            +%time:~3,1%*10*60^
            +%time:~4,1%*60^
            +%time:~6,1%*10^
            +%time:~7,1% >nul

set /A _elapsed=%_toc%-%_tic

echo ----------------------------------------
echo %_fGreen% %_elapsed%  seconds.%_fBWhite%
echo ----------------------------------------
