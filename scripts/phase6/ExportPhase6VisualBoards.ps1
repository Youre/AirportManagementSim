[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$source = Join-Path $projectRoot 'docs\planning\50-production\phase6-visual-validation\board-source.html'
$output = Join-Path $projectRoot 'docs\planning\50-production\phase6-visual-validation'
$chrome = 'C:\Program Files\Google\Chrome\Application\chrome.exe'

if (!(Test-Path -LiteralPath $source)) {
    throw "Phase 6 visual source is missing: $source"
}
if (!(Test-Path -LiteralPath $chrome)) {
    throw "Chrome is required for deterministic board export: $chrome"
}

$boards = [ordered]@{
    runways    = 'p6-01-parallel-runway-operations.png'
    terminal   = 'p6-02-high-capacity-terminal-flow.png'
    turnaround = 'p6-02-widebody-turnaround.png'
    major      = 'p6-03-major-capability.png'
    incident   = 'p6-04-serious-incident-continuity.png'
    scale      = 'p6-05-maximum-scale-overview.png'
}
$sourceUri = [Uri]::new($source).AbsoluteUri
foreach ($entry in $boards.GetEnumerator()) {
    $destination = Join-Path $output $entry.Value
    $arguments = @(
        '--headless',
        '--disable-gpu',
        '--hide-scrollbars',
        '--force-device-scale-factor=1',
        '--allow-file-access-from-files',
        '--window-size=1920,1080',
        "--screenshot=$destination",
        "${sourceUri}?board=$($entry.Key)"
    )
    & $chrome @arguments | Out-Null
    if ($LASTEXITCODE -ne 0 -or !(Test-Path -LiteralPath $destination)) {
        throw "Failed to export Phase 6 board: $($entry.Key)"
    }
}

Write-Host "Exported $($boards.Count) Phase 6 visual boards."
