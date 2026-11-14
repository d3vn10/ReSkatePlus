@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --preset ci-windows
if %errorlevel% neq 0 exit /b %errorlevel%

cmake --build --preset release --parallel 4
if %errorlevel% neq 0 exit /b %errorlevel%