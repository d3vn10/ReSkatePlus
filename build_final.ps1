param([string]$Version)
$ErrorActionPreference = "Stop"
if ([string]::IsNullOrWhiteSpace($Version)) {
    Write-Error "No version provided, and 'current.version' is empty or missing."
    exit 1
}
Write-Host ""
Write-Host "Preparing version $Version..."
Set-Location "build\ci-windows\Release"
foreach ($File in @("ReSkateLauncher.exe", "ReSkate.dll")) {
	if (-not (Test-Path -LiteralPath $File -PathType Leaf)) {
		throw "File not found: $File"
	}
}
if (-not (Test-Path -LiteralPath "releases" -PathType Container)) {
	New-Item -ItemType Directory -Path "releases" | Out-Null
}
if (-not (Test-Path -LiteralPath "complete" -PathType Container)) {
	New-Item -ItemType Directory -Path "complete" | Out-Null
}
$Out = Join-Path "releases" $Version
if (Test-Path -LiteralPath $Out) {
	Remove-Item -LiteralPath $Out -Recurse -Force
}
try {
    New-Item -ItemType Directory -Path $Out | Out-Null
    $ManifestData = [ordered]@{
        schema = 1
        launcher = [ordered]@{
            version = $Version
            url = "asset:ReSkateLauncher.exe"
            sha256 = (Get-FileHash -LiteralPath "ReSkateLauncher.exe" -Algorithm SHA256).Hash.ToLowerInvariant()
            size = (Get-Item -LiteralPath "ReSkateLauncher.exe").Length
        }
        runtime = [ordered]@{
            version = $Version
            url = "asset:ReSkate.dll"
            sha256 = (Get-FileHash -LiteralPath "ReSkate.dll" -Algorithm SHA256).Hash.ToLowerInvariant()
            size = (Get-Item -LiteralPath "ReSkate.dll").Length
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
            url = "https://github.com/SteamRE/DepotDownloader/releases/download/DepotDownloader_3.4.0/DepotDownloader-windows-x64.zip"
            sha256 = "41c9e9f0df54b3ad02e67a11726756e5c73283bd7c2e1b04acfa5ae4c2ed3767"
            size = 33474005
        }
    }
    $ManifestPath = Join-Path $Out "launcher.json"
    $ManifestData |
        ConvertTo-Json -Depth 5 |
        Set-Content -LiteralPath $ManifestPath -Encoding utf8
    Copy-Item -LiteralPath "ReSkateLauncher.exe" -Destination $Out -Force
    Copy-Item -LiteralPath "ReSkate.dll" -Destination $Out -Force
    $ZipPath = Join-Path $Out "ReSkatePlus-v$Version.zip"
    Compress-Archive -LiteralPath @(
        (Join-Path $Out "ReSkateLauncher.exe"),
        (Join-Path $Out "ReSkate.dll"),
        $ManifestPath
    ) -DestinationPath $ZipPath -Force
	Write-Host ""
    Write-Host "Release generated successfully:"
	Write-Host ""
	Write-Host "ReSkate-v$Version.zip"
	Write-Host "    => ReSkate.dll"
	Write-Host "    => ReSkateLauncher.exe"
	Write-Host "    => launcher.json"
	Start-Process explorer.exe -ArgumentList (Resolve-Path -LiteralPath $Out).Path
	Set-Location "..\..\.."
    exit 0
}
catch {
	Set-Location "..\..\.."
    [Console]::Error.WriteLine("ERROR: $($_.Exception.Message)")
    exit 1
}
