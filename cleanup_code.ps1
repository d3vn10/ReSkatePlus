$excluded = @(".defs", ".github", ".git", ".vs", "assets", "vs2022-x64", "cmake", "config", "contrib", "Test", "External", ".patch-files", "SpotifyHelper", "backends", "20260929", "CMakeFiles", "generated", "ci-windows")
$formatFile = Join-Path $PSScriptRoot ".clang-format"

function Get-SourceFiles($path) {
    Get-ChildItem $path -File |
        Where-Object { $_.Extension -in @(".cpp", ".h") }
    Get-ChildItem $path -Directory |
        Where-Object { $_.Name -notin $excluded } |
        ForEach-Object { Get-SourceFiles $_.FullName }
}

Get-SourceFiles $PSScriptRoot | ForEach-Object -Parallel {
    $relative = Resolve-Path -Relative $_.FullName
    Write-Output $relative
    clang-format -i --style="file:$($using:formatFile)" $_.FullName
}