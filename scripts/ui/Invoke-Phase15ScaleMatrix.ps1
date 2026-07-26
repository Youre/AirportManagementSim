[CmdletBinding()]
param(
    [string]$EngineRoot = 'C:\Program Files\Epic Games\UE_5.8',
    [string]$OutputRoot = '',
    [ValidateSet(100, 125, 150, 175, 200)]
    [int[]]$ScalePercents = @(100, 125, 150, 175, 200),
    [switch]$SummarizeOnly
)

$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$project = Join-Path $repoRoot 'AMSim\AMSim.uproject'
$editor = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$savedRoot = Join-Path $repoRoot 'AMSim\Saved\Phase1'
$outputRoot = if ($OutputRoot) {
    [IO.Path]::GetFullPath($OutputRoot)
} else {
    Join-Path $savedRoot 'Scale-Phase15Final'
}
$savedBoundary = [IO.Path]::GetFullPath((Join-Path $repoRoot 'AMSim\Saved'))
if (-not $outputRoot.StartsWith(
    $savedBoundary.TrimEnd('\') + '\',
    [StringComparison]::OrdinalIgnoreCase)) {
    throw "Scale evidence must remain under AMSim/Saved: $outputRoot"
}

$proofNames = @(
    'va01-new-airport.png',
    'va02-construction.png',
    'va04-offer.png',
    'va05-turnaround.png',
    'phase1-complete.png',
    'smoke-result.json'
)
$results = @()
foreach ($percent in $(if ($SummarizeOnly) { @() } else { $ScalePercents })) {
    $scale = $percent / 100.0
    $destination = Join-Path $outputRoot $percent
    New-Item -ItemType Directory -Path $destination -Force | Out-Null
    $startedAt = [DateTime]::UtcNow
    $process = Start-Process `
        -FilePath $editor `
        -ArgumentList @(
            $project,
            '-game',
            '-AMSimPhase1Smoke',
            '-AMSimPhase1ReferenceProfile',
            '-windowed',
            '-ForceRes',
            '-ResX=1920',
            '-ResY=1080',
            '-unattended',
            '-nop4',
            "-ini:Engine:[/Script/Engine.UserInterfaceSettings]:ApplicationScale=$scale"
        ) `
        -WindowStyle Hidden `
        -PassThru `
        -Wait
    if ($process.ExitCode -ne 0) {
        throw "Rendered smoke exited with code $($process.ExitCode) at $percent%."
    }

    $smokePath = Join-Path $savedRoot 'smoke-result.json'
    if (-not (Test-Path -LiteralPath $smokePath)) {
        throw "Rendered smoke did not create $smokePath."
    }
    $smokeFile = Get-Item -LiteralPath $smokePath
    if ($smokeFile.LastWriteTimeUtc -lt $startedAt.AddSeconds(-1)) {
        throw "Rendered smoke result was not refreshed at $percent%."
    }
    $smoke = Get-Content -LiteralPath $smokePath -Raw | ConvertFrom-Json
    if (-not $smoke.passed) {
        throw "Rendered smoke failed at $percent%."
    }
    foreach ($proofName in $proofNames) {
        Copy-Item `
            -LiteralPath (Join-Path $savedRoot $proofName) `
            -Destination (Join-Path $destination $proofName) `
            -Force
    }
    $results += [ordered]@{
        scalePercent = $percent
        passed = $smoke.passed
        p99FrameMilliseconds = $smoke.p99FrameMilliseconds
        maximum8xBacklogSteps = $smoke.maximum8xBacklogSteps
        proofDirectory = $destination
    }
}

$availableResults = @()
foreach ($availablePercent in @(100, 125, 150, 175, 200)) {
    $availableDirectory = Join-Path $outputRoot $availablePercent
    $availableSmokePath = Join-Path $availableDirectory 'smoke-result.json'
    if (Test-Path -LiteralPath $availableSmokePath) {
        $availableSmoke = Get-Content -LiteralPath $availableSmokePath -Raw | ConvertFrom-Json
        $availableResults += [ordered]@{
            scalePercent = $availablePercent
            passed = $availableSmoke.passed
            p99FrameMilliseconds = $availableSmoke.p99FrameMilliseconds
            maximum8xBacklogSteps = $availableSmoke.maximum8xBacklogSteps
            proofDirectory = $availableDirectory
        }
    }
}
$summary = [ordered]@{
    generatedAtUtc = [DateTime]::UtcNow.ToString('o')
    resolution = '1920x1080'
    results = $availableResults
}
$summaryPath = Join-Path $outputRoot 'scale-matrix.json'
$summary | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath $summaryPath -Encoding utf8
Write-Output $summaryPath
