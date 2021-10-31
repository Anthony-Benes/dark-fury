@echo off
pushd ..\
call ext_lib\bin\premake\premake5.exe vs2019
popd
pause