param(
    [string]$Version,
    [string]$BuildPath = "build/vs2022-x64/Release" # Defaults to project workspace relative layout
)

$VersionFile = Join-Path $PSScriptRoot "current.version"
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Version)) {
    $CurrentVersion = if (Test-Path -LiteralPath $VersionFile) {
        (Get-Content -LiteralPath $VersionFile -Raw).Trim()
    } else { 
        "0.0.0.0"
    }
    Write-Host "No version entered."
    Write-Host "Leave blank to build version '$CurrentVersion'..."
    $Version = Read-Host "Or enter version now"
    if ([string]::IsNullOrWhiteSpace($Version)) {
        $Version = $CurrentVersion
    } else {
        Set-Content -LiteralPath $VersionFile -Value $Version -NoNewline
    }
}
else {
    Set-Content -LiteralPath $VersionFile -Value $Version -NoNewline
}

if ([string]::IsNullOrWhiteSpace($Version)) {
    Write-Error "No version provided, and 'current.version' is empty or missing."
    exit 1
}

# Resolve the clean input build location path completely safely
$TargetBuildFolder = (Resolve-Path $BuildPath -ErrorAction Ignore).Path
if (-not $TargetBuildFolder) {
    # Fallback to absolute workspace joining if paths are relative but unbuilt
    $TargetBuildFolder = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot $BuildPath))
}

# Navigate safely to the automated environment context instead of using a hardcoded local machine string
if (-not (Test-Path -LiteralPath $TargetBuildFolder -PathType Container)) {
    Write-Error "Target compilation build folder not found: $TargetBuildFolder"
    exit 1
}
Set-Location $TargetBuildFolder

$Out = Join-Path "complete" $Version
try {
    Write-Host ""
    Write-Host "Preparing version $Version inside $TargetBuildFolder..."
    foreach ($File in @("ReSkatePlusLauncher.exe", "ReSkatePlus.dll", "ReSkatePlusServer.exe")) {
        if (-not (Test-Path -LiteralPath $File -PathType Leaf)) {
            throw "File not found: $File"
        }
    }
    if (-not (Test-Path -LiteralPath "complete" -PathType Container)) {
        New-Item -ItemType Directory -Path "complete" | Out-Null
    }
    if (Test-Path -LiteralPath $Out) {
        Remove-Item -LiteralPath $Out -Recurse -Force
    }
    New-Item -ItemType Directory -Path $Out | Out-Null
    
    $ManifestData = [ordered]@{
        schema = 1
        launcher = [ordered]@{
            version = $Version
            url = "asset:ReSkatePlusLauncher.exe"
            sha256 = (Get-FileHash -LiteralPath "ReSkatePlusLauncher.exe" -Algorithm SHA256).Hash.ToLowerInvariant()
            size = (Get-Item -LiteralPath "ReSkatePlusLauncher.exe").Length
        }
        runtime = [ordered]@{
            version = $Version
            url = "asset:ReSkatePlus.dll"
            sha256 = (Get-FileHash -LiteralPath "ReSkatePlus.dll" -Algorithm SHA256).Hash.ToLowerInvariant()
            size = (Get-Item -LiteralPath "ReSkatePlus.dll").Length
        }
        server = [ordered]@{
            version = $Version
            url = "asset:ReSkatePlusServer.exe"
            sha256 = (Get-FileHash -LiteralPath "ReSkatePlusServer.exe" -Algorithm SHA256).Hash.ToLowerInvariant()
            size = (Get-Item -LiteralPath "ReSkatePlusServer.exe").Length
        }
        game = [ordered]@{
            app_id = 3354750
            depot_id = 3354751
            manifest_id = "4621099302092747785"
            build_id = "25414733"
            skate_sha256 = "fbce74d5e28ef525dbba2cb4adbebc13405bdbd88f31bc940bca45e4ae88b8f9"
        }
        depot_downloader = [ordered]@{
            version = "3.4.0"
            url = "https://github.com"
            sha256 = "41c9e9f0df54b3ad02e67a11726756e5c73283bd7c2e1b04acfa5ae4c2ed3767"
            size = 33474005
        }
    }
    
    $ManifestPath = Join-Path $Out "launcher.json"
    $ManifestData |
        ConvertTo-Json -Depth 5 |
        Set-Content -LiteralPath $ManifestPath -Encoding utf8
        
    Copy-Item -LiteralPath "ReSkatePlusLauncher.exe" -Destination $Out -Force
    Copy-Item -LiteralPath "ReSkatePlus.dll" -Destination $Out -Force
    Copy-Item -LiteralPath "ReSkatePlusServer.exe" -Destination $Out -Force
    
    $ZipPath = Join-Path $Out "ReSkatePlus-v$Version.zip"
    Compress-Archive -LiteralPath @(
        (Join-Path $Out "ReSkatePlusLauncher.exe"),
        (Join-Path $Out "ReSkatePlus.dll"),
        (Join-Path $Out "ReSkatePlusServer.exe"),
        $ManifestPath
    ) -DestinationPath $ZipPath -Force
    
    Write-Host ""
    Write-Host "Release generated successfully:"
    Write-Host "ReSkatePlus-v$Version.zip"
    Write-Host "    => ReSkatePlusLauncher.exe"
    Write-Host "    => ReSkatePlus.dll"
    Write-Host "    => ReSkatePlusServer.exe"
    Write-Host "    => launcher.json"
    
    # Only pop open File Explorer if running interactively on a local machine
    if ([Environment]::UserInteractive) {
        Start-Process explorer.exe -ArgumentList (Resolve-Path -LiteralPath $Out).Path
    }
    
    Set-Location $PSScriptRoot
    exit 0
}
catch {
    Set-Location $PSScriptRoot
    [Console]::Error.WriteLine("ERROR: $($_.Exception.Message)")
    exit 1
}
