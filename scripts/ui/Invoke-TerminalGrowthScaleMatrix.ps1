[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [ValidateSet(100, 125, 150, 175, 200)]
    [int[]]$ScalePercents = @(100, 125, 150, 175, 200)
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$project = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$terminalProof = Join-Path $repoRoot 'AMSim\Saved\TerminalGrowth\starter-ga-terminal-build.png'
$phase4Root = Join-Path $repoRoot 'AMSim\Saved\Phase4'
$grownProof = Join-Path $phase4Root 'va03-terminal-passenger-flow.png'
$phase4Result = Join-Path $phase4Root 'smoke-result.json'
$galleryProof = Join-Path $repoRoot 'AMSim\Saved\TerminalGrowth\component-gallery.png'
$outputRoot = Join-Path $repoRoot 'AMSim\Saved\TerminalGrowth\ScaleMatrix'
$results = @()

foreach ($percent in $ScalePercents) {
    $scale = $percent / 100.0
    $destination = Join-Path $outputRoot $percent
    New-Item -ItemType Directory -Path $destination -Force | Out-Null

    $terminalStartedAt = [DateTime]::UtcNow
    $terminalProcess = Start-Process -FilePath $editor -ArgumentList @(
        $project,
        '-game',
        '-AMSimTerminalGrowthProof',
        '-windowed',
        '-ForceRes',
        '-ResX=1920',
        '-ResY=1080',
        '-unattended',
        '-nop4',
        "-ini:Engine:[/Script/Engine.UserInterfaceSettings]:ApplicationScale=$scale"
    ) -WindowStyle Hidden -PassThru -Wait
    if ($terminalProcess.ExitCode -ne 0 -or
        -not (Test-Path -LiteralPath $terminalProof) -or
        (Get-Item -LiteralPath $terminalProof).LastWriteTimeUtc -lt $terminalStartedAt.AddSeconds(-1)) {
        throw "Terminal proof failed or was not refreshed at $percent%."
    }
    Copy-Item -LiteralPath $terminalProof -Destination (
        Join-Path $destination 'starter-ga-terminal-build.png') -Force

    $grownStartedAt = [DateTime]::UtcNow
    $grownProcess = Start-Process -FilePath $editor -ArgumentList @(
        $project,
        '-game',
        '-AMSimPhase4Smoke',
        '-AMSimPhase1ReferenceProfile',
        '-windowed',
        '-ForceRes',
        '-ResX=1920',
        '-ResY=1080',
        '-unattended',
        '-nop4',
        "-ini:Engine:[/Script/Engine.UserInterfaceSettings]:ApplicationScale=$scale"
    ) -WindowStyle Hidden -PassThru -Wait
    if ($grownProcess.ExitCode -ne 0 -or
        -not (Test-Path -LiteralPath $grownProof) -or
        -not (Test-Path -LiteralPath $phase4Result) -or
        (Get-Item -LiteralPath $grownProof).LastWriteTimeUtc -lt $grownStartedAt.AddSeconds(-1)) {
        throw "Grown-terminal proof failed or was not refreshed at $percent%."
    }
    $phase4 = Get-Content -LiteralPath $phase4Result -Raw | ConvertFrom-Json
    if (-not $phase4.passed) {
        throw "Grown-terminal Phase 4 smoke failed at $percent%."
    }
    Copy-Item -LiteralPath $grownProof -Destination (
        Join-Path $destination 'grown-regional-terminal-operations.png') -Force

    $galleryStartedAt = [DateTime]::UtcNow
    $galleryProcess = Start-Process -FilePath $editor -ArgumentList @(
        $project,
        '-game',
        '-AMSimComponentGalleryProof',
        '-windowed',
        '-ForceRes',
        '-ResX=1920',
        '-ResY=1080',
        '-unattended',
        '-nop4',
        "-ini:Engine:[/Script/Engine.UserInterfaceSettings]:ApplicationScale=$scale"
    ) -WindowStyle Hidden -PassThru -Wait
    if ($galleryProcess.ExitCode -ne 0 -or
        -not (Test-Path -LiteralPath $galleryProof) -or
        (Get-Item -LiteralPath $galleryProof).LastWriteTimeUtc -lt $galleryStartedAt.AddSeconds(-1)) {
        throw "Component-gallery proof failed or was not refreshed at $percent%."
    }
    Copy-Item -LiteralPath $galleryProof -Destination (
        Join-Path $destination 'component-gallery.png') -Force

    $results += [ordered]@{
        scalePercent = $percent
        passed = $true
        compactMode = $percent -ge 175
        starterProof = "AMSim/Saved/TerminalGrowth/ScaleMatrix/$percent/starter-ga-terminal-build.png"
        grownProof = "AMSim/Saved/TerminalGrowth/ScaleMatrix/$percent/grown-regional-terminal-operations.png"
        componentGalleryProof = "AMSim/Saved/TerminalGrowth/ScaleMatrix/$percent/component-gallery.png"
        phase4P99FrameMilliseconds = $phase4.p99FrameMilliseconds
    }
}

$summary = [ordered]@{
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    resolution = '1920x1080'
    states = @('starter-ga-terminal-build', 'grown-regional-terminal-operations', 'component-gallery')
    results = $results
    passed = $results.Count -eq $ScalePercents.Count -and
        @($results | Where-Object { -not $_.passed }).Count -eq 0
}
New-Item -ItemType Directory -Path $outputRoot -Force | Out-Null
$summaryPath = Join-Path $outputRoot 'scale-matrix.json'
$summary | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath $summaryPath -Encoding utf8
Write-Output $summaryPath
