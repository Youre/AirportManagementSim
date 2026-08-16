[CmdletBinding()]
param(
    [string]$LauncherPath = 'D:\AMSim-Current\Windows\AMSim.exe',
    [string]$SlotId = 'VisualBaseline',
    [switch]$Refresh,
    [ValidateRange(30, 300)]
    [int]$TimeoutSeconds = 180
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$launcher = [IO.Path]::GetFullPath($LauncherPath)
$evidenceDirectory = Join-Path $repoRoot 'AMSim\Saved\VisualBaseline'
$evidencePath = Join-Path $evidenceDirectory 'visual-baseline-install.json'
$innerExecutable = Join-Path `
    (Split-Path -Parent $launcher) `
    'AMSim\Binaries\Win64\AMSim.exe'
$packageProjectRoot = Split-Path -Parent (
    Split-Path -Parent (
        Split-Path -Parent $innerExecutable))
$runtimeSaveRoot = [IO.Path]::GetFullPath(
    (Join-Path $packageProjectRoot 'Saved'))
$slotRoot = Join-Path $runtimeSaveRoot 'SaveSlots'
$sourceSlot = Join-Path $slotRoot 'Phase4Smoke'
$destinationSlot = Join-Path $slotRoot $SlotId
$phase4ResultPath = Join-Path $runtimeSaveRoot 'Phase4\smoke-result.json'

function Assert-PathWithin {
    param(
        [Parameter(Mandatory)][string]$Candidate,
        [Parameter(Mandatory)][string]$Root,
        [Parameter(Mandatory)][string]$Label
    )

    $candidateFull = [IO.Path]::GetFullPath($Candidate)
    $rootFull = [IO.Path]::GetFullPath($Root).TrimEnd('\') + '\'
    if (-not $candidateFull.StartsWith(
            $rootFull,
            [StringComparison]::OrdinalIgnoreCase)) {
        throw "$Label is outside the intended root: $candidateFull"
    }
    return $candidateFull
}

function Get-SlotIdentity {
    param([Parameter(Mandatory)][string]$Directory)

    $snapshotPath = Join-Path $Directory 'snapshot.ams'
    $metadataPath = Join-Path $Directory 'metadata.json'
    if (-not (Test-Path -LiteralPath $snapshotPath -PathType Leaf)) {
        throw "Save snapshot is missing: $snapshotPath"
    }
    if (-not (Test-Path -LiteralPath $metadataPath -PathType Leaf)) {
        throw "Save metadata is missing: $metadataPath"
    }
    $metadata = Get-Content -LiteralPath $metadataPath -Raw |
        ConvertFrom-Json
    return [ordered]@{
        slotId = [string]$metadata.slotId
        airportName = [string]$metadata.airportName
        playerLabel = [string]$metadata.playerLabel
        gameTimeMilliseconds = [int64]$metadata.gameTimeMilliseconds
        saveSchema = [int]$metadata.saveSchema
        snapshotBytes = (Get-Item -LiteralPath $snapshotPath).Length
        snapshotSha256 = (Get-FileHash `
            -LiteralPath $snapshotPath `
            -Algorithm SHA256).Hash.ToLowerInvariant()
        metadataSha256 = (Get-FileHash `
            -LiteralPath $metadataPath `
            -Algorithm SHA256).Hash.ToLowerInvariant()
    }
}

if (-not (Test-Path -LiteralPath $launcher -PathType Leaf)) {
    throw "Canonical packaged launcher is missing: $launcher"
}
if (-not (Test-Path -LiteralPath $innerExecutable -PathType Leaf)) {
    throw "Packaged inner executable is missing: $innerExecutable"
}
if ($SlotId -notmatch '^[A-Za-z0-9_-]{1,48}$') {
    throw 'SlotId must contain 1-48 letters, numbers, hyphens, or underscores.'
}

$slotRoot = Assert-PathWithin `
    -Candidate $slotRoot `
    -Root $runtimeSaveRoot `
    -Label 'Save-slot root'
$destinationSlot = Assert-PathWithin `
    -Candidate $destinationSlot `
    -Root $slotRoot `
    -Label 'Visual baseline slot'
if ((Test-Path -LiteralPath $destinationSlot) -and -not $Refresh) {
    $existing = Get-SlotIdentity -Directory $destinationSlot
    if ($existing.slotId -ne $SlotId) {
        throw "Existing slot metadata does not match $SlotId."
    }
    Write-Host "Visual baseline already installed: $destinationSlot"
    Write-Host "Snapshot SHA-256: $($existing.snapshotSha256)"
    exit 0
}

$runningGame = Get-Process -ErrorAction SilentlyContinue |
    Where-Object { $_.Path -eq $innerExecutable } |
    Select-Object -First 1
if ($runningGame) {
    throw 'Close the packaged game before regenerating the visual baseline.'
}

New-Item -ItemType Directory -Path $slotRoot -Force | Out-Null
New-Item -ItemType Directory -Path $evidenceDirectory -Force | Out-Null
if (Test-Path -LiteralPath $phase4ResultPath -PathType Leaf) {
    Remove-Item -LiteralPath $phase4ResultPath -Force
}

$arguments = @(
    '-windowed',
    '-ForceRes',
    '-ResX=1920',
    '-ResY=1080',
    '-nosound',
    '-AMSimPhase4Smoke',
    '-AMSimPhase1ReferenceProfile',
    '-AMSimPhase1PerformanceSeconds=5'
)
$launcherProcess = Start-Process `
    -FilePath $launcher `
    -ArgumentList $arguments `
    -PassThru `
    -WindowStyle Hidden
$startDeadline = [DateTime]::UtcNow.AddSeconds(20)
$gameProcess = $null
do {
    $gameProcess = Get-Process -ErrorAction SilentlyContinue |
        Where-Object { $_.Path -eq $innerExecutable } |
        Select-Object -First 1
    if (-not $gameProcess) {
        Start-Sleep -Milliseconds 100
    }
} while (-not $gameProcess -and [DateTime]::UtcNow -lt $startDeadline)
if (-not $gameProcess) {
    if (-not $launcherProcess.HasExited) {
        Stop-Process -Id $launcherProcess.Id -Force -ErrorAction SilentlyContinue
    }
    throw 'The packaged game did not start while generating the baseline.'
}

$runDeadline = [DateTime]::UtcNow.AddSeconds($TimeoutSeconds)
do {
    Start-Sleep -Milliseconds 250
    $gameProcess.Refresh()
} while (-not $gameProcess.HasExited -and [DateTime]::UtcNow -lt $runDeadline)
if (-not $gameProcess.HasExited) {
    Stop-Process -Id $gameProcess.Id -Force -ErrorAction SilentlyContinue
    throw "Visual-baseline generation exceeded $TimeoutSeconds seconds."
}
if ($null -ne $gameProcess.ExitCode -and $gameProcess.ExitCode -ne 0) {
    throw "Visual-baseline fixture exited with code $($gameProcess.ExitCode)."
}
if (-not (Test-Path -LiteralPath $phase4ResultPath -PathType Leaf)) {
    throw "Phase 4 fixture emitted no result: $phase4ResultPath"
}
$phase4Result = Get-Content -LiteralPath $phase4ResultPath -Raw |
    ConvertFrom-Json
if (-not $phase4Result.passed -or -not $phase4Result.journeyPassed) {
    throw 'The deterministic Phase 4 fixture did not pass.'
}
$sourceSlot = Assert-PathWithin `
    -Candidate $sourceSlot `
    -Root $slotRoot `
    -Label 'Phase 4 source slot'
$sourceIdentity = Get-SlotIdentity -Directory $sourceSlot

$temporarySlotId = "$SlotId-installing-$([Guid]::NewGuid().ToString('N'))"
$temporarySlot = Assert-PathWithin `
    -Candidate (Join-Path $slotRoot $temporarySlotId) `
    -Root $slotRoot `
    -Label 'Temporary visual baseline slot'
$backupSlot = $null
try {
    Copy-Item -LiteralPath $sourceSlot -Destination $temporarySlot -Recurse
    $metadataPath = Join-Path $temporarySlot 'metadata.json'
    $metadata = Get-Content -LiteralPath $metadataPath -Raw |
        ConvertFrom-Json
    $now = [DateTimeOffset]::UtcNow.ToUnixTimeSeconds()
    $metadata.slotId = $SlotId
    $metadata.playerLabel = 'Visual review baseline'
    $metadata.createdUnixSeconds = $now
    $metadata.lastPlayedUnixSeconds = $now
    $metadata.lastResult = 'Ready for visual review'
    $metadata | ConvertTo-Json -Depth 8 |
        Set-Content -LiteralPath $metadataPath -Encoding utf8

    $temporaryIdentity = Get-SlotIdentity -Directory $temporarySlot
    if ($temporaryIdentity.slotId -ne $SlotId) {
        throw 'Temporary baseline metadata failed validation.'
    }
    if ($temporaryIdentity.snapshotSha256 -ne $sourceIdentity.snapshotSha256) {
        throw 'The installed baseline snapshot differs from the fixture snapshot.'
    }

    if (Test-Path -LiteralPath $destinationSlot) {
        $backupRoot = Join-Path $runtimeSaveRoot 'VisualBaselineBackups'
        New-Item -ItemType Directory -Path $backupRoot -Force | Out-Null
        $backupSlot = Assert-PathWithin `
            -Candidate (Join-Path `
                $backupRoot `
                ("$SlotId-" + (Get-Date -Format 'yyyyMMdd-HHmmss'))) `
            -Root $backupRoot `
            -Label 'Visual baseline backup'
        Move-Item -LiteralPath $destinationSlot -Destination $backupSlot
    }
    Move-Item -LiteralPath $temporarySlot -Destination $destinationSlot
}
catch {
    if (Test-Path -LiteralPath $temporarySlot) {
        Remove-Item -LiteralPath $temporarySlot -Recurse -Force
    }
    if ($backupSlot -and
        (Test-Path -LiteralPath $backupSlot) -and
        -not (Test-Path -LiteralPath $destinationSlot)) {
        Move-Item -LiteralPath $backupSlot -Destination $destinationSlot
    }
    throw
}

$installedIdentity = Get-SlotIdentity -Directory $destinationSlot
if ($installedIdentity.slotId -ne $SlotId -or
    $installedIdentity.snapshotSha256 -ne $sourceIdentity.snapshotSha256) {
    throw 'Installed visual baseline failed its final identity check.'
}

$launcherIdentity = Get-FileHash `
    -LiteralPath $launcher `
    -Algorithm SHA256
$innerIdentity = Get-FileHash `
    -LiteralPath $innerExecutable `
    -Algorithm SHA256
[ordered]@{
    schema = 1
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    sourceCommit = (& git -C $repoRoot rev-parse HEAD).Trim()
    launcher = [ordered]@{
        path = $launcher
        sha256 = $launcherIdentity.Hash.ToLowerInvariant()
    }
    executable = [ordered]@{
        path = $innerExecutable
        sha256 = $innerIdentity.Hash.ToLowerInvariant()
    }
    fixture = [ordered]@{
        resultPath = $phase4ResultPath
        passed = [bool]$phase4Result.passed
        journeyPassed = [bool]$phase4Result.journeyPassed
        sourceSlot = $sourceIdentity
    }
    installedSlot = [ordered]@{
        path = $destinationSlot
        identity = $installedIdentity
        previousBackup = $backupSlot
    }
    preservedOtherSlots = @(
        Get-ChildItem -LiteralPath $slotRoot -Directory |
            Where-Object { $_.FullName -ne $destinationSlot } |
            Select-Object -ExpandProperty Name |
            Sort-Object
    )
    passed = $true
} | ConvertTo-Json -Depth 10 |
    Set-Content -LiteralPath $evidencePath -Encoding utf8

Write-Host "Visual baseline installed: $destinationSlot"
Write-Host "Snapshot SHA-256: $($installedIdentity.snapshotSha256)"
Write-Host "Evidence: $evidencePath"
