param([string]$Version)

Set-Location ..\..

cmake.exe -S . -B build\vs2022-x64 -DDINGOSDK_VERSION="$Version"
if ($LASTEXITCODE -ne 0) { 
	Set-Location build\vs2022-x64
	exit $LASTEXITCODE 
}

cmake.exe --build --preset release --parallel 4
if ($LASTEXITCODE -ne 0) { 
	Set-Location build\vs2022-x64
	exit $LASTEXITCODE 
}

Set-Location build\vs2022-x64
.\build_manifest.bat $Version