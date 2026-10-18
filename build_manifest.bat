@echo off
setlocal EnableExtensions
cd Release

rem ===== CONFIGURATION =====
set "NAME=ReSkatePlus"
set "LAUNCHER=%NAME%Launcher.exe"
set "RUNTIME=%NAME%.dll"
set "MANIFEST=launcher.json"
set "VERSION=%~1"
set "OUT=complete\%VERSION%"
set "ZIP_NAME=%NAME%-v%VERSION%.zip"

echo Preparing version %VERSION%...

rem ===== CHECK FILES =====
if not exist "%LAUNCHER%" (
    echo ERROR: Launcher not found: %LAUNCHER%
    exit /b 1
)
if not exist "%RUNTIME%" (
    echo ERROR: Runtime not found: %RUNTIME%
    exit /b 1
)

if not exist "complete" mkdir "complete"
if exist "%OUT%" rmdir /s /q "%OUT%"
mkdir "%OUT%"

rem ===== HASH FILES =====
call :hashfile "%LAUNCHER%"
if errorlevel 1 exit /b 1
set "LAUNCHER_HASH=%HASH%"
for %%F in ("%LAUNCHER%") do set "LAUNCHER_SIZE=%%~zF"

call :hashfile "%RUNTIME%"
if errorlevel 1 exit /b 1
set "RUNTIME_HASH=%HASH%"
for %%F in ("%RUNTIME%") do set "RUNTIME_SIZE=%%~zF"

rem ===== GENERATE JSON =====
(
    echo {
    echo     "schema": 1,
    echo     "launcher": {
    echo         "version": "%VERSION%",
    echo         "url": "asset:%LAUNCHER%",
    echo         "sha256": "%LAUNCHER_HASH%",
    echo         "size": %LAUNCHER_SIZE%
    echo     },
    echo     "runtime": {
    echo         "version": "%VERSION%",
    echo         "url": "asset:%RUNTIME%",
    echo         "sha256": "%RUNTIME_HASH%",
    echo         "size": %RUNTIME_SIZE%
    echo     },
    echo     "game": {
    echo         "app_id": 3354750,
    echo         "depot_id": 3354751,
    echo         "manifest_id": "4621099302092747785",
    echo         "build_id": "25414733",
    echo         "skate_sha256": "fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9"
    echo     },
    echo     "depot_downloader": {
    echo         "version": "3.4.0",
    echo         "url": "https://github.com/SteamRE/DepotDownloader/releases/download/DepotDownloader_3.4.0/DepotDownloader-windows-x64.zip",
    echo         "sha256": "41c9e9f0df54b3ad02e67a11726756e5c73283bd7c2e1b04acfa5ae4c2ed3767",
    echo         "size": 33474005
    echo     }
    echo }
) > "%OUT%\%MANIFEST%"

rem ===== STAGE RELEASE FILES =====
copy /y "%LAUNCHER%" "%OUT%\%LAUNCHER%" >nul
if errorlevel 1 exit /b 1

copy /y "%RUNTIME%" "%OUT%\%RUNTIME%" >nul
if errorlevel 1 exit /b 1

rem ===== CREATE ZIP =====
powershell -NoProfile -Command "Compress-Archive -LiteralPath '%OUT%\%LAUNCHER%','%OUT%\%RUNTIME%','%OUT%\%MANIFEST%' -DestinationPath '%OUT%\%ZIP_NAME%' -Force"
if errorlevel 1 exit /b 1

echo.
echo Release generated successfully:
echo %ZIP_NAME%
echo     =^> ..\%LAUNCHER%
echo     =^> ..\%RUNTIME%
echo     =^> ..\%MANIFEST%
exit /b 0

:hashfile
set "HASH="
for /f "skip=1 tokens=*" %%H in ('certutil -hashfile "%~1" SHA256') do if not defined HASH set "HASH=%%H"
if not defined HASH (
    echo ERROR: Failed to hash %~1
    exit /b 1
)
exit /b 0
```